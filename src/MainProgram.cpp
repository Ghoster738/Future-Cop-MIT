#include "MainProgram.h"

#include "Config.h"
#include "InputMenu.h"

#include <iostream>


const std::string MainProgram::CUSTOM_IDENTIFIER = "custom-map";

MainProgram::MainProgram( int argc, char** argv ) : parameters( argc, argv ), paths( parameters ), options( paths, parameters ) {
    this->play_loop = true;

    // Set everything to null.
    this->global_r         = nullptr;
    this->resource_r       = nullptr;
    this->environment_p    = nullptr;
    this->text_2d_buffer_r = nullptr;
    this->first_person_r   = nullptr;
    this->control_system_p = nullptr;
    this->sound_system_p   = nullptr;
    this->menu_r           = nullptr;
    this->primary_game_r   = nullptr;

    if( parameters.help.getValue() ) {
        return;
    }

    // TODO: Use venice beach as this map has all three types vertex animations.
    this->resource_identifier = Data::Manager::pa_urban_jungle;
    this->platform = Data::Manager::getPlatformFromString( this->options.getCurrentPlatform() );

    this->switch_to_platform = this->platform;

    if(this->options.getLoadAllMaps())
        this->importance_level = Data::Manager::Importance::NOT_NEEDED;
    else
        this->importance_level = Data::Manager::Importance::NEEDED;

    setupLogging();
    initGraphics();
    setupGraphics();
    initSound();
    setupSound();
    loadResources();
    loadGraphics();
    loadSound();
    setupCamera();
    setupControls();
}

void MainProgram::displayLoop() {
    auto last_time = std::chrono::high_resolution_clock::now();
    auto this_time = last_time;
    auto delta = this_time - last_time;

    while( this->play_loop ) {
        this_time = std::chrono::high_resolution_clock::now();
        delta = this_time - last_time;
        float delta_f = std::chrono::duration<float, std::ratio<1>>( delta ).count();

        // Make the control system poll all the inputs.
        if( menu_r != &InputMenu::input_menu )
            control_system_p->advanceTime( delta_f );

        // Update menu_r and primary_game_r
        if( menu_r != nullptr ) {
            menu_r->update( *this, std::chrono::duration_cast<std::chrono::microseconds>(delta) );
        }
        else
        if( primary_game_r != nullptr ) {
            primary_game_r->update( *this, std::chrono::duration_cast<std::chrono::microseconds>(delta) );
        }

        // If position of the Camera changes then apply the changes.
        updateCamera();

        // Render the frame.
        environment_p->setupFrame();
        environment_p->advanceTime( delta_f );
        environment_p->drawFrame();

        text_2d_buffer_r->reset();

        last_time = this_time;

        if( !this->switch_to_resource_identifier.empty() || this->switch_to_platform != this->platform ) {
            switchToResource( this->switch_to_resource_identifier, this->switch_to_platform );
            this->switch_to_resource_identifier = "";
            this->platform = this->switch_to_platform;
        }

        if( delta < FRAME_MS_LIMIT )
            std::this_thread::sleep_for( FRAME_MS_LIMIT - delta );
    }
}

MainProgram::~MainProgram() {
    cleanup();
}

