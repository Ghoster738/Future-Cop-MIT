#include "PrimaryGame.h"
#include "MainProgram.h"
#include "MainMenu.h"

#include "Utilities/ImageFormat/Chooser.h"

#include <ratio>

#include "Config.h"

namespace {

void writeThreadedImage( std::string NAME, Utilities::Image2D *image_screenshot_p ) {
    Utilities::Buffer file;
    Utilities::ImageFormat::Chooser chooser;
    auto the_choosen_r = chooser.getWriterReference( *image_screenshot_p );

    if( the_choosen_r != nullptr ) {
        the_choosen_r->write( *image_screenshot_p, file );
        file.write( NAME );

        auto log = Utilities::logger.getLog( Utilities::Logger::INFO );
        log.output << "Successfully written \"" << the_choosen_r->appendExtension( NAME ) << "\".\n";
    }
    else {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "\"" << NAME << "\" cannot be written because there is not image format that supports the particular pixel format color.\n";
    }

    delete image_screenshot_p;
}

}

PrimaryGame PrimaryGame::primary_game;

PrimaryGame::PrimaryGame() {
    this->counter = std::chrono::seconds(0);
    this->map_index = 0;
}

PrimaryGame::~PrimaryGame() {
}

void PrimaryGame::load( MainProgram &main_program ) {
    this->is_camera_moving          = false;
    this->camera_position_transform = { 0, 0, 0 };
    this->camera_rotation_transform = { 0, 0 };
    this->camera_distance_transform = 0;

    this->current_tile_selected     =  -1;
    this->til_polygon_type_selected = 111;

    main_program.camera_position = { 0, 0, 0 };
    main_program.camera_rotation = glm::vec2( glm::pi<float>() / 4.0f, glm::pi<float>() / 4.0f );
    main_program.camera_distance = -20;
    main_program.centerCamera();

    this->til_resources = Data::Mission::TilResource::getVector( *main_program.resource_r );

    main_program.loadGraphics();
}

void PrimaryGame::unload( MainProgram &main_program ) {
}

