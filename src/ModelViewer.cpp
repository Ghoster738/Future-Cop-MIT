#include "ModelViewer.h"
#include "MainProgram.h"
#include "MainMenu.h"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "Data/Mission/ACTResource.h"
#include "Data/Mission/TilResource.h"

#include <glm/ext/quaternion_trigonometric.hpp>

ModelViewer ModelViewer::model_viewer;

ModelViewer::ModelViewer() {
    this->displayed_instance_p = nullptr;
}

ModelViewer::~ModelViewer() {}

void ModelViewer::load( MainProgram &main_program ) {
    this->cobj_index = 0;

    this->count_down = 0;
    this->rotation = 0;
    this->exported_textures = false;

    if( !main_program.parameters.export_path.wasModified() )
        this->resource_export_path = "";
    else
        this->resource_export_path = main_program.parameters.export_path.getValue();

    main_program.camera_position = { 0, 0, 0 };
    main_program.camera_rotation = glm::vec2( glm::pi<float>() / 4.0f, glm::pi<float>() / 4.0f );
    main_program.camera_distance = -(this->radius + 4.0f);

    glm::u32vec2 scale = main_program.getWindowScale();
    this->font_height = (1. / 30.) * static_cast<float>( scale.y );

    if( this->displayed_instance_p != nullptr )
        delete this->displayed_instance_p;
    this->displayed_instance_p = nullptr;

    // Do not load from resource if it does not exist.
    if( main_program.resource_r == nullptr )
        return;

    this->obj_vector = main_program.accessor.getAllConstOBJ();

    main_program.loadGraphics( false );
    main_program.loadSound();

    // cobj_index needs to be restricted to the obj_vector size
    if( this->obj_vector.size() <= cobj_index )
        cobj_index = this->obj_vector.size() - 1;

    if( Graphics::ModelInstance::exists( *main_program.environment_p, this->obj_vector.at( cobj_index )->getResourceID() ) ) {
        this->displayed_instance_p = Graphics::ModelInstance::alloc( *main_program.environment_p, obj_vector.at( cobj_index )->getResourceID(), glm::vec3(0,0,0) );
        this->displayed_instance_p->getBoundingSphere( this->position, this->radius );
        this->displayed_instance_p->setPosition( -this->position );
    }

    if( !main_program.text_2d_buffer_r->selectFont( this->font, 0.8 * this->font_height, this->font_height ) ) {
        this->font = 1;

        main_program.text_2d_buffer_r->scaleFont( this->font, this->font_height );

        // Small bitmap font should not be shrunk.
        if( this->font.scale < 1 ) {
            this->font_height = static_cast<float>(this->font_height) / this->font.scale;
            this->font.scale = 1;
        }
    }
}

void ModelViewer::unload( MainProgram &main_program ) {
    if( this->displayed_instance_p != nullptr )
        delete this->displayed_instance_p;
    this->displayed_instance_p = nullptr;

    this->obj_vector.clear();
}