bool MainProgram::switchToResource( std::string switch_resource_identifier, Data::Manager::Platform switch_platform ) {
    if( this->resource_identifier == switch_resource_identifier && switch_platform == this->platform )
        return true;

    // Check if the parameter resource_identifier exists if not return false.
    if( !this->manager.hasEntry( switch_resource_identifier ) ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "The mission " << switch_resource_identifier << " does not exist cannot switch.";
        return false;
    }

    if( switch_platform != this->platform ) {
        this->manager.togglePlatform( switch_platform, true );
    }

    // Next load the given resource.
    auto switch_entry = this->manager.getIFFEntry( switch_resource_identifier );
    switch_entry.importance = Data::Manager::Importance::NEEDED;

    if( !this->manager.setIFFEntry( switch_resource_identifier, switch_entry ) ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "Set IFF Entry has failed for \"" << switch_resource_identifier << "\" cannot switch.";
        return false;
    }

    this->manager.setLoad( this->importance_level );

    // Check if the given resource successfully loaded if not return false.
    auto switch_resource_r = manager.getIFFEntry( switch_resource_identifier ).getIFF( switch_platform );
    if( switch_resource_r == nullptr ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "The mission IFF " << switch_resource_identifier << " did not load cannot switch.";
        return false;
    }

    // Set up environment to switch to the new resource.
    this->first_person_r->removeText2DBuffer( this->text_2d_buffer_r );

    // Unload the old resource.
    if( switch_resource_identifier != this->resource_identifier ) {
        auto old_entry = this->manager.getIFFEntry( this->resource_identifier );
        old_entry.importance = Data::Manager::Importance::NOT_NEEDED;

        if( !this->manager.setIFFEntry( this->resource_identifier, old_entry ) ) {
            auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
            log.output << "Set IFF Entry has failed for current \"" << this->resource_identifier << "\" in the switching process.";
        }
    }

    if( switch_platform != this->platform ) {
        this->manager.togglePlatform( this->platform, false );
    }

    this->manager.setLoad( this->importance_level );

    if( switch_platform != this->platform ) {
        auto switch_global_resource_r = manager.getIFFEntry( Data::Manager::global ).getIFF( switch_platform );

        if( switch_global_resource_r == nullptr ) {
            auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
            log.output << "The mission IFF does not have a global file.";
        }

        this->global_r = switch_global_resource_r;

    }

    this->resource_r = switch_resource_r;
    this->resource_identifier = switch_resource_identifier;

    // Update the accessor
    accessor.clear();
    accessor.load( *this->global_r );
    accessor.load( *this->resource_r );

    if( this->primary_game_r != nullptr ) {
        this->primary_game_r->unload( *this );
        this->primary_game_r->load( *this );
    }

    // Finally, return true for successful switching.
    return true;
}

glm::u32vec2 MainProgram::getWindowScale() const {
    glm::u32vec2 scale( 0, 0 );

    if( this->environment_p != nullptr && this->environment_p->window_p != nullptr)
        scale = this->environment_p->window_p->getDimensions();

    return scale;
}

void MainProgram::switchMenu( GameState* menu_r ) {
    if( this->menu_r != nullptr )
        this->menu_r->unload( *this );

    this->menu_r = menu_r;

    if( this->menu_r != nullptr )
        this->menu_r->load( *this );
}

void MainProgram::switchPrimaryGame( GameState* primary_game_r ) {
    if( this->primary_game_r != nullptr )
        this->primary_game_r->unload( *this );

    this->primary_game_r = primary_game_r;

    if( this->primary_game_r != nullptr )
        this->primary_game_r->load( *this );
}

void MainProgram::throwException( std::string output ) {
    {
        auto log = Utilities::logger.getLog( Utilities::Logger::CRITICAL );
        log.output << output << "\n";
    }

    cleanup();

    throw std::runtime_error( output );
}

void MainProgram::setupLogging() {
    // Setup the professional logger next.
    Utilities::logger.setOutputLog( &std::cout, 0, Utilities::Logger::INFO );

    {
        auto initialize_log = Utilities::logger.getLog( Utilities::Logger::ALL );
        initialize_log.output << parameters.getBinaryName() << " started at " << Utilities::Logger::getTime();
    }

    Utilities::logger.setTimeStampMode( true );

    // Export some info.
    {
        auto info_log = Utilities::logger.getLog( Utilities::Logger::INFO );
        info_log.output << "Using config file: " << paths.getConfigDirPath() << Utilities::Options::Paths::CONFIG_FILE_NAME << "\n";
    }
    {
        auto debug_log = Utilities::logger.getLog( Utilities::Logger::DEBUG );
        debug_log.output << "\nData directories\n"
                         << "  Windows directory:     " << options.getWindowsDataDirectory()     << "\n"
                         << "  Macintosh directory:   " << options.getMacintoshDataDirectory()   << "\n"
                         << "  Playstation directory: " << options.getPlaystationDataDirectory() << "\n"
                         << "\nUser directories\n"
                         << "  Savedgames directory:  " << options.getSaveDirectory()            << "\n"
                         << "  Screenshots directory: " << options.getScreenshotsDirectory()     << "\n";
    }
}

void MainProgram::initGraphics() {
    auto graphics_identifiers = Graphics::Environment::getAvailableIdentifiers();

    if( graphics_identifiers.empty() )
        throwException( "Graphics has no available identifiers. Therefore there is nothing for the game to render to." );

    if( !Graphics::Environment::isIdentifier( graphics_identifiers[0] ) )
        throwException( "The graphics identifier \"" + graphics_identifiers[0] + "\" is not an identifer." );

    this->graphics_identifier = graphics_identifiers[0];

    Graphics::Environment::initSystem( this->graphics_identifier );
}