void PrimaryGame::grabControls( MainProgram &main_program, std::chrono::microseconds delta ) {
    float delta_f = std::chrono::duration<float, std::ratio<1>>( delta ).count();

    if( main_program.control_system_p->isOrderedToExit() )
        main_program.play_loop = false;

    if( !main_program.controllers_r.empty() && main_program.controllers_r[0]->isChanged() )
    {
        auto input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::UP );
        if( input_r->isChanged() )
        {
            if( input_r->getState() > 0.5 )
                camera_position_transform.z = -16;
            else
                camera_position_transform.z = 0;
        }

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::DOWN );
        if( input_r->isChanged() )
        {
            if( input_r->getState() > 0.5 )
                camera_position_transform.z = 16;
            else
                camera_position_transform.z = 0;
        }

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::LEFT );
        if( input_r->isChanged() )
        {
            if( input_r->getState() > 0.5 )
                camera_position_transform.x = -16;
            else
                camera_position_transform.x = 0;
        }

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::RIGHT );
        if( input_r->isChanged() )
        {
            if( input_r->getState() > 0.5 )
                camera_position_transform.x = 16;
            else
                camera_position_transform.x = 0;
        }

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::ACTION );
        if( input_r->isChanged() )
        {
            if( input_r->getState() > 0.5 )
                is_camera_moving = true;
            else
                is_camera_moving = false;
        }

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::ROTATE_LEFT );
        if( input_r->isChanged() && input_r->getState() > 0.5 )
        {
            if( !is_camera_moving )
            {
                // Stop the blinking on the previous current_tile_selected
                main_program.environment_p->setTilBlink( current_tile_selected, -1.0 );

                current_tile_selected--;

                // Set the next current_tile_selected to flash
                if( main_program.environment_p->setTilBlink( current_tile_selected, 1.0 ) == 0 ) {
                    current_tile_selected = main_program.environment_p->getTilAmount();
                }
            }
            else
            {
                til_polygon_type_selected--;

                if( main_program.environment_p->setTilPolygonBlink( til_polygon_type_selected ) <= 0 ) {
                    til_polygon_type_selected = 111;
                    main_program.environment_p->setTilPolygonBlink( til_polygon_type_selected );
                }
            }
        }

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::ROTATE_RIGHT );
        if( input_r->isChanged() && input_r->getState() > 0.5 )
        {
            if( !is_camera_moving )
            {
                // Stop the blinking on the previous current_tile_selected
                main_program.environment_p->setTilBlink( current_tile_selected, -1.0 );

                current_tile_selected++;

                // Set the next current_tile_selected to flash
                if( main_program.environment_p->setTilBlink( current_tile_selected, 1.0 ) == 0 ) {
                    current_tile_selected = -1;
                }
            }
            else
            {
                til_polygon_type_selected++;

                if( main_program.environment_p->setTilPolygonBlink( til_polygon_type_selected ) <= 0 ) {
                    til_polygon_type_selected = 0;
                    main_program.environment_p->setTilPolygonBlink( til_polygon_type_selected );
                }
            }
        }

        input_r = main_program.controllers_r.front()->getInput( Controls::StandardInputSet::Buttons::CAMERA );
        if( input_r->isChanged() && input_r->getState() > 0.5 )
        {
            const std::string NAME = main_program.options.getScreenshotsDirectory() + Utilities::Logger::getTime();

            {
                auto log = Utilities::logger.getLog( Utilities::Logger::INFO );
                log.output << "Creating screenshot \"" << NAME << "\"\n";
            }

            const auto dimensions = main_program.environment_p->window_p->getDimensions();

            Utilities::Image2D *image_screenshot_p = new Utilities::Image2D( dimensions.x, dimensions.y, Utilities::PixelFormatColor_R8G8B8A8() );

            if( main_program.environment_p->screenshot( *image_screenshot_p ) ) {
                {
                    auto log = Utilities::logger.getLog( Utilities::Logger::DEBUG );
                    log.output << "Launching screenshot \"" << NAME << "\"\n";
                }
                std::thread thread( writeThreadedImage, NAME, image_screenshot_p );

                thread.detach();
            }
            else {
                delete image_screenshot_p;

                auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
                log.output << "Failed to generate screenshot \"" << NAME << "\"\n";
            }
        }

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::TOGGLE_COMMAND_LINE );
        if( input_r->isChanged() )
        {
            MainMenu::main_menu.is_game_on = true;
            MainMenu::main_menu.load( main_program );

            main_program.menu_r = &MainMenu::main_menu;
        }
    }

    if( main_program.control_cursor_r->isChanged() )
    {
        auto input_r = main_program.control_cursor_r->getInput( Controls::CursorInputSet::Inputs::MIDDLE_BUTTON );
        if( input_r->isChanged() )
        {
            if( input_r->getState() > 0.5 )
                is_camera_moving = true;
            else
                is_camera_moving = false;
        }

        if( is_camera_moving )
        {
            input_r = main_program.control_cursor_r->getInput( Controls::CursorInputSet::Inputs::MOTION_X );

            main_program.camera_rotation.x += delta_f * static_cast<double>( input_r->getState() ) * (16.0 / glm::pi<double>());

            input_r = main_program.control_cursor_r->getInput( Controls::CursorInputSet::Inputs::MOTION_Y );

            main_program.camera_rotation.y += delta_f * static_cast<double>( input_r->getState() ) * (16.0 / glm::pi<double>());
        }

        input_r = main_program.control_cursor_r->getInput( Controls::CursorInputSet::Inputs::WHEEL_Y );
        if( input_r->isChanged() )
        {
            camera_distance_transform = 16.0 * static_cast<double>( input_r->getState() );
        }
        else
            camera_distance_transform = 0;
    }

    glm::vec4 tmp = glm::rotate( glm::mat4( 1.0f ), -main_program.camera_rotation.x, glm::vec3( 0.0, 1.0, 0.0 ) ) * (glm::vec4(camera_position_transform.x, camera_position_transform.y, camera_position_transform.z, 1 ) * delta_f );

    main_program.camera_position += glm::vec3( tmp.x, tmp.y, tmp.z );
    main_program.camera_distance += delta_f * camera_distance_transform;

    #ifdef FCOption_AUTOMATIC_MAP_SWITCHING
    this->counter += delta;

    if( this->counter > std::chrono::seconds(10) ) {
        main_program.transitionToResource( *Data::Manager::map_iffs[ this->map_index ] );

        this->counter -= std::chrono::seconds(10);
        this->map_index = (this->map_index + 1) % Data::Manager::AMOUNT_OF_IFF_IDS;
    }
    #endif
}

void PrimaryGame::display( MainProgram &main_program ) {
    const auto text_2d_buffer_r = main_program.text_2d_buffer_r;

    if( text_2d_buffer_r->setFont( 6 ) == -3 )
            text_2d_buffer_r->setFont( 2 );
    text_2d_buffer_r->setColor( glm::vec4( 1, 0, 0, 1 ) );
    text_2d_buffer_r->setPosition( glm::vec2( 0, 0 ) );
    text_2d_buffer_r->print( "Position = (" + std::to_string(main_program.camera_position.x) + ", " + std::to_string(main_program.camera_position.y) + ", " + std::to_string(main_program.camera_position.z) + ")" );

    if( text_2d_buffer_r->setFont( 5 ) == -3 )
        text_2d_buffer_r->setFont( 2 );
    text_2d_buffer_r->setColor( glm::vec4( 0, 1, 0, 1 ) );
    text_2d_buffer_r->setPosition( glm::vec2( 0, 20 ) );
    text_2d_buffer_r->print( "Rotation = (" + std::to_string(main_program.camera_rotation.x) + ", " + std::to_string(main_program.camera_rotation.y) + ")" );

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
}