void ModelViewer::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    if( main_program.getMenu() != nullptr )
        return;

    float delta_f = std::chrono::duration<float, std::ratio<1>>( delta ).count();

    if( main_program.control_system_p->isOrderedToExit() )
        main_program.play_loop = false;

    this->count_down -= delta_f;
    this->rotation += delta_f;

    if( !main_program.controllers_r.empty() && main_program.controllers_r[0]->isChanged() )
    {
        auto input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::ACTION );

        if( input_r->isChanged() && input_r->getState() < 0.5 && !resource_export_path.empty() ) {
            // Export the textures from the mission file.
            if(!this->exported_textures) {
                auto bmps = Data::Mission::BMPResource::getVector( *main_program.resource_r );

                for( auto it : bmps ) {
                    auto str = resource_export_path + (*it).getFullName( (*it).getResourceID() );

                    (*it).write( str.c_str() );
                }

                exported_textures = true;
            }

            auto obj = this->obj_vector[ cobj_index ];

            auto str = resource_export_path + obj->getFullName( obj->getResourceID() );

            obj->write( str.c_str() );
        }

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::MENU );
        if( input_r->isChanged() )
        {
            MainMenu::main_menu.is_game_on = true;

            main_program.switchMenu( &MainMenu::main_menu );
        }

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::JUMP );
        if( input_r->isChanged() && input_r->getState() < 0.5 )
        {
            auto act_r = Data::Mission::ACTResource::getVector( *main_program.resource_r );

            auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );

            bool displayed = false;

            for( auto i : act_r ) {
                if( i->hasRSL( Data::Mission::ObjResource::IDENTIFIER_TAG, this->obj_vector[cobj_index]->getResourceID() ) ) {
                    if( !displayed ) {
                        log.output << "Resource ID: " << std::dec << this->obj_vector[cobj_index]->getResourceID() << "\n";
                        log.output << "Type ID: "<< (unsigned)i->getTypeID() << "\n";
                        log.output << "Type ID Name: "<< i->getTypeIDName() << "\n";
                        log.output << "Size: 0x" << std::hex << i->getSize() << "\n";
                        displayed = true;
                    }
                    log.output << "  ID: "<< std::dec << i->getID() << "\n";
                    log.output << "  Offset: 0x" << std::hex << i->getOffset() << "\n";
                    log.output << "  SAC: " << i->getSpawnChunk().getString() << "\n";
                    log.output << "  JSON: " << i->makeJson() << "\n";
                }
            }
        }

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::CHANGE_TARGET );

        if( input_r->isChanged() && input_r->getState() < 0.5 && this->count_down < 0.0f ) {
            main_program.environment_p->setBoundingBoxDraw(!main_program.environment_p->getBoundingBoxDraw());
            this->count_down = 0.5f;
        }

        int next = 0;

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::RIGHT );

        if( input_r->isChanged() && input_r->getState() < 0.5 && this->count_down < 0.0f )
            next++;

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::LEFT );

        if( input_r->isChanged() && input_r->getState() < 0.5 && this->count_down < 0.0f )
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

            if( this->displayed_instance_p != nullptr )
                delete this->displayed_instance_p;

            if( Graphics::ModelInstance::exists( *main_program.environment_p, obj_vector.at( cobj_index )->getResourceID() ) ) {
                this->displayed_instance_p = Graphics::ModelInstance::alloc( *main_program.environment_p, this->obj_vector.at( this->cobj_index )->getResourceID(), glm::vec3( 0, 0, 0 ) );

                auto log = Utilities::logger.getLog( Utilities::Logger::DEBUG );

                log.output << "Sphere result is " << this->displayed_instance_p->getBoundingSphere( this->position, this->radius ) << "\n";
                log.output << "position is (" << this->position.x << ", " << this->position.y << ", " << this->position.z << ")\n";
                log.output << "radius is " << this->radius;

                // first_person->setView3D( placeView( glm::pi<float>() / 4.0f, this->radius + 4.0f, this->position ) );
                this->displayed_instance_p->setPosition( -this->position );
                main_program.camera_distance = -(this->radius + 4.0f);
            }
            else
                this->displayed_instance_p = nullptr;


            this->count_down = 0.5f;
            this->rotation = 0;
        }
    }

    if( this->displayed_instance_p != nullptr )
        this->displayed_instance_p->setRotation( glm::angleAxis( rotation, glm::vec3( 0.0f, 1.0f, 0.0f ) ) );

    const auto text_2d_buffer_r = main_program.text_2d_buffer_r;

    text_2d_buffer_r->setCenterMode( Graphics::Text2DBuffer::CenterMode::LEFT );

    text_2d_buffer_r->setFont( this->font );
    text_2d_buffer_r->setColor( glm::vec4( 1, 1, 1, 1 ) );
    text_2d_buffer_r->setPosition( glm::vec2( 0, 0 ) );
    text_2d_buffer_r->print( "Resource ID = " + std::to_string( this->obj_vector.at( this->cobj_index )->getResourceID() ) );

    if( !this->resource_export_path.empty() ) {
        text_2d_buffer_r->setColor( glm::vec4( 1, 0, 1, 1 ) );
        text_2d_buffer_r->setPosition( glm::vec2( 0, this->font_height ) );
        text_2d_buffer_r->print( "PRESS the " + main_program.controllers_r.at(0)->getInput( Controls::StandardInputSet::ACTION )->getName() + " button to export model." );
    }
}