void MainProgram::setupGraphics() {
    this->environment_p = Graphics::Environment::alloc( this->graphics_identifier );

    if( this->environment_p == nullptr )
        throwException( "Sorry, but OpenGL 2/OpenGL ES 2 are the minimum requirements for this engine. Identifier: " + this->graphics_identifier );

    this->environment_p->setDynamicTriangleFinalizeBitfield(options.getTemporaryBitfield());

    // Declare a pointer
    Graphics::Window *window_r = Graphics::Window::alloc( *this->environment_p );

    if( window_r == nullptr )
        throwException( "The graphics window has failed to allocate." );

    std::string title = "Future Cop M.I.T.";

    window_r->setWindowTitle( title );
    window_r->setDimensions( glm::u32vec2( options.getVideoWidth(), options.getVideoHeight() ) );

    if( window_r->attach() != 1 )
        throwException( "The graphics window has failed to attach." );

    // Initialize the camera
    this->first_person_r = Graphics::Camera::alloc( *this->environment_p );
    this->environment_p->window_p->attachCamera( *this->first_person_r );

    // Center the camera.
    if( this->environment_p->window_p->center() != 1 ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "The window had failed to center.";
    }

    window_r->setFullScreen( options.getVideoFullscreen() );
}

void MainProgram::initSound() {
    auto sound_identifiers = Sounds::Environment::getAvailableIdentifiers();

    if( sound_identifiers.empty() )
        throwException( "Sound has no available identifiers." );

    if( !Sounds::Environment::isIdentifier( sound_identifiers[0] ) )
        throwException( "The sound identifier \"" + sound_identifiers[0] + "\" is not a valid identifer." );

    this->sound_identifier = sound_identifiers[0];

    Sounds::Environment::initSystem( this->sound_identifier );
}

void MainProgram::setupSound() {
    this->sound_system_p = Sounds::Environment::alloc( this->sound_identifier );

    if( this->sound_system_p == nullptr )
        throwException( "Sound system does not work. Identifier: " + this->sound_identifier );
}

void MainProgram::loadResources() {
    accessor.clear();

    manager.autoSetEntries( options.getWindowsDataDirectory(),     Data::Manager::Platform::WINDOWS );
    manager.autoSetEntries( options.getMacintoshDataDirectory(),   Data::Manager::Platform::MACINTOSH );
    manager.autoSetEntries( options.getPlaystationDataDirectory(), Data::Manager::Platform::PLAYSTATION );

    if( this->parameters.global_path.wasModified() ) {
        Data::Manager::IFFEntry entry = manager.getIFFEntry( Data::Manager::global );
        // Just in case if this was not set on global id.
        entry.importance = Data::Manager::Importance::NEEDED;
        // Overide the global path.
        entry.setPath( Data::Manager::Platform::ALL, this->parameters.global_path.getValue() );
        manager.setIFFEntry( Data::Manager::global, entry );
    }

    if( this->parameters.mission_path.wasModified() ) {
        Data::Manager::IFFEntry entry = manager.getIFFEntry( CUSTOM_IDENTIFIER );
        // Overide the global path.
        entry.setPath( Data::Manager::Platform::ALL, this->parameters.mission_path.getValue() );
        manager.setIFFEntry( CUSTOM_IDENTIFIER, entry );

        resource_identifier = CUSTOM_IDENTIFIER;
    }

    auto entry = manager.getIFFEntry( resource_identifier );
    entry.importance = Data::Manager::Importance::NEEDED;

    if( !manager.setIFFEntry( this->resource_identifier, entry ) )
        throwException( "Set IFF Entry has failed for \"" + this->resource_identifier + "\"." );

    manager.togglePlatform( this->platform, true );

    manager.setLoad( this->importance_level );

    this->global_r = manager.getIFFEntry( Data::Manager::global ).getIFF( this->platform );
    if( this->global_r == nullptr ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "The global IFF " << Data::Manager::global << " did not load.";
    }
    else
        accessor.load( *this->global_r );

    this->resource_r = manager.getIFFEntry( this->resource_identifier ).getIFF( this->platform );
    if( this->resource_r == nullptr ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "The mission IFF " << this->resource_identifier << " did not load.";
    }
    else
        accessor.load( *this->resource_r );
}

void MainProgram::loadGraphics( bool show_map ) {
    this->environment_p->loadResources( this->accessor );
    this->environment_p->displayMap( show_map );
    this->environment_p->setBoundingBoxDraw( false );

    // Get the font from the resource file.
    if( Graphics::Text2DBuffer::loadFonts( *this->environment_p, this->accessor ) == 0 ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "Fonts are missing.";
    }

    this->text_2d_buffer_r = Graphics::Text2DBuffer::alloc( *this->environment_p );

    if( this->text_2d_buffer_r == nullptr )
        throwException( "The Graphics::Text2DBuffer has failed to allocate." );

    this->first_person_r->attachText2DBuffer( *this->text_2d_buffer_r );
}

