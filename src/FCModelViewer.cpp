#include <iostream>
#include <ratio>
#include <chrono>
#include <thread>

#include "Config.h"

#include "Data/Manager.h"

#include "Data/Mission/IFF.h"
#include "Data/Mission/ObjResource.h"
#include "Data/Mission/BMPResource.h"
#include "Data/Mission/Til/Mesh.h"

#include "Utilities/DataHandler.h"
#include "Graphics/Environment.h"
#include "Graphics/Text2DBuffer.h"
#include "Controls/System.h"
#include "Controls/StandardInputSet.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "ConfigureInput.h"
#include "SplashScreens.h"

namespace {
void helpExit( std::ostream &stream ) {
   
    stream << "\n";
    stream << "Future Cop: MIT - Model viewer/exporter (version " << FUTURE_COP_MIT_VERSION << ")\n";
    stream << "\n";
    stream << "Usage:" << "\n";
    stream << "  FCModelViewer [-h|--help]" << "\n";
    stream << "                [--width <number>] [--height <number>]" << "\n";
    stream << "                [-w] [-m] [-p] [--id <id>]" << "\n";
    stream << "                [--model-export-path <path>] [--type ??] [--start <number>]" << "\n";
    stream << "                [--global <path>] [--path <path>]" << "\n";
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
    stream << "  Export options:" << "\n";
    stream << "    --model-export-path <path>  Where to export the models, must point to an existing directory" <<"\n";
    stream << "    --type               ??" <<"\n";
    stream << "    --start              The index of the model to look at when the program starts up" <<"\n";
    stream << "  Maps:" << "\n";
    stream << "    --global <path>  Path to the global file which every map uses." << "\n";
    stream << "    --path <path>    Path to the mission file which contains the rest of the data like the map." << "\n";
    stream << "\n";
    exit( 0 );
}

glm::mat4 placeView( float angle, float distance, glm::vec3 position ) {
    glm::mat4 mat = glm::rotate( glm::translate( glm::mat4(1.0f), glm::vec3( 0, 0, -distance ) ), angle, glm::vec3( 1.0f, 0.0f, 0.0f ) );
    return glm::translate( mat, -position );
}
}

