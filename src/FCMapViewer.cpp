// This source code is a refarctored version of https://gist.github.com/SuperV1234/5c5ad838fe5fe1bf54f9 or SuperV1234
#include <iostream>
#include <ratio>
#include <chrono>
#include <thread>

#include "Config.h"

#include "Data/Manager.h"

#include "Data/Mission/IFF.h"
#include "Data/Mission/ObjResource.h"
#include "Data/Mission/BMPResource.h"
#include "Data/Mission/PTCResource.h"
#include "Data/Mission/Til/Mesh.h"
#include "Data/Mission/Til/Colorizer.h"

#include "Graphics/Environment.h"

#include "Controls/System.h"
#include "Controls/StandardInputSet.h"

#include "Utilities/ImageFormat/Chooser.h"

#include "ConfigureInput.h"
#include "SplashScreens.h"

namespace {
void helpExit( std::ostream &stream ) {
    stream << "\n";
    stream << "Future Cop: MIT - Map viewer (version " << FUTURE_COP_MIT_VERSION << ")\n";
    stream << "\n";
    stream << "Usage:" << "\n";
    stream << "  FCMapViewer [-h|--help]" << "\n";
    stream << "              [--width <number>] [--height <number>]" << "\n";
    stream << "              [-w] [-m] [-p] [--id <id>] [--load-all] [--platform-all]" << "\n";
    stream << "              [--global <path>] [--path <path>]" << "\n";
    stream << "\n";
    stream << "Options:" << "\n";
    stream << "  General:" << "\n";
    stream << "    -h --help  Display this screen and exit" << "\n";
    stream << "  Interface:" << "\n";
    stream << "    --width <number>   Window/screen resolution width - defaults to 640 if not specified." << "\n";
    stream << "    --height <number>  Window/screen resolution height - defaults to 480 if not specified." << "\n";
    stream << "  Data loading:" << "\n";
    stream << "    -w              Load Windows game data from './Data/Platform/Windows'" << "\n";
    stream << "    -m              Load Macintosh game data from './Data/Platform/Macintosh'" << "\n";
    stream << "    -p              Load PlayStation game data from './Data/Platform/Playstation'" << "\n";
    stream << "    --id <id>       Load the specified mission ID. Type in an invalid id to get a listing of valid IDs." << "\n";
    stream << "    --load-all      Load every single map. Will take some time." << "\n";
    stream << "    --platform-all  Attempt to load from all three platforms for the given ID." << "\n";
    stream << "                    If --load-all is also present on the command line then the program will load all the levels." << "\n";
    stream << "  Maps:" << "\n";
    stream << "    --global <path>  Path to the global file which every map uses." << "\n";
    stream << "    --path <path>    Path to the mission file which contains the rest of the data like the map." << "\n";
    stream << "\n";
    exit( 0 );
}
}