void MainProgram::loadSound() {
    auto result = this->sound_system_p->loadResources( this->accessor );

    auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );

    log.output << "Sound did not load. Error code: " << result;
}

void MainProgram::setupCamera() {
    this->first_person_r->setViewportOrigin( glm::u32vec2( 0, 0 ) );
    this->first_person_r->setViewportDimensions( glm::u32vec2( options.getVideoWidth(), options.getVideoHeight() ) );

    glm::mat4 projection_matrix = glm::ortho( 0.0f, static_cast<float>( options.getVideoWidth() ), -static_cast<float>( options.getVideoHeight() ), 0.0f, -1.0f, 1.0f );

    this->first_person_r->setProjection2D( projection_matrix );

    projection_matrix = glm::perspective( glm::pi<float>() / 4.0f, static_cast<float>( options.getVideoWidth() ) / static_cast<float>( options.getVideoHeight() ), 0.1f, 200.0f );

    this->first_person_r->setProjection3D( projection_matrix );

    this->camera_position = { 0, 0, 0 };
    this->camera_rotation = glm::vec2( glm::pi<float>() / 4.0f, glm::pi<float>() / 4.0f );
    this->camera_distance = -20;

    updateCamera();
}

void MainProgram::centerCamera() {
    if( this->resource_r != nullptr ) {
        Data::Mission::PTCResource *map_r = Data::Mission::PTCResource::getVector( *this->resource_r ).at( 0 );

        if( map_r != nullptr ) {
            this->camera_position.x = static_cast<float>( map_r->getWidth()  - 1 ) / 2.0f * Data::Mission::TilResource::AMOUNT_OF_TILES;
            this->camera_position.y = 0;
            this->camera_position.z = static_cast<float>( map_r->getHeight() - 1 ) / 2.0f * Data::Mission::TilResource::AMOUNT_OF_TILES;
        }
        else
            this->camera_position = { 0, 0, 0 };
    }
    else
        this->camera_position = { 0, 0, 0 };
}

void MainProgram::updateCamera() {
    glm::mat4 extra_matrix_0;
    glm::mat4 extra_matrix_1;
    glm::mat4 extra_matrix_2;

    extra_matrix_0 = glm::rotate( glm::mat4(1.0f), -this->camera_rotation.x, glm::vec3( 0.0, 1.0, 0.0 ) );

    extra_matrix_0 = glm::translate( glm::mat4(1.0f), glm::vec3( 0, 0, this->camera_distance ) );
    extra_matrix_1 = glm::rotate( glm::mat4(1.0f), this->camera_rotation.y, glm::vec3( 1.0, 0.0, 0.0 ) ); // rotate up and down.
    extra_matrix_2 = extra_matrix_0 * extra_matrix_1;
    extra_matrix_1 = glm::rotate( glm::mat4(1.0f), this->camera_rotation.x, glm::vec3( 0.0, 1.0, 0.0 ) ); // rotate left and right.
    extra_matrix_0 = extra_matrix_2 * extra_matrix_1;
    extra_matrix_1 = glm::translate( glm::mat4(1.0f), -this->camera_position );
    extra_matrix_2 = extra_matrix_0 * extra_matrix_1;

    this->first_person_r->setView3D( extra_matrix_2 );
}

void MainProgram::setupControls() {
    // Setup the controls
    this->control_system_p = Controls::System::getSingleton(); // create the new system for controls
    this->controllers_r.push_back( new Controls::StandardInputSet( "Player 1" ) ); // It is not player_1_controller_r job to delete itself.
    control_system_p->addInputSet( this->controllers_r.back() );

    control_system_p->allocateCursor();
    this->control_cursor_r = control_system_p->getCursor();
}

void MainProgram::cleanup() {
    if( this->control_system_p != nullptr )
        delete this->control_system_p;

    if( this->environment_p != nullptr )
        delete this->environment_p;

    // Set everything to null.
    this->global_r         = nullptr;
    this->resource_r       = nullptr;
    this->environment_p    = nullptr;
    this->text_2d_buffer_r = nullptr;
    this->first_person_r   = nullptr;
    this->control_system_p = nullptr;
    this->menu_r           = nullptr;
    this->primary_game_r   = nullptr;
}
