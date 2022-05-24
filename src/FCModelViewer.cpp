#include <iostream>
#include <ratio>
#include <chrono>
#include <thread>

#include "Data/Manager.h"

#include "Data/Mission/IFF.h"
#include "Data/Mission/ObjResource.h"
#include "Data/Mission/BMPResource.h"
#include "Data/Mission/Til/Mesh.h"

#include "Utilities/Math.h"
#include "Graphics/Environment.h"
#include "Controls/System.h"
#include "Controls/StandardInputSet.h"

bool test_Math();

int main(int argc, char** argv)
{
    // Data::Mission::Til::Mesh::loadMeshScript( "./tile_set.json", nullptr );
    const int WIDTH = 1024;
    const int HEIGHT = 764;

    test_Math();

    std::string type = Data::Mission::ObjResource::FILE_EXTENSION;
    std::string iff_mission_id = "pa_urban_jungle";
    std::string global_id      = "global";
    std::string start_number   = "0";
    Data::Manager::Platform platform = Data::Manager::Platform::WINDOWS;
    std::string global_path = "";
    std::string mission_path = "";
    std::string variable_name = "";

    for( int index = 1; index < argc; index++ ) {
        std::string input = std::string( argv[index] );
        
        if( variable_name.compare("") == 0 ) {
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
            if( input.find( "--start" ) == 0 )
                variable_name = "--start";
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
            if( variable_name.find( "--start" ) == 0 )
                start_number = input;
            
            variable_name = "";
        }
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

    if( !manager.hasEntry( iff_mission_id ) )
        return -1;

    auto entry = manager.getIFFEntry( iff_mission_id );
    entry.importance = Data::Manager::Importance::NEEDED;

    if( !manager.setIFFEntry( iff_mission_id, entry ) )
        return -2;

    // manager.togglePlatform( Data::Manager::Platform::ALL, true );
    manager.togglePlatform( platform, true );

    if( manager.setLoad( Data::Manager::Importance::NEEDED ) < 2 )
        return -3;

    Data::Mission::IFF &resource = *manager.getIFFEntry( iff_mission_id ).getIFF( platform );
    Data::Mission::IFF &global = *manager.getIFFEntry( global_id ).getIFF( platform );

	// Declare a pointer to the Environment.
    Graphics::Window *window =  new Graphics::Window();

    Graphics::Environment::initSystem();
    std::cout << "Graphics::Environment::initSystem() loaded!" << std::endl;
    
    Graphics::Environment *environment = new Graphics::Environment();

    window->setWindowTitle( "Future Cop Individual Model Viewer" );
    window->setDimensions( Utilities::DataTypes::Vec2UInt( WIDTH, HEIGHT ) );
    if( environment->attachWindow( *window ) != 1 )
        return -40;

    // First get the model textures from the resource file.
    {
        auto cbmp_resources = Data::Mission::BMPResource::getVector( resource );

        int status = environment->setModelTextures( cbmp_resources );

        if( status < 0 )
            std::cout << (-status) << " general textures had failed to load out of " << cbmp_resources.size() << std::endl;
    }

	bool viewer_loop = true;
    bool is_error = false;
    std::string error_message = "";
    unsigned int number_of_models = 0;

    // Load all the 3D meshes from the resource as well.
    {
        auto cobj_resources = Data::Mission::ObjResource::getVector( resource );

        number_of_models = cobj_resources.size();

        if( number_of_models != 0 ) // If the status is not one then there is an error
        {
            int failures = environment->setModelTypes( cobj_resources );

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
    {
        auto font_resources = Data::Mission::FontResource::getVector( resource );

        if( font_resources.size() != 0 )
            environment->setFonts( font_resources );
        else
        {
            font_resources = Data::Mission::FontResource::getVector( global );
            environment->setFonts( font_resources );
            if( font_resources.size() == 0 )
                std::cout << " general fonts had failed to load out of " << font_resources.size() << std::endl;
        }
    }

    int times = 0;

    // Setup the camera
    Graphics::Camera *first_person = new Graphics::Camera();
    first_person->setViewportOrigin( Utilities::DataTypes::Vec2UInt( 0, 0 ) );
    first_person->setViewportDimensions( Utilities::DataTypes::Vec2UInt( WIDTH, HEIGHT ) );
    window->attachCamera( *first_person );
    Utilities::DataTypes::Mat4 extra_matrix_0;
    Utilities::DataTypes::Mat4 extra_matrix_1;
    Utilities::DataTypes::Mat4 extra_matrix_2;

    // Setup the font
    Graphics::Text2DBuffer *text_2d_buffer = new Graphics::Text2DBuffer( 128 ); // 128 Kibibytes.
    text_2d_buffer->loadFontLibrary( *environment );
    Utilities::Math::setOthro( extra_matrix_0, 0, WIDTH, -HEIGHT, 0, -1.0, 1.0 );
    first_person->attachText2DBuffer( *text_2d_buffer );
    first_person->setProjection2D( extra_matrix_0 );

    Utilities::Math::setPerspective( extra_matrix_0, M_PI / 4.0f, static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.1f, 100.0f );
    first_person->setProjection3D( extra_matrix_0 );

    Utilities::Math::setRotation( extra_matrix_0, Utilities::DataTypes::Vec3( 1.0f, 0.0f, 0.0f ), M_PI / 4.0f );

    if( type.compare( "til" ) == 0 )
        Utilities::Math::setTranslation( extra_matrix_1, Utilities::DataTypes::Vec3( 0.0f, -18.0f, -24.0f ) );
    else
        Utilities::Math::setTranslation( extra_matrix_1, Utilities::DataTypes::Vec3( 0.0f, -4.0f, -5.0f ) );

    Utilities::Math::multiply( extra_matrix_2, extra_matrix_0, extra_matrix_1 );
    first_person->setView3D( extra_matrix_2 );

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
        text_2d_buffer->setFont( 0 );
        text_2d_buffer->setColor( Utilities::DataTypes::Vec4( 1, 1, 1, 1 ) );
        text_2d_buffer->setPosition( Utilities::DataTypes::Vec2( 0, 0 ) );
        text_2d_buffer->print( error_message );

        control_system_p->advanceTime( 0 );

        if( control_system_p->isOrderedToExit() )
            is_error = false;

        environment->drawFrame();

        environment->advanceTime( 0 );

        std::this_thread::sleep_for( std::chrono::microseconds(40) ); // delay for 40ms the frequency really does not mater for things like this. Run 25 times in one second.
    }

    for( unsigned x = 0; x < control_system_p->amountOfInputSets(); x++ )
    {
        auto input_set_r = control_system_p->getInputSet( x );

        for( unsigned y = 0; input_set_r->getInput( y ) != nullptr; y++ )
        {
            int status = 0;

            while( status < 1  && viewer_loop )
            {
                text_2d_buffer->setFont( 2 );
                text_2d_buffer->setColor( Utilities::DataTypes::Vec4( 1, 1, 1, 1 ) );
                text_2d_buffer->setPosition( Utilities::DataTypes::Vec2( 0, 0 ) );
                text_2d_buffer->print( "Input Set: \"" + input_set_r->getName() +"\"" );

                text_2d_buffer->setFont( 5 );
                text_2d_buffer->setColor( Utilities::DataTypes::Vec4( 1, 0.25, 0.25, 1 ) );
                text_2d_buffer->setPosition( Utilities::DataTypes::Vec2( 0, 20 ) );
                text_2d_buffer->print( "Enter a key for Input, \"" + input_set_r->getInput( y )->getName() +"\"" );

                status = control_system_p->pollEventForInputSet( x, y );

                if( control_system_p->isOrderedToExit() )
                    viewer_loop = false;

                environment->drawFrame();
                environment->advanceTime( 0 );

                text_2d_buffer->reset();

                std::this_thread::sleep_for( std::chrono::microseconds(40) ); // delay for 40ms the frequency really does not mater for things like this. Run 25 times in one second.
            }
        }
    }

    float rotate = 0.0;
    float count_down = 0.0;
    unsigned int cobj_index = std::stoi( start_number );

    bool nextModel = false; // The next model

    Graphics::ModelInstance* displayed_instance = new Graphics::ModelInstance( Utilities::DataTypes::Vec3( 0, 0, 0 ) );

    environment->attachInstanceObj( cobj_index, *displayed_instance );

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
            for( unsigned x = 0; player_1_controller_r->getInput( x ) != nullptr; x++ )
            {
                auto input_r = player_1_controller_r->getInput( x );

                if( input_r->isChanged() && input_r->getState() < 0.5 )
                {
                    nextModel = true;
                    x = Controls::StandardInputSet::Buttons::TOTAL_BUTTONS;
                }
            }
        }

        rotate += time_unit(delta).count();

        displayed_instance->setRotation( Utilities::Math::setRotation( Utilities::DataTypes::Vec3( 0.0f, 1.0f, 0.0f ), rotate ) );

        count_down -= time_unit(delta).count();

        if( nextModel && count_down < 0.0f )
        {
            environment->deleteQueue( displayed_instance );

            cobj_index++;

            displayed_instance = new Graphics::ModelInstance( Utilities::DataTypes::Vec3( 0, 0, 0 ) );

            // Check to see if the cobj_index is in bounds
            if( environment->attachInstanceObj( cobj_index, *displayed_instance ) != 1 )
            {
                // If not change cobj_index to zero.
                cobj_index = 0;

                // If it is still out of bounds then there is no cobj to view.
                if( environment->attachInstanceObj( cobj_index, *displayed_instance ) != 1 )
                {
                    // Exit the program for there is no model to view.
                    viewer_loop = false;
                }
            }

            nextModel = false;
            count_down = 0.5f;
            rotate = 0;
        }

        text_2d_buffer->setFont( 2 );
        text_2d_buffer->setColor( Utilities::DataTypes::Vec4( 1, 1, 1, 1 ) );
        text_2d_buffer->setPosition( Utilities::DataTypes::Vec2( 0, 0 ) );
        text_2d_buffer->print( "index = " + std::to_string(cobj_index) );

        environment->drawFrame();
        environment->advanceTime( time_unit(delta).count() );

        text_2d_buffer->reset();

        if( delta < std::chrono::microseconds(17) )
            std::this_thread::sleep_for( std::chrono::microseconds(17) - delta );

        last_time = this_time;
    };

    // Clean up
    delete control_system_p;
    delete environment;

    Graphics::Environment::deinitEntireSystem();

    return 0;
}
