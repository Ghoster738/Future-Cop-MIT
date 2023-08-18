#include "PrimaryGame.h"
#include "MainProgram.h"

#include "Utilities/ImageFormat/Chooser.h"

PrimaryGame::PrimaryGame() {
}

PrimaryGame::~PrimaryGame() {
}

void PrimaryGame::load() {
    is_camera_moving = false;
    camera_position_transform = { 0, 0, 0 };
    camera_rotation_transform = { 0, 0 };
    camera_distance_transform = 0;

    current_tile_selected = -1;
    til_polygon_type_selected = 111;
}

void PrimaryGame::unload() {
}

void PrimaryGame::grabControls( MainProgram &main_program, std::chrono::microseconds delta ) {
    float delta_f = std::chrono::duration<float, std::ratio<1>>( delta ).count();

    if( main_program.control_system_p->isOrderedToExit() )
        main_program.play_loop = false;

    if( main_program.controllers_r[0]->isChanged() )
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
            const auto dimensions = main_program.environment_p->window_p->getDimensions();

            Utilities::Image2D image_screenshot( dimensions.x, dimensions.y, Utilities::PixelFormatColor_R8G8B8A8() );

            if( main_program.environment_p->screenshot( image_screenshot ) ) {
                Utilities::Buffer file;
                Utilities::ImageFormat::Chooser chooser;
                auto the_choosen_r = chooser.getWriterReference( image_screenshot );
                if( the_choosen_r != nullptr ) {
                    the_choosen_r->write( image_screenshot, file);
                    file.write( the_choosen_r->appendExtension( "screenshot" ) );
                }
            }
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
}

void PrimaryGame::display( MainProgram &main_program ) {
}