int main(int argc, char** argv)
{
    // Reference the tile set data.
    // Data::Mission::Til::Mesh::loadMeshScript( "./tile_set.json", &std::cout );
    
    int width = 0;
    int height = 0;

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

        if( variable_name.empty() ) {
            if( input.find( "--help") == 0 || input.find( "-h") == 0 )
                helpExit( std::cout );
            else
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
            else
            if( input.find( "--width") == 0 )
                variable_name = "--width";
            else
            if( input.find( "--height") == 0 )
                variable_name = "--height";
            else
                helpExit( std::cout );
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
            else
            if( variable_name.find( "--width") == 0 )
                width = std::stoi(input);
            else
            if( variable_name.find( "--height") == 0 )
                height = std::stoi(input);
            
            variable_name = "";
        }
    }
    
    if( width <= 0 )
        width = 640;
    if( height <= 0 )
        height = 480;

    auto graphics_identifiers = Graphics::Environment::getAvailableIdentifiers();
    
    if( graphics_identifiers.size() == 0 )
        return -37;
    
    if( !Graphics::Environment::isIdentifier( graphics_identifiers[0] ) )
        return -38;

    Graphics::Environment::initSystem( graphics_identifiers[0] );
    
    Graphics::Environment *environment_p = Graphics::Environment::alloc( graphics_identifiers[0] );
    if( environment_p == nullptr )
        return -39;
    
    
    // Declare a pointer
    Graphics::Window *window_p = nullptr;
    
    {
        window_p = Graphics::Window::alloc( *environment_p );
        
        if( window_p == nullptr ) {
            delete environment_p;
            return -40;
        }
    }
    std::string title = "Future Cop Map Viewer";

    window_p->setWindowTitle( title );
    if( window_p->center() != 1 )
        std::cout << "The window had failed to center! " << window_p->center() << std::endl;
    window_p->setDimensions( glm::u32vec2( width, height ) );
    window_p->setFullScreen( true );
    
    if( window_p->attach() != 1 ) {
        delete environment_p;
        return -40;
    }
    
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

    if( !manager.hasEntry( iff_mission_id ) ) {
        Data::Manager::listIDs( std::cout );
        delete environment_p;
        return -1;
    }

    auto entry = manager.getIFFEntry( iff_mission_id );
    entry.importance = Data::Manager::Importance::NEEDED;

    if( !manager.setIFFEntry( iff_mission_id, entry ) ) {
        delete environment_p;
        return -2;
    }
    
    manager.togglePlatform( platform, true );
    
    if( platform_all )
        manager.togglePlatform( Data::Manager::Platform::ALL, true );

    auto number_of_iffs = manager.setLoad( load_all );

    if( number_of_iffs < 2 ) {
        std::cout << "The number IFF " << number_of_iffs << " is not enough." << std::endl;
        delete environment_p;
        return -3;
    }
    
    std::vector<Data::Mission::IFF*> loaded_IFFs;
    
    Data::Mission::IFF *global_r = manager.getIFFEntry( global_id ).getIFF( platform );
    if( global_r != nullptr )
        loaded_IFFs.push_back( global_r );
    else
        std::cout << "The global IFF " << global_id << " did not load." << std::endl;

    Data::Mission::IFF *resource_r = manager.getIFFEntry( iff_mission_id ).getIFF( platform );
    if( resource_r != nullptr )
        loaded_IFFs.push_back( resource_r );
    else
        std::cout << "The mission IFF " << iff_mission_id << " did not load." << std::endl;

    // First get the model textures from the resource file.
    if( resource_r != nullptr ) {
        auto cbmp_resources = Data::Mission::BMPResource::getVector( *resource_r );

        int status = environment_p->setupTextures( cbmp_resources );

        if( status < 0 )
            std::cout << (-status) << " general textures had failed to load out of " << cbmp_resources.size() << std::endl;
    }

    // Load all the 3D meshes from the resource as well.
    if( resource_r != nullptr ) {
        auto cobj_resources = Data::Mission::ObjResource::getVector( *resource_r );

        int status = environment_p->setModelTypes( cobj_resources );

        if( status < 0 )
            std::cout << (-status) << " 3d meshes had failed to load out of " << cobj_resources.size() << std::endl;
    }
    
    // Get the font from the resource file.
    if( Graphics::Text2DBuffer::loadFonts( *environment_p, loaded_IFFs ) == 0 ) {
        std::cout << "Fonts missing!" << std::endl;
    }
    
    if( resource_r != nullptr ) {
        auto til_resources = Data::Mission::TilResource::getVector( *resource_r );
        environment_p->setMap( *Data::Mission::PTCResource::getVector( *resource_r ).at( 0 ), til_resources );
    }

    bool viewer_loop = true;

    Graphics::Text2DBuffer *text_2d_buffer_r = Graphics::Text2DBuffer::alloc( *environment_p );

    // Setup the camera
    Graphics::Camera *first_person_r = Graphics::Camera::alloc( *environment_p );
    first_person_r->attachText2DBuffer( *text_2d_buffer_r );
    window_p->attachCamera( *first_person_r );
    first_person_r->setViewportOrigin( glm::u32vec2( 0, 0 ) );
    first_person_r->setViewportDimensions( glm::u32vec2( width, height ) );
    glm::mat4 extra_matrix_0;
    glm::mat4 extra_matrix_1;
    glm::mat4 extra_matrix_2;
    
    extra_matrix_0 = glm::ortho( 0.0f, static_cast<float>(width), -static_cast<float>(height), 0.0f, -1.0f, 1.0f );
    
    first_person_r->setProjection2D( extra_matrix_0 );

    extra_matrix_0 = glm::perspective( glm::pi<float>() / 4.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 200.0f );

    first_person_r->setProjection3D( extra_matrix_0 );

    // Setup the timer
    auto last_time = std::chrono::high_resolution_clock::now();
    auto this_time = last_time;
    auto delta = this_time - last_time;

    float glow_amount = 0.0f;
    int current_tile_selected = -1;
    unsigned til_polygon_type_selected = 111;

    glm::vec3 position_of_camera = glm::vec3( 105, 0, 96 );
    glm::vec4 movement_of_camera = glm::vec4( 0, 0, 0, 0 );
    glm::vec2 rotation = glm::vec2( 0, glm::pi<float>() / 4.0f );
    double distance_away = -10;
    bool is_camera_moving = false;

    if( window_p->center() != 1 )
        std::cout << "The window had failed to center! " << window_p->center() << std::endl;

    // Setup the controls
    auto control_system_p = Controls::System::getSingleton(); // create the new system for controls
    auto player_1_controller_r = new Controls::StandardInputSet( "Player 1" ); // It is not player_1_controller_r job to delete itself.
    control_system_p->addInputSet( player_1_controller_r );

    control_system_p->allocateCursor();
    auto control_cursor_r = control_system_p->getCursor();

    viewer_loop = configure_input( control_system_p, environment_p, text_2d_buffer_r, "controls");
    
    if( resource_r == nullptr ) {
        display_game_files_missing( control_system_p, environment_p, text_2d_buffer_r, &manager, iff_mission_id, platform );
        delete control_system_p;
        delete environment_p;
        return -4;
    }

    bool update_culling = true;

    if( update_culling )
        environment_p->setupFrame();
    
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
                    movement_of_camera.z = -16;
                else
                    movement_of_camera.z = 0;
            }
            
            input_r = player_1_controller_r->getInput( Controls::StandardInputSet::Buttons::DOWN );
            if( input_r->isChanged() )
            {
                if( input_r->getState() > 0.5 )
                    movement_of_camera.z = 16;
                else
                    movement_of_camera.z = 0;
            }

            input_r = player_1_controller_r->getInput( Controls::StandardInputSet::Buttons::LEFT );
            if( input_r->isChanged() )
            {
                if( input_r->getState() > 0.5 )
                    movement_of_camera.x = -16;
                else
                    movement_of_camera.x = 0;
            }

            input_r = player_1_controller_r->getInput( Controls::StandardInputSet::Buttons::RIGHT );
            if( input_r->isChanged() )
            {
                if( input_r->getState() > 0.5 )
                    movement_of_camera.x = 16;
                else
                    movement_of_camera.x = 0;
            }
            
            input_r = player_1_controller_r->getInput( Controls::StandardInputSet::Buttons::ACTION );
            if( input_r->isChanged() )
            {
                if( input_r->getState() > 0.5 )
                    is_camera_moving = true;
                else
                    is_camera_moving = false;
            }

            input_r = player_1_controller_r->getInput( Controls::StandardInputSet::Buttons::ROTATE_LEFT );
            if( input_r->isChanged() && input_r->getState() > 0.5 )
            {
                if( !is_camera_moving )
                {
                    // Stop the blinking on the previous current_tile_selected
                    environment_p->setTilBlink( current_tile_selected, -1.0 );
                    
                    current_tile_selected--;
                    
                    // Set the next current_tile_selected to flash
                    if( environment_p->setTilBlink( current_tile_selected, 1.0 ) == 0 ) {
                        current_tile_selected = environment_p->getTilAmount();
                    }
                }
                else
                {
                    til_polygon_type_selected--;
                    
                    if( environment_p->setTilPolygonBlink( til_polygon_type_selected ) <= 0 ) {
                        til_polygon_type_selected = 111;
                        environment_p->setTilPolygonBlink( til_polygon_type_selected );
                    }
                }
            }

            input_r = player_1_controller_r->getInput( Controls::StandardInputSet::Buttons::ROTATE_RIGHT );
            if( input_r->isChanged() && input_r->getState() > 0.5 )
            {
                if( !is_camera_moving )
                {
                    // Stop the blinking on the previous current_tile_selected
                    environment_p->setTilBlink( current_tile_selected, -1.0 );
                    
                    current_tile_selected++;
                    
                    // Set the next current_tile_selected to flash
                    if( environment_p->setTilBlink( current_tile_selected, 1.0 ) == 0 ) {
                        current_tile_selected = -1;
                    }
                }
                else
                {
                    til_polygon_type_selected++;
                    
                    if( environment_p->setTilPolygonBlink( til_polygon_type_selected ) <= 0 ) {
                        til_polygon_type_selected = 0;
                        environment_p->setTilPolygonBlink( til_polygon_type_selected );
                    }
                }
            }

            input_r = player_1_controller_r->getInput( Controls::StandardInputSet::Buttons::CAMERA );
            if( input_r->isChanged() && input_r->getState() > 0.5 )
            {
                Utilities::Image2D image_screenshot( width, height, Utilities::PixelFormatColor_R8G8B8A8() );

                if( environment_p->screenshot( image_screenshot ) ) {
                    Utilities::Buffer file;
                    Utilities::ImageFormat::Chooser chooser;
                    auto the_choosen_r = chooser.getWriterReference( image_screenshot );
                    if( the_choosen_r != nullptr ) {
                        the_choosen_r->write( image_screenshot, file);
                        file.write( the_choosen_r->appendExtension( "screenshot" ) );
                    }
                }
            }

            input_r = player_1_controller_r->getInput( Controls::StandardInputSet::Buttons::JUMP );
            if( input_r->isChanged() && input_r->getState() > 0.5 )
            {
                update_culling = !update_culling;
            }
        }

        if( control_cursor_r != nullptr && control_cursor_r->isChanged() )
        {
            auto input_r = control_cursor_r->getInput( Controls::CursorInputSet::Inputs::MIDDLE_BUTTON );
            if( input_r->isChanged() )
            {
                if( input_r->getState() > 0.5 )
                    is_camera_moving = true;
                else
                    is_camera_moving = false;
            }

            if( is_camera_moving )
            {
                input_r = control_cursor_r->getInput( Controls::CursorInputSet::Inputs::MOTION_X );
                rotation.x += delta_f * static_cast<double>( input_r->getState() ) * (16.0 / 3.14);

                input_r = control_cursor_r->getInput( Controls::CursorInputSet::Inputs::MOTION_Y );
                rotation.y += delta_f * static_cast<double>( input_r->getState() ) * (16.0 / 3.14);
            }

            input_r = control_cursor_r->getInput( Controls::CursorInputSet::Inputs::WHEEL_Y );
            if( input_r->isChanged() )
            {
                 distance_away += 16.0 * delta_f * static_cast<double>( input_r->getState() );
            }
        }

        extra_matrix_0 = glm::rotate( glm::mat4(1.0f), -rotation.x, glm::vec3( 0.0, 1.0, 0.0 ) );
        glm::vec4 tmp = extra_matrix_0 * glm::vec4(movement_of_camera.x * delta_f, movement_of_camera.y * delta_f, movement_of_camera.z * delta_f, 1 );

        position_of_camera.x += tmp.x;
        position_of_camera.y += tmp.y;
        position_of_camera.z += tmp.z;

        extra_matrix_0 = glm::translate( glm::mat4(1.0f), glm::vec3( 0, 0, distance_away ) );
        extra_matrix_1 = glm::rotate( glm::mat4(1.0f), rotation.y, glm::vec3( 1.0, 0.0, 0.0 ) ); // rotate up and down.
        extra_matrix_2 = extra_matrix_0 * extra_matrix_1;
        extra_matrix_1 = glm::rotate( glm::mat4(1.0f), rotation.x, glm::vec3( 0.0, 1.0, 0.0 ) ); // rotate left and right.
        extra_matrix_0 = extra_matrix_2 * extra_matrix_1;
        extra_matrix_1 = glm::translate( glm::mat4(1.0f), -position_of_camera );
        extra_matrix_2 = extra_matrix_0 * extra_matrix_1;

        first_person_r->setView3D( extra_matrix_2 );

        if( text_2d_buffer_r->setFont( 6 ) == -3 )
            text_2d_buffer_r->setFont( 2 );
        text_2d_buffer_r->setColor( glm::vec4( 1, 0, 0, 1 ) );
        text_2d_buffer_r->setPosition( glm::vec2( 0, 0 ) );
        text_2d_buffer_r->print( "Position = (" + std::to_string(position_of_camera.x) + ", " + std::to_string(position_of_camera.y) + ", " + std::to_string(position_of_camera.z) + ")" );

        if( text_2d_buffer_r->setFont( 5 ) == -3 )
            text_2d_buffer_r->setFont( 2 );
        text_2d_buffer_r->setColor( glm::vec4( 0, 1, 0, 1 ) );
        text_2d_buffer_r->setPosition( glm::vec2( 0, 20 ) );
        text_2d_buffer_r->print( "Rotation = (" + std::to_string(rotation.x) + ", " + std::to_string(rotation.y) + ")" );
        
        if( til_polygon_type_selected != 111 ) {
            if( text_2d_buffer_r->setFont( 3 ) == -3 )
                text_2d_buffer_r->setFont( 1 );
            text_2d_buffer_r->setColor( glm::vec4( 1, 0, 1, 1 ) );
            text_2d_buffer_r->setPosition( glm::vec2( 0, 40 ) );
            text_2d_buffer_r->print( "Selected Polygon Type = " + std::to_string( til_polygon_type_selected ) );
        }

        if( current_tile_selected >= 0 && static_cast<unsigned>(current_tile_selected) < til_resources.size() ) {
            if( text_2d_buffer_r->setFont( 3 ) == -3 )
                text_2d_buffer_r->setFont( 1 );
            text_2d_buffer_r->setColor( glm::vec4( 0, 1, 1, 1 ) );
            text_2d_buffer_r->setPosition( glm::vec2( 0, 60 ) );
            text_2d_buffer_r->print( "Ctil Resource ID = " + std::to_string( til_resources.at(current_tile_selected)->getResourceID() ) );
            text_2d_buffer_r->setColor( glm::vec4( 0, 1, 1, 1 ) );
            text_2d_buffer_r->setPosition( glm::vec2( 0, 80 ) );
            text_2d_buffer_r->print( "Ctil Offset = " + std::to_string( til_resources.at(current_tile_selected)->getOffset() ) );
        }

        if( update_culling )
            environment_p->setupFrame();

        environment_p->drawFrame();
        environment_p->advanceTime( delta_f );

        text_2d_buffer_r->reset();

        if( delta < std::chrono::microseconds(17) )
            std::this_thread::sleep_for( std::chrono::microseconds(17) - delta );

        last_time = this_time;
    }

    delete control_system_p;
    delete environment_p;

    // Clean up
    Graphics::Environment::deinitEntireSystem( graphics_identifiers[0] );

    return 0;
}
