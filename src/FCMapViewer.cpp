// This source code is a refarctored version of https://gist.github.com/SuperV1234/5c5ad838fe5fe1bf54f9 or SuperV1234
#include <iostream>
#include <ratio>
#include <chrono>
#include <thread>

#include "Data/Manager.h"

#include "Data/Mission/IFF.h"
#include "Data/Mission/ObjResource.h"
#include "Data/Mission/BMPResource.h"

#include "Data/Mission/Til/Mesh.h"
#include "Data/Mission/Til/Colorizer.h"

#include "Utilities/Math.h"
#include "Graphics/Environment.h"
#include "Controls/System.h"
#include "Controls/StandardInputSet.h"

bool test_MissionTilResource();

int main(int argc, char** argv)
{
    // Reference the tile set data.
    // Data::Mission::Til::Mesh::loadMeshScript( "./tile_set.json", &std::cout );
    
    const int WIDTH = 1024;
    const int HEIGHT = 764;

	// Declare a pointer
    Graphics::Window *window = new Graphics::Window();
    std::string title = "Future Cop Map Viewer";

    Graphics::Environment::initSystem();

    window->setWindowTitle( title );
    if( window->center() != 1 )
        std::cout << "The window had failed to center! " << window->center() << std::endl;
    window->setDimensions( Utilities::DataTypes::Vec2UInt( WIDTH, HEIGHT ) );
    window->setFullScreen( true );

    std::string iff_mission_id = "pa_urban_jungle";
    std::string global_id = "global";
    Data::Manager::Platform platform = Data::Manager::Platform::WINDOWS;
    auto load_all = Data::Manager::Importance::NEEDED;
    std::string global_path = "";
    std::string mission_path = "";
    std::string variable_name = "";
    bool platform_all = false;

    for( int index = 1; index < argc; index++ ) {
        std::string input = std::string( argv[index] );

        if( variable_name.compare("") == 0 ) {
            if( input.find( "--load-all") == 0 )
                load_all = Data::Manager::Importance::NOT_NEEDED;
            else
            if( input.find( "--platform-all") == 0 )
                platform_all = true;
            else
            if( input.find( "-w") == 0 )
                platform = Data::Manager::Platform::WINDOWS;
            else
            if( input.find( "-m") == 0 )
                platform = Data::Manager::Platform::MACINTOSH;
            else
            if( input.find( "-p") == 0 )
                platform = Data::Manager::Platform::PLAYSTATION;
            else
            if( input.find( "--id") == 0 )
                variable_name = "--id";
            else
            if( input.find( "--global") == 0 )
                variable_name = "--global";
            else
            if( input.find( "--path") == 0 )
                variable_name = "--path";
        }
        else {
            if( variable_name.find( "--id") == 0 )
                iff_mission_id = input;
            else
            if( variable_name.find( "--global") == 0 )
                global_path = input;
            else
            if( variable_name.find( "--path") == 0 )
                mission_path = input;
            
            variable_name = "";
        }
    }

    Graphics::Environment *environment = new Graphics::Environment();
    if( environment->attachWindow( *window ) != 1 )
        return -40;

    Data::Manager manager;

    manager.autoSetEntries( "Data/Platform/" );
    
    // If the global path is specified then use a specified path.
    if( global_path.compare("") != 0 ) {
        Data::Manager::IFFEntry entry = manager.getIFFEntry( global_id );
        // Just in case if this was not set on global id.
        entry.importance = Data::Manager::Importance::NEEDED;
        // Overide the global path.
        entry.setPath( platform, global_path );
        manager.setIFFEntry( global_id, entry );
    }
    
    // If the mission path is specified then use a specified path.
    if( mission_path.compare("") != 0  ) {
        iff_mission_id = "unk_custom_mission";
        
        Data::Manager::IFFEntry entry = manager.getIFFEntry( iff_mission_id );
        // Overide the global path.
        entry.setPath( platform, mission_path );
        manager.setIFFEntry( iff_mission_id, entry );
    }

    if( !manager.hasEntry( iff_mission_id ) )
        return -1;

    auto entry = manager.getIFFEntry( iff_mission_id );
    entry.importance = Data::Manager::Importance::NEEDED;

    if( !manager.setIFFEntry( iff_mission_id, entry ) )
        return -2;

    manager.togglePlatform( platform, true );
    
    if( platform_all )
        manager.togglePlatform( Data::Manager::Platform::ALL, true );

    auto number_of_iffs = manager.setLoad( load_all );

    if( number_of_iffs < 2 ) {
        std::cout << "The number IFF " << number_of_iffs << " is not enough." << std::endl;
        return -3;
    }

    Data::Mission::IFF *resource_r = manager.getIFFEntry( iff_mission_id ).getIFF( platform );
    Data::Mission::IFF   *global_r = manager.getIFFEntry( global_id ).getIFF( platform );

    if( resource_r == nullptr ) {
        std::cout << "The mission IFF " << iff_mission_id << " did not load." << std::endl;
        return -4;
    }

    if( global_r == nullptr ) {
        std::cout << "The global IFF did not load." << std::endl;
        return -5;
    }

    // First get the model textures from the resource file.
    {
        auto cbmp_resources = Data::Mission::BMPResource::getVector( *resource_r );

        int status = environment->setModelTextures( cbmp_resources );

        if( status < 0 )
            std::cout << (-status) << " general textures had failed to load out of " << cbmp_resources.size() << std::endl;
    }

    // Load all the 3D meshes from the resource as well.
    {
        auto cobj_resources = Data::Mission::ObjResource::getVector( *resource_r );

        int status = environment->setModelTypes( cobj_resources );

        if( status < 0 )
            std::cout << (-status) << " 3d meshes had failed to load out of " << cobj_resources.size() << std::endl;
    }
    
    // Get the font from the resource file.
    {
        auto font_resources = Data::Mission::FontResource::getVector( *resource_r );

        if( font_resources.size() != 0 )
            environment->setFonts( font_resources );
        else
        {
            font_resources = Data::Mission::FontResource::getVector( *global_r );

            for( auto i : font_resources ) {
                std::cout << "Pointer " << i << std::endl;
            }

            environment->setFonts( font_resources );
            if( font_resources.size() == 0 )
                std::cout << " general fonts had failed to load out of " << font_resources.size() << std::endl;
        }
    }

    {
        auto til_resources = Data::Mission::TilResource::getVector( *resource_r );

        environment->setMap( *Data::Mission::PTCResource::getVector( *resource_r ).at( 0 ), til_resources );
    }

	bool viewer_loop = true;

    Graphics::Text2DBuffer *text_2d_buffer = new Graphics::Text2DBuffer( 128 ); // 128 Kibibytes.
    text_2d_buffer->loadFontLibrary( *environment );

    // Setup the camera
    Graphics::Camera *first_person = new Graphics::Camera();
    first_person->attachText2DBuffer( *text_2d_buffer );
    window->attachCamera( *first_person );
    first_person->setViewportOrigin( Utilities::DataTypes::Vec2UInt( 0, 0 ) );
    first_person->setViewportDimensions( Utilities::DataTypes::Vec2UInt( WIDTH, HEIGHT ) );
    Utilities::DataTypes::Mat4 extra_matrix_0;
    Utilities::DataTypes::Mat4 extra_matrix_1;
    Utilities::DataTypes::Mat4 extra_matrix_2;
    
    Utilities::Math::setOthro( extra_matrix_0, 0, WIDTH, -HEIGHT, 0, -1.0, 1.0 );
    
    first_person->setProjection2D( extra_matrix_0 );

    Utilities::Math::setPerspective( extra_matrix_0, Utilities::Math::toRadians(45.0), static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.1f, 200.0f );

    first_person->setProjection3D( extra_matrix_0 );

    // Setup the timer
    auto last_time = std::chrono::high_resolution_clock::now();
    auto this_time = last_time;
    auto delta = this_time - last_time;

	//GLint GlowCurrentLoc = glGetUniformLocation( default_graphics_program.getProgram(), "CurrentGlow" );
	float glow_amount = 0.0f;
	//GLint WhichTileLoc = glGetUniformLocation( default_graphics_program.getProgram(), "WhichTile" );
	int current_tile_selected = -1;
	//glUniform1f( WhichTileLoc, current_tile_selected );
	bool entering_number = false;

    Utilities::DataTypes::Vec3 position_of_camera = Utilities::DataTypes::Vec3( 16, 0, 16 );
    Utilities::DataTypes::Vec4 direction_keyboard = Utilities::DataTypes::Vec4( 0, 0, 0, 0 );
    Utilities::DataTypes::Vec4 movement_of_camera = Utilities::DataTypes::Vec4( 0, 0, 0, 0 );
    Utilities::DataTypes::Vec2 rotation = Utilities::DataTypes::Vec2( 0, Utilities::Math::toRadians(45.0) );
    double distance_away = -10;
    bool isCameraMoving = false;

    if( window->center() != 1 )
        std::cout << "The window had failed to center! " << window->center() << std::endl;

    // Setup the controls
    auto control_system_p = Controls::System::getSingleton(); // create the new system for controls
    auto player_1_controller_r = new Controls::StandardInputSet( "Player 1" ); // It is not player_1_controller_r job to delete itself.
    control_system_p->addInputSet( player_1_controller_r );

    control_system_p->allocateCursor();
    auto control_cursor_r = control_system_p->getCursor();

    for( unsigned x = 0; x < control_system_p->amountOfInputSets(); x++ )
    {
        auto input_set_r = control_system_p->getInputSet( x );

        for( unsigned y = 0; input_set_r->getInput( y ) != nullptr; y++ )
        {
            int status = 0;

            text_2d_buffer->setFont( 0 );
            text_2d_buffer->setColor( Utilities::DataTypes::Vec4( 1, 1, 1, 1 ) );
            text_2d_buffer->setPosition( Utilities::DataTypes::Vec2( 0, 0 ) );
            text_2d_buffer->print( "Input Set: \"" + input_set_r->getName() +"\"" );

            text_2d_buffer->setFont( 0 );
            text_2d_buffer->setColor( Utilities::DataTypes::Vec4( 1, 0.25, 0.25, 1 ) );
            text_2d_buffer->setPosition( Utilities::DataTypes::Vec2( 0, 20 ) );
            text_2d_buffer->print( "Enter a key for Input, \"" + input_set_r->getInput( y )->getName() +"\"" );

            text_2d_buffer->setColor( Utilities::DataTypes::Vec4( 1, 1, 0, 1 ) );
            text_2d_buffer->setPosition( Utilities::DataTypes::Vec2( 0, 40 ) );
            if( control_cursor_r == nullptr )
                text_2d_buffer->print( "There is no cursor!" );
            else
                text_2d_buffer->print( "One cursor is being used!" );

            while( status < 1  && viewer_loop )
            {
                status = control_system_p->pollEventForInputSet( x, y );

                if( control_system_p->isOrderedToExit() )
                    viewer_loop = false;

                environment->drawFrame();
                environment->advanceTime( 0 );

                std::this_thread::sleep_for( std::chrono::microseconds(40) ); // delay for 40ms the frequency really does not mater for things like this. Run 25 times in one second.
            }

            text_2d_buffer->reset();
        }
    }

    test_MissionTilResource();

    while(viewer_loop)
    {
        // Get the time
        this_time = std::chrono::high_resolution_clock::now();
        delta = this_time - last_time;
        double delta_f = std::chrono::duration<float, std::ratio<1>>( delta ).count();

        // glUniform1f( GlowCurrentLoc, static_cast<float>(glow_amount < 1.0f) * glow_amount + static_cast<float>(glow_amount >= 1.0f) * (2.0f - glow_amount) );
        glow_amount += delta_f;
        glow_amount = static_cast<float>(glow_amount < 2.0f) * glow_amount;

        // Start the input routine

        // Make the control system poll all the inputs.
        control_system_p->advanceTime( delta_f );

        if( control_system_p->isOrderedToExit() )
            viewer_loop = false;

        if( player_1_controller_r->isChanged() )
        {
            auto input_r = player_1_controller_r->getInput( Controls::StandardInputSet::Buttons::UP );
            if( input_r->isChanged() )
            {
                if( input_r->getState() > 0.5 )
                    movement_of_camera.z = 16;
                else
                    movement_of_camera.z = 0;
            }

            input_r = player_1_controller_r->getInput( Controls::StandardInputSet::Buttons::DOWN );
            if( input_r->isChanged() )
            {
                if( input_r->getState() > 0.5 )
                    movement_of_camera.z = -16;
                else
                    movement_of_camera.z = 0;
            }

            input_r = player_1_controller_r->getInput( Controls::StandardInputSet::Buttons::LEFT );
            if( input_r->isChanged() )
            {
                if( input_r->getState() > 0.5 )
                    movement_of_camera.x = 16;
                else
                    movement_of_camera.x = 0;
            }

            input_r = player_1_controller_r->getInput( Controls::StandardInputSet::Buttons::RIGHT );
            if( input_r->isChanged() )
            {
                if( input_r->getState() > 0.5 )
                    movement_of_camera.x = -16;
                else
                    movement_of_camera.x = 0;
            }

            input_r = player_1_controller_r->getInput( Controls::StandardInputSet::Buttons::ROTATE_LEFT );
            if( input_r->isChanged() && input_r->getState() > 0.5 )
            {
                // Stop the blinking on the previous current_tile_selected
                environment->setTilBlink( current_tile_selected, -1.0 );

                // Set the next current_tile_selected to flash
                current_tile_selected = environment->setTilBlink( current_tile_selected - 1, 1.0 );
            }

            input_r = player_1_controller_r->getInput( Controls::StandardInputSet::Buttons::ROTATE_RIGHT );
            if( input_r->isChanged() && input_r->getState() > 0.5 )
            {
                // Stop the blinking on the previous current_tile_selected
                environment->setTilBlink( current_tile_selected, -1.0 );

                // Set the next current_tile_selected to flash
                current_tile_selected = environment->setTilBlink( current_tile_selected + 1, 1.0 );
            }

            input_r = player_1_controller_r->getInput( Controls::StandardInputSet::Buttons::CAMERA );
            if( input_r->isChanged() && input_r->getState() > 0.5 )
            {
                Utilities::ImageData image_screenshot( WIDTH, HEIGHT,  Utilities::ImageData::RED_GREEN_BLUE, 1 );
                environment->screenshot( image_screenshot );
                image_screenshot.write( "./dialog.png" );
            }
        }

        if( control_cursor_r != nullptr && control_cursor_r->isChanged() )
        {
            auto input_r = control_cursor_r->getInput( Controls::CursorInputSet::Inputs::MIDDLE_BUTTON );
            if( input_r->isChanged() )
            {
                if( input_r->getState() > 0.5 )
                    isCameraMoving = true;
                else
                    isCameraMoving = false;
            }

            if( isCameraMoving )
            {
                input_r = control_cursor_r->getInput( Controls::CursorInputSet::Inputs::MOTION_X );
                rotation.x += delta_f * static_cast<double>( input_r->getState() ) * (1.0 / 3.14);

                input_r = control_cursor_r->getInput( Controls::CursorInputSet::Inputs::MOTION_Y );
                rotation.y += delta_f * static_cast<double>( input_r->getState() ) * (1.0 / 3.14);
            }

            input_r = control_cursor_r->getInput( Controls::CursorInputSet::Inputs::WHEEL_Y );
            if( input_r->isChanged() )
            {
                 distance_away += 16.0 * delta_f * static_cast<double>( input_r->getState() );
            }
        }

        Utilities::Math::setRotation( extra_matrix_0, Utilities::DataTypes::Vec3( 0.0, 1.0, 0.0 ), -rotation.x );
        Utilities::DataTypes::Vec4 tmp;
        Utilities::Math::multiply( tmp, extra_matrix_0, Utilities::DataTypes::Vec4(movement_of_camera.x * delta_f, movement_of_camera.y * delta_f, movement_of_camera.z * delta_f, 1) );

        position_of_camera.x += tmp.x;
        position_of_camera.y += tmp.y;
        position_of_camera.z += tmp.z;

        Utilities::Math::setTranslation( extra_matrix_0, Utilities::DataTypes::Vec3( 0, 0, distance_away ) );
        Utilities::Math::setRotation( extra_matrix_1, Utilities::DataTypes::Vec3( 1.0, 0.0, 0.0), rotation.y ); // rotate up and down.
        Utilities::Math::multiply( extra_matrix_2, extra_matrix_0, extra_matrix_1 );
        Utilities::Math::setRotation( extra_matrix_1, Utilities::DataTypes::Vec3( 0.0, 1.0, 0.0), rotation.x ); // rotate left and right.
        Utilities::Math::multiply( extra_matrix_0, extra_matrix_2, extra_matrix_1 );
        Utilities::Math::setTranslation( extra_matrix_1, position_of_camera );
        Utilities::Math::multiply( extra_matrix_2, extra_matrix_0, extra_matrix_1 );

        first_person->setView3D( extra_matrix_2 );

        text_2d_buffer->setFont( 5 );
        text_2d_buffer->setColor( Utilities::DataTypes::Vec4( 1, 0, 0, 1 ) );
        text_2d_buffer->setPosition( Utilities::DataTypes::Vec2( 0, 0 ) );
        text_2d_buffer->print( "Position = (" + std::to_string(position_of_camera.x) + ", " + std::to_string(position_of_camera.y) + ", " + std::to_string(position_of_camera.z) + ")" );

        text_2d_buffer->setFont( 4 );
        text_2d_buffer->setColor( Utilities::DataTypes::Vec4( 0, 1, 0, 1 ) );
        text_2d_buffer->setPosition( Utilities::DataTypes::Vec2( 0, 20 ) );
        text_2d_buffer->print( "Rotation = (" + std::to_string(rotation.x) + ", " + std::to_string(rotation.y) + ")" );

        text_2d_buffer->setFont( 2 );
        text_2d_buffer->setColor( Utilities::DataTypes::Vec4( 0, 1, 1, 1 ) );
        text_2d_buffer->setPosition( Utilities::DataTypes::Vec2( 0, 40 ) );
        text_2d_buffer->print( "TilRes = " + std::to_string(current_tile_selected) );

        environment->drawFrame();
        environment->advanceTime( delta_f );

        text_2d_buffer->reset();

        if( delta < std::chrono::microseconds(17) )
            std::this_thread::sleep_for( std::chrono::microseconds(17) - delta );

        last_time = this_time;
    }

    delete control_system_p;
    delete environment;

    // Clean up
    Graphics::Environment::deinitEntireSystem();

    return 0;
}
