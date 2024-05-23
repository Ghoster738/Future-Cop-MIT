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
    loadResources();
    loadGraphics();
    setupCamera();
    setupControls();
}

void MainProgram::displayLoop() {
    auto last_time = std::chrono::high_resolution_clock::now();
    auto this_time = last_time;
    auto delta = this_time - last_time;

     const float times[] = {0.000000, 0.345506, 0.062126, 0.004702, 0.005930, 0.004484, 0.005249, 0.004432, 0.007193, 0.004118, 0.003835, 0.006508, 0.003940, 0.004251, 0.006268, 0.003840, 0.004147, 0.004201, 0.005783, 0.003782, 0.004153, 0.004188, 0.004299, 0.003933, 0.004151, 0.004004, 0.004273, 0.003799, 0.004086, 0.003974, 0.004256, 0.004047, 0.004218, 0.003904, 0.004223, 0.004229, 0.004164, 0.004263, 0.005651, 0.003913, 0.004269, 0.004049, 0.004891, 0.003537, 0.004150, 0.004015, 0.004100, 0.003808, 0.005492, 0.006580, 0.004264, 0.004162, 0.004374, 0.004354, 0.003943, 0.003924, 0.004034, 0.004388, 0.003717, 0.004082, 0.004111, 0.004084, 0.003844, 0.004121, 0.004227, 0.005018, 0.003635, 0.003960, 0.003964, 0.004730, 0.003595, 0.004368, 0.003674, 0.005214, 0.003742, 0.003714, 0.004133, 0.005093, 0.003533, 0.003984, 0.003987, 0.004434, 0.003794, 0.004195, 0.003921, 0.005243, 0.003632, 0.003898, 0.003973, 0.005558, 0.003603, 0.004092, 0.003976, 0.005194, 0.003630, 0.004008, 0.003961, 0.004089, 0.003992, 0.004021, 0.004037, 0.004847, 0.003535, 0.004013, 0.004001, 0.005765, 0.003650, 0.003941, 0.003950, 0.005096, 0.003683, 0.004007, 0.004021, 0.005364, 0.003643, 0.003994, 0.003896, 0.004857, 0.003693, 0.003914, 0.003875, 0.005648, 0.003666, 0.003837, 0.004037, 0.004612, 0.003627, 0.003962, 0.003991, 0.005700, 0.003676, 0.004074, 0.003986, 0.004021, 0.003748, 0.003928, 0.004063, 0.005314, 0.003614, 0.003936, 0.004018, 0.006380, 0.003704, 0.003794, 0.004138, 0.004287, 0.003761, 0.004213, 0.004009, 0.005519, 0.003765, 0.004160, 0.004196, 0.004203, 0.003616, 0.004030, 0.004110, 0.005528, 0.003696, 0.004152, 0.004028, 0.004794, 0.003819, 0.003928, 0.003966, 0.004905, 0.004099, 0.003949, 0.004040, 0.005114, 0.003827, 0.003977, 0.007473, 0.003889, 0.004091, 0.004037, 0.004044, 0.004069, 0.004004, 0.004034, 0.006176, 0.003591, 0.004166, 0.004102, 0.004124, 0.003920, 0.004152, 0.004214, 0.004055, 0.003905, 0.004025, 0.004044, 0.005469, 0.003580, 0.004014, 0.003948, 0.004744, 0.003533, 0.003886, 0.003948, 0.005438, 0.003579, 0.003945, 0.003863, 0.004786, 0.003593, 0.003978, 0.004082, 0.004680, 0.003623, 0.004206, 0.003626, 0.005373, 0.003579, 0.003994, 0.003946, 0.004933, 0.003602, 0.004300, 0.003656, 0.004512, 0.003572, 0.003964, 0.003836, 0.005961, 0.003655, 0.004011, 0.003943, 0.004525, 0.003627, 0.003956, 0.003829, 0.005616, 0.003598, 0.004333, 0.003573, 0.004075, 0.003718, 0.004018, 0.003891, 0.005322, 0.003541, 0.003862, 0.003900, 0.005347, 0.003586, 0.003974, 0.003873, 0.004605, 0.003613, 0.004279, 0.003825, 0.004971, 0.003633, 0.003954, 0.003881, 0.005730, 0.003547, 0.004123, 0.003868, 0.004505, 0.003559, 0.003883, 0.003791, 0.005834, 0.003586, 0.004029, 0.003909, 0.005399, 0.003559, 0.003945, 0.003918, 0.005082, 0.003538, 0.004012, 0.003963, 0.005054, 0.003546, 0.003986, 0.003924, 0.005359, 0.003562, 0.003888, 0.003979, 0.005142, 0.003523, 0.004068, 0.003928, 0.013639, 0.014159, 0.004044, 0.004203, 0.008263, 0.003978, 0.004040, 0.004268, 0.004033, 0.004009, 0.004265, 0.004230, 0.004070, 0.004122, 0.004260, 0.004074, 0.003843, 0.004158, 0.004068, 0.004074, 0.004253, 0.003967, 0.004050, 0.004243, 0.003930, 0.004006, 0.004096, 0.003982, 0.004089, 0.003859, 0.004299, 0.003598, 0.004105, 0.003787, 0.004100, 0.003910, 0.003937, 0.003968, 0.004011, 0.004038, 0.004044, 0.004338, 0.004009, 0.003923, 0.003945, 0.003962, 0.004057, 0.003952, 0.004062, 0.003930, 0.004051, 0.003950, 0.004021, 0.003891, 0.003980, 0.004003, 0.004266, 0.003565, 0.004075, 0.003928, 0.004333, 0.003663, 0.003986, 0.003960, 0.003930, 0.004033, 0.003728, 0.003996, 0.003967, 0.003823, 0.003889, 0.004001, 0.003932, 0.003964, 0.003868, 0.004025, 0.003913, 0.004057, 0.003965, 0.004032, 0.003923, 0.004039, 0.003887, 0.004015, 0.004028, 0.004047, 0.004042, 0.003971, 0.004026, 0.004050, 0.004061, 0.004049, 0.004075, 0.003946, 0.004056, 0.003971, 0.004086, 0.004000, 0.004123, 0.003972, 0.004068, 0.003937, 0.003822, 0.004207, 0.004026, 0.003965, 0.004491, 0.003670, 0.004108, 0.003820, 0.004178, 0.003754, 0.003920, 0.003838, 0.003901, 0.003955, 0.004079, 0.003866, 0.004136, 0.003887, 0.003978, 0.003975, 0.003897, 0.003976, 0.003945, 0.003855, 0.004138, 0.004199, 0.003666, 0.003984, 0.003961, 0.004001, 0.003980, 0.003950, 0.003943, 0.004062, 0.003933, 0.004028, 0.003961, 0.004044, 0.003967, 0.004008, 0.004033, 0.003952, 0.004085, 0.003891, 0.003991, 0.003979, 0.004075, 0.003984, 0.004065, 0.003982, 0.004360, 0.003709, 0.004066, 0.003999, 0.003952, 0.003863, 0.004113, 0.004073, 0.004135, 0.004019, 0.004010, 0.004050, 0.003998, 0.003993, 0.004023, 0.003981, 0.004029, 0.003952, 0.004006, 0.003977, 0.004113, 0.003985, 0.003882, 0.004061, 0.004138, 0.004031, 0.004022, 0.004044, 0.003979, 0.004273, 0.003569, 0.004121, 0.004011, 0.003980, 0.004075, 0.004062, 0.004106, 0.004039, 0.003924, 0.004120, 0.004021, 0.003814, 0.004112, 0.003839, 0.004075, 0.003852, 0.004102, 0.003785, 0.003941, 0.004037, 0.003934, 0.003972, 0.003991, 0.003926, 0.003981, 0.003900, 0.003931, 0.003919, 0.003972, 0.003898, 0.004025, 0.003921, 0.003988, 0.003950, 0.003985, 0.003977, 0.003942, 0.003891, 0.003842, 0.003996, 0.004013, 0.004641, 0.003618, 0.004006, 0.003934, 0.004005, 0.003895, 0.003827, 0.004054, 0.003970, 0.003932, 0.003945, 0.003923, 0.003970, 0.003849, 0.003872, 0.003945, 0.004012, 0.003985, 0.003968, 0.003937, 0.003969, 0.003929, 0.003952, 0.003965, 0.003983, 0.003987, 0.003884, 0.004076, 0.003965, 0.003996, 0.004067, 0.003973, 0.004067, 0.003925, 0.004055, 0.004007, 0.004006, 0.004000, 0.004056, 0.004759, 0.004045, 0.003996, 0.004026, 0.003934, 0.004036, 0.003996, 0.004139, 0.003942, 0.004015, 0.004029, 0.004300, 0.003607, 0.004072, 0.004007, 0.003987, 0.003956, 0.004038, 0.003965, 0.003987, 0.004029, 0.003869, 0.004031, 0.004007, 0.003958, 0.004034, 0.003962, 0.003990, 0.003969, 0.004016, 0.003961, 0.003960, 0.003959, 0.003967, 0.003977, 0.003998, 0.003977, 0.003942, 0.003984, 0.003984, 0.003964, 0.004027, 0.003925, 0.004076, 0.004032, 0.004055, 0.004016, 0.004089, 0.003979, 0.003989, 0.003961, 0.004094, 0.004019, 0.003974, 0.004009, 0.004065, 0.004003, 0.004019, 0.004039, 0.004084, 0.004008, 0.004334, 0.003632, 0.004010, 0.004009, 0.004032, 0.003943, 0.003974, 0.003959, 0.004010, 0.004002, 0.004030, 0.004073, 0.004090, 0.003995, 0.004038, 0.004017, 0.004075, 0.004043, 0.003982, 0.003900, 0.004052, 0.004044, 0.003958, 0.003991, 0.004047, 0.003958, 0.003964, 0.004019, 0.003967, 0.004026, 0.003895, 0.003818, 0.003985, 0.003982, 0.004053, 0.004001, 0.003994, 0.003939, 0.004016, 0.003895, 0.003984, 0.004000, 0.004003, 0.004005, 0.004052, 0.003956, 0.004013, 0.004008, 0.003963, 0.004010, 0.004047, 0.004019, 0.003992, 0.003990, 0.004054, 0.003948, 0.004067, 0.003924, 0.004008, 0.003988, 0.003999, 0.003991, 0.004065, 0.003991, 0.004071, 0.004089, 0.003956, 0.004041, 0.003999, 0.003881, 0.003932, 0.005367, 0.003674, 0.003919, 0.003833, 0.004035, 0.003739, 0.003934, 0.003892, 0.003971, 0.003882, 0.003945, 0.004016, 0.003983, 0.004014, 0.003977, 0.004018, 0.004036, 0.004074, 0.004006, 0.003982, 0.003989, 0.004026, 0.003860, 0.004006, 0.003944, 0.004030, 0.003998, 0.004025, 0.003954, 0.003974, 0.003971, 0.003968, 0.003938, 0.003940, 0.003948, 0.003987, 0.003963, 0.003983, 0.003962, 0.003978, 0.003957, 0.004074, 0.003963, 0.004006, 0.003970, 0.004026, 0.003958, 0.004011, 0.004049, 0.004032, 0.004067, 0.003864, 0.004002, 0.003984, 0.003801, 0.003979, 0.003964, 0.004042, 0.004027, 0.003933, 0.003994, 0.004004, 0.004064, 0.004008, 0.003956, 0.004001, 0.003939, 0.003977, 0.003978, 0.004030, 0.004015, 0.004003, 0.004003, 0.004099, 0.003965, 0.004160, 0.003903, 0.004034, 0.003975, 0.004032, 0.003984, 0.004083, 0.004026, 0.004132, 0.003984, 0.004014, 0.004138, 0.004028, 0.003829, 0.004010, 0.003984, 0.003933, 0.003955, 0.004067, 0.003889, 0.004031, 0.004047, 0.004004, 0.004026, 0.004006, 0.004060, 0.003923, 0.003950, 0.004043, 0.003993, 0.003978, 0.003953, 0.004181, 0.004091, 0.003993, 0.004076, 0.004039, 0.004010, 0.003990, 0.003957, 0.004074, 0.004046, 0.004024, 0.004005, 0.004033, 0.004048, 0.004065, 0.003990, 0.004005, 0.004094, 0.004165, 0.003969, 0.004036, 0.004057, 0.004032, 0.004029, 0.003980, 0.004004, 0.004008, 0.004005, 0.004022, 0.004037, 0.003960, 0.004016, 0.004002, 0.003833, 0.003957, 0.003959, 0.003995, 0.003992, 0.004024, 0.003974, 0.003986, 0.004002, 0.004060, 0.003972, 0.003972, 0.004020, 0.003999, 0.004017, 0.004043, 0.003966, 0.003993, 0.003951, 0.003980, 0.004024, 0.003992, 0.004017, 0.004176, 0.004027, 0.004008, 0.004037, 0.004035, 0.004003, 0.004018, 0.004001, 0.003973, 0.003999, 0.004051, 0.004032, 0.003943, 0.003966, 0.004074, 0.003937, 0.003958, 0.003980, 0.004014, 0.004011, 0.004056, 0.003983, 0.003960, 0.004013, 0.004034, 0.004025, 0.003957, 0.004016, 0.003941, 0.004014, 0.003955, 0.004010, 0.003979, 0.004003};

    unsigned time_count = 0;

    while( this->play_loop ) {
        this_time = std::chrono::high_resolution_clock::now();
        delta = this_time - last_time;
        float delta_f = times[time_count];// std::chrono::duration<float, std::ratio<1>>( delta ).count();

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

        time_count++;

        if(time_count >= sizeof(times) / sizeof(times[0]))
            play_loop = false;
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