int main(int argc, char** argv)
{
    int WIDTH = 0;
    int HEIGHT = 0;

    std::string type = Data::Mission::ObjResource::FILE_EXTENSION;
    std::string iff_mission_id = "pa_urban_jungle";
    std::string global_id      = "global";
    std::string model_id       = "0";
    Data::Manager::Platform platform = Data::Manager::Platform::WINDOWS;
    std::string global_path = "";
    std::string mission_path = "";
    std::string variable_name = "";
    std::string resource_export_path = "";
    bool exported_textures = false;

    for( int index = 1; index < argc; index++ ) {
        std::string input = std::string( argv[index] );
        
        if( variable_name.empty() ) {
            if( input.find( "--help") == 0 || input.find( "-h") == 0 )
                helpExit( std::cout );
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
            if( input.find( "--type") == 0 )
                variable_name = "--type";
            else
            if( input.find( "--model-id" ) == 0 )
                variable_name = "--model-id";
            else
            if( input.find( "--model-export-path" ) == 0 )
                variable_name = "--model-export-path";
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
            if( variable_name.find( "--type") == 0 )
                type = input;
            else
            if( variable_name.find( "--model-id" ) == 0 )
                model_id = input;
            else
            if( variable_name.find( "--model-export-path" ) == 0 ) {
                resource_export_path = input;
                if( *resource_export_path.end() != '/')
                    resource_export_path += "/";
            }
            else
            if( variable_name.find( "--width") == 0 )
                WIDTH = std::stoi(input);
            else
            if( variable_name.find( "--height") == 0 )
                HEIGHT = std::stoi(input);
            
            variable_name = "";
        }
    }
    
    if( WIDTH <= 0 )
        WIDTH = 640;
    if( HEIGHT <= 0 )
        HEIGHT = 480;

    Utilities::DataHandler::init();
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
        return -1;
    }

    auto entry = manager.getIFFEntry( iff_mission_id );
    entry.importance = Data::Manager::Importance::NEEDED;

    if( !manager.setIFFEntry( iff_mission_id, entry ) )
        return -2;

    // manager.togglePlatform( Data::Manager::Platform::ALL, true );
    manager.togglePlatform( platform, true );

    if( manager.setLoad( Data::Manager::Importance::NEEDED ) < 2 )
        return -3;
    
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

    auto graphics_identifiers = Graphics::Environment::getAvailableIdentifiers();
    
    if( graphics_identifiers.size() == 0 )
        return -37;
    
    if( !Graphics::Environment::isIdentifier( graphics_identifiers[0] ) )
        return -38;

    Graphics::Environment::initSystem( graphics_identifiers[0] );
    
    Graphics::Environment *environment_p = Graphics::Environment::alloc( graphics_identifiers[0] );
    if( environment_p == nullptr )
        return -39;

    // Declare a pointer to the Environment.
    Graphics::Window *window_r = nullptr;
    
    {
        window_r = Graphics::Window::alloc( *environment_p );
        
        if( window_r == nullptr ) {
            delete environment_p;
            return -40;
        }
    }
    
    
    window_r->setWindowTitle( "Future Cop Individual Model Viewer" );
    window_r->setDimensions( glm::u32vec2( WIDTH, HEIGHT ) );
    
    window_r->attach();

    // First get the model textures from the resource file.
    if( resource_r != nullptr ) {
        auto cbmp_resources = Data::Mission::BMPResource::getVector( *resource_r );

        int status = environment_p->setupTextures( cbmp_resources );

        if( status < 0 )
            std::cout << (-status) << " general textures had failed to load out of " << cbmp_resources.size() << std::endl;
    }

	bool viewer_loop = true;
    bool is_error = false;
    std::string error_message = "";
    unsigned int number_of_models = 0;

    // Load all the 3D meshes from the resource as well.
    if( resource_r != nullptr ) {
        auto cobj_resources = Data::Mission::ObjResource::getVector( *resource_r );

        number_of_models = cobj_resources.size();

        if( number_of_models != 0 ) // If the status is not one then there is an error
        {
            int failures = environment_p->setModelTypes( cobj_resources );

            if( failures != 0 )
            {
                is_error = true;
                error_message += std::to_string(failures) + " 3D models had failed to load out of " + std::to_string(number_of_models);
            }
        }
        else
        {
            error_message += "In the IFF for " + iff_mission_id + " there are no models detected. Thus this model viewer has no 3D model to show.";
            is_error = true;
            viewer_loop = false;
        }
    }

    // Get the font from the resource file.
    if( Graphics::Text2DBuffer::loadFonts( *environment_p, loaded_IFFs ) == 0 )
    {
        std::cout << "Fonts missing!" << std::endl;
    }

    // Setup the camera
    Graphics::Camera *first_person = new Graphics::Camera();
    first_person->setViewportOrigin( glm::u32vec2( 0, 0 ) );
    first_person->setViewportDimensions( glm::u32vec2( WIDTH, HEIGHT ) );
    window_r->attachCamera( *first_person );
    glm::mat4 extra_matrix_0;

    // Setup the font
    Graphics::Text2DBuffer *text_2d_buffer_r = Graphics::Text2DBuffer::alloc( *environment_p );
    extra_matrix_0 = glm::ortho( 0.0f, static_cast<float>(WIDTH), -static_cast<float>(HEIGHT), 0.0f, -1.0f, 1.0f );
    
    first_person->attachText2DBuffer( *text_2d_buffer_r );
    first_person->setProjection2D( extra_matrix_0 );

    extra_matrix_0 = glm::perspective( glm::pi<float>() / 4.0f, static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.1f, 100.0f );
    first_person->setProjection3D( extra_matrix_0 );

    // Setup the timer
    auto last_time = std::chrono::high_resolution_clock::now();
    auto this_time = last_time;
    auto delta = this_time - last_time;

    // Setup the controls
    auto control_system_p = Controls::System::getSingleton(); // create the new system for controls
    auto player_1_controller_r = new Controls::StandardInputSet( "Player 1" ); // It is not player_1_controller_r job to delete itself.
    control_system_p->addInputSet( player_1_controller_r );

    // This is the method used to convert time to floats.
    using time_unit = std::chrono::duration<float, std::ratio<1>>;

    // If there is an error detected it is time to show it.
    while( is_error )
    {
        text_2d_buffer_r->setFont( 1 );
        text_2d_buffer_r->setColor( glm::vec4( 1, 1, 1, 1 ) );
        text_2d_buffer_r->setPosition( glm::vec2( 0, 0 ) );
        text_2d_buffer_r->print( error_message );

        control_system_p->advanceTime( 0 );

        if( control_system_p->isOrderedToExit() )
            is_error = false;

        environment_p->drawFrame();

        environment_p->advanceTime( 0 );

        std::this_thread::sleep_for( std::chrono::microseconds(40) ); // delay for 40ms the frequency really does not mater for things like this. Run 25 times in one second.
    }

    viewer_loop = configure_input( control_system_p, environment_p, text_2d_buffer_r, "controls");
    
    if( resource_r == nullptr ) {
        display_game_files_missing( control_system_p, environment_p, text_2d_buffer_r, &manager, iff_mission_id, platform );
        delete control_system_p;
        delete environment_p;
        return -4;
    }
    
    auto obj_vector = Data::Mission::ObjResource::getVector( *resource_r );

    float rotate = 0.0;
    float count_down = 0.0;
    unsigned int cobj_index = std::stoi( model_id );
    
    // Convert the id into an index.
    for( auto i = obj_vector.begin(); i != obj_vector.end(); i++ ) {
        if( (*i)->getResourceID() == cobj_index ) {
            cobj_index = (*i)->getIndexNumber();
            i = obj_vector.end() - 1;
        }
    }
    
    Graphics::ModelInstance* displayed_instance_p = Graphics::ModelInstance::alloc( *environment_p, obj_vector.at(cobj_index)->getResourceID(), glm::vec3(0,0,0) );
    
    glm::vec3 position(0,0,0);
    float radius = 1.0f;
    
    if( displayed_instance_p != nullptr )
        displayed_instance_p->getBoundingSphere( position, radius );
    
    first_person->setView3D( placeView( glm::pi<float>() / 4.0f, radius + 4.0f, position ) );

    while(viewer_loop)
    {
        // The time must be registered first
        this_time = std::chrono::high_resolution_clock::now();
        delta = this_time - last_time;

        // Start the input routine

        // Make the control system poll all the inputs.
        control_system_p->advanceTime( time_unit(delta).count() );

        if( control_system_p->isOrderedToExit() )
            viewer_loop = false;

        if( player_1_controller_r->isChanged() )
        {
            auto input_r = player_1_controller_r->getInput( Controls::StandardInputSet::Buttons::ACTION );
            
            if( input_r->isChanged() && input_r->getState() < 0.5 && !resource_export_path.empty() ) {
                // Export the textures from the mission file.
                if(!exported_textures) {
                    auto bmps = Data::Mission::BMPResource::getVector( *resource_r );
                    
                    for( auto it : bmps ) {
                        auto str = resource_export_path + (*it).getFullName( (*it).getResourceID() );
                        
                        (*it).write( str.c_str() );
                    }
                    
                    exported_textures = true;
                }
                
                auto obj = Data::Mission::ObjResource::getVector( *resource_r )[ cobj_index ];
                
                auto str = resource_export_path + obj->getFullName( obj->getResourceID() );
                
                obj->write( str.c_str() );
            }
            
            input_r = player_1_controller_r->getInput( Controls::StandardInputSet::Buttons::RIGHT );
            
            int next = 0;

            if( input_r->isChanged() && input_r->getState() < 0.5 && count_down < 0.0f )
                next++;
            
            input_r = player_1_controller_r->getInput( Controls::StandardInputSet::Buttons::LEFT );
            
            if( input_r->isChanged() && input_r->getState() < 0.5 && count_down < 0.0f )
                next--;
            
            if( next != 0 ) {
                if( next > 0 )
                {
                    cobj_index += next;
                    
                    if( cobj_index >= obj_vector.size() )
                        cobj_index = 0;
                }
                else
                {
                    if( cobj_index == 0 )
                        cobj_index = obj_vector.size() - 1;
                    else
                        cobj_index += next;
                }
                    
                if( displayed_instance_p != nullptr )
                    delete displayed_instance_p;
                
                if( Graphics::ModelInstance::exists( *environment_p, obj_vector.at(cobj_index)->getResourceID() ) ) {
                    displayed_instance_p = Graphics::ModelInstance::alloc( *environment_p, obj_vector.at(cobj_index)->getResourceID(), glm::vec3(0,0,0) );
                    
                    std::cout << "Sphere result is "<< displayed_instance_p->getBoundingSphere( position, radius ) << std::endl;
                    std::cout << " position is (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
                    std::cout << " radius is "<< radius << std::endl;
                    
                    first_person->setView3D( placeView( glm::pi<float>() / 4.0f, radius + 4.0f, position ) );
                }
                else
                    displayed_instance_p = nullptr;
                
                
                count_down = 0.5f;
                rotate = 0;
            }
        }

        rotate += time_unit(delta).count();

        if( displayed_instance_p != nullptr )
            displayed_instance_p->setRotation( glm::quat( glm::vec3( 0, rotate, 0 ) ) );

        count_down -= time_unit(delta).count();
        
        if( text_2d_buffer_r->setFont( 3 ) == -3 )
            text_2d_buffer_r->setFont( 1 );
        text_2d_buffer_r->setColor( glm::vec4( 1, 1, 1, 1 ) );
        text_2d_buffer_r->setPosition( glm::vec2( 0, 0 ) );
        text_2d_buffer_r->print( "Resource ID = " + std::to_string(obj_vector.at(cobj_index)->getResourceID()) );
        
        if( !resource_export_path.empty() ) {
            text_2d_buffer_r->setColor( glm::vec4( 1, 0, 1, 1 ) );
            text_2d_buffer_r->setPosition( glm::vec2( 0, 16 ) );
            text_2d_buffer_r->print( "PRESS the \"" + player_1_controller_r->getInput( Controls::StandardInputSet::ACTION )->getName() + "\" button to export model." );
        }

        environment_p->drawFrame();
        environment_p->advanceTime( time_unit(delta).count() );

        text_2d_buffer_r->reset();

        if( delta < std::chrono::microseconds(17) )
            std::this_thread::sleep_for( std::chrono::microseconds(17) - delta );

        last_time = this_time;
    };

    // Clean up
    delete control_system_p;
    delete environment_p;

    Graphics::Environment::deinitEntireSystem( graphics_identifiers[0] );

    return 0;
}
