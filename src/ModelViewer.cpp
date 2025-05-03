#include "ModelViewer.h"
#include "MainProgram.h"
#include "MainMenu.h"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "Data/Mission/ACTResource.h"
#include "Data/Mission/TilResource.h"

#include <glm/ext/quaternion_trigonometric.hpp>

namespace {

const std::string LEFT_TEXT[2]  = {"Press LEFT to", "Decrement ID"};
const std::string RIGHT_TEXT[2] = {"Press RIGHT to", "Increment ID"};

}

ModelViewer ModelViewer::model_viewer;

ModelViewer::ModelViewer() {
    this->displayed_instance_p = nullptr;
}

ModelViewer::~ModelViewer() {}

void ModelViewer::load( MainProgram &main_program ) {
    this->count_down = 0;
    this->rotation = 0;
    this->exported_textures = false;

    if( !main_program.parameters.export_dir.wasModified() )
        this->resource_export_path = "";
    else
        this->resource_export_path = main_program.parameters.export_dir.getValue();

    main_program.camera_position = { 0, 0, 0 };
    main_program.camera_rotation = glm::vec2( glm::pi<float>() / 4.0f, glm::pi<float>() / 4.0f );
    main_program.camera_distance = -(this->radius + 4.0f);

    glm::u32vec2 scale = main_program.getWindowScale();
    this->font_height = (1. / 30.) * static_cast<float>( scale.y );

    if( this->displayed_instance_p != nullptr )
        delete this->displayed_instance_p;
    this->displayed_instance_p = nullptr;

    main_program.loadGraphics( false );
    main_program.loadSound();

    if( !main_program.text_2d_buffer_r->selectFont( this->font, 0.8 * this->font_height, this->font_height ) ) {
        this->font = 1;

        main_program.text_2d_buffer_r->scaleFont( this->font, this->font_height );

        // Small bitmap font should not be shrunk.
        if( this->font.scale < 1 ) {
            this->font_height = static_cast<float>(this->font_height) / this->font.scale;
            this->font.scale = 1;
        }
    }

    this->right_text_placement = main_program.getWindowScale();
    this->right_text_placement.y -= 2 * this->font_height;

    this->obj_vector = main_program.accessor.getAllConstOBJ();
    this->cobj_index  = 0;

    this->animation_track_state.animation_track_index = 0;
    this->animation_track_state.current_time = std::chrono::microseconds(0);
    this->animation_track_state.obj_r = nullptr;

    // Do not load from resource if it does not exist.
    if( this->obj_vector.empty() )
        return;

    // cobj_index needs to be restricted to the obj_vector size
    if( this->obj_vector.size() <= cobj_index )
        cobj_index = this->obj_vector.size() - 1;

    if( main_program.environment_p->doesModelExist( this->obj_vector.at( cobj_index )->getResourceID() ) ) {
        this->animation_track_state.obj_r = this->obj_vector.at( cobj_index );

        this->displayed_instance_p = main_program.environment_p->allocateModel( this->animation_track_state.obj_r->getResourceID() );
        this->displayed_instance_p->getBoundingSphere( this->position, this->radius );
        this->displayed_instance_p->setPosition( -this->position );
        this->displayed_instance_p->setVisable(true);
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
                auto bmps = main_program.accessor.getAllConstBMP();

                for( auto it : bmps ) {
                    auto str = resource_export_path;
                    str += (*it).getFullName( (*it).getResourceID() );

                    (*it).write( str );
                }

                exported_textures = true;
            }

            auto obj = this->obj_vector[ cobj_index ];

            auto str = resource_export_path;
            str += obj->getFullName( obj->getResourceID() );

            obj->write( str );
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
            auto act_r = main_program.accessor.getActorAccessor().getAllConst();

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
            this->count_down = 0.25f;
        }

        int next_track = 0;

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::UP );

        if( input_r->isChanged() && input_r->getState() > 0.5 && this->count_down < 0.0f )
            next_track++;

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::DOWN );

        if( input_r->isChanged() && input_r->getState() > 0.5 && this->count_down < 0.0f )
            next_track--;

        if( next_track != 0 && !this->obj_vector.empty() && !this->animation_track_state.obj_r->getAnimationTracks().empty() ) {
            if( next_track > 0 )
            {
                this->animation_track_state.animation_track_index += next_track;

                if( this->animation_track_state.animation_track_index >= this->animation_track_state.obj_r->getAnimationTracks().size() )
                    this->animation_track_state.animation_track_index = 0;
            }
            else
            {
                if( this->animation_track_state.animation_track_index == 0 )
                    this->animation_track_state.animation_track_index = this->animation_track_state.obj_r->getAnimationTracks().size() - 1;
                else
                    this->animation_track_state.animation_track_index += next_track;
            }

            this->animation_track_state.current_time = std::chrono::microseconds(0);
            if(!this->animation_track_state.obj_r->getAnimationTracks().empty())
                this->animation_track_state.animation_track_index = this->animation_track_state.animation_track_index;
        }

        int next_cobj = 0;

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::RIGHT );

        if( input_r->isChanged() && input_r->getState() > 0.5 && this->count_down < 0.0f )
            next_cobj++;

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::LEFT );

        if( input_r->isChanged() && input_r->getState() > 0.5 && this->count_down < 0.0f )
            next_cobj--;

        if( next_cobj != 0 && !this->obj_vector.empty() ) {
            if( next_cobj > 0 )
            {
                cobj_index += next_cobj;

                if( cobj_index >= this->obj_vector.size() )
                    cobj_index = 0;
            }
            else
            {
                if( cobj_index == 0 )
                    cobj_index = this->obj_vector.size() - 1;
                else
                    cobj_index += next_cobj;
            }

            if( this->displayed_instance_p != nullptr )
                delete this->displayed_instance_p;
            this->displayed_instance_p = nullptr;

            this->animation_track_state.animation_track_index = 0;
            this->animation_track_state.current_time = std::chrono::microseconds(0);
            this->animation_track_state.obj_r = this->obj_vector.at( cobj_index );

            if( main_program.environment_p->doesModelExist( this->animation_track_state.obj_r->getResourceID() ) ) {
                this->displayed_instance_p = main_program.environment_p->allocateModel( this->animation_track_state.obj_r->getResourceID() );

                auto log = Utilities::logger.getLog( Utilities::Logger::DEBUG );

                log.output << "Sphere result is " << this->displayed_instance_p->getBoundingSphere( this->position, this->radius ) << "\n";
                log.output << "position is (" << this->position.x << ", " << this->position.y << ", " << this->position.z << ")\n";
                log.output << "radius is " << this->radius;

                // first_person->setView3D( placeView( glm::pi<float>() / 4.0f, this->radius + 4.0f, this->position ) );
                this->displayed_instance_p->setPosition( -this->position );
                this->displayed_instance_p->setVisable( true );
                main_program.camera_distance = -(this->radius + 4.0f);
            }

            this->count_down = 0.25f;
            this->rotation = 0;
        }
    }

    if( this->displayed_instance_p != nullptr ) {
        this->animation_track_state.advance( delta );

        this->displayed_instance_p->setPositionTransformTimeline( this->animation_track_state.getCurrentFrame() );
        this->displayed_instance_p->setRotation( glm::angleAxis( rotation, glm::vec3( 0.0f, 1.0f, 0.0f ) ) );
    }

    const auto text_2d_buffer_r = main_program.text_2d_buffer_r;

    text_2d_buffer_r->setCenterMode( Graphics::Text2DBuffer::CenterMode::LEFT );
    text_2d_buffer_r->setFont( this->font );
    text_2d_buffer_r->setColor( glm::vec4( 1, 1, 1, 1 ) );
    text_2d_buffer_r->setPosition( glm::vec2( 0, 0 ) );

    if(this->obj_vector.empty())
        text_2d_buffer_r->print( "No models exist in any resources that are loaded!" );
    else
        text_2d_buffer_r->print(
            "Resource ID (" + std::to_string( this->cobj_index ) + ", " + std::to_string( this->obj_vector.size() ) + "): " +
            std::to_string( this->obj_vector.at( this->cobj_index )->getResourceID() ) );

    text_2d_buffer_r->setPosition( glm::vec2( 0, right_text_placement.y ) );
    text_2d_buffer_r->print( LEFT_TEXT[0] );
    text_2d_buffer_r->setPosition( glm::vec2( 0, right_text_placement.y + this->font_height ) );
    text_2d_buffer_r->print( LEFT_TEXT[1] );

    text_2d_buffer_r->setCenterMode( Graphics::Text2DBuffer::CenterMode::RIGHT );

    if(!this->obj_vector.empty() && !this->obj_vector.at( this->cobj_index )->getAnimationTracks().empty()) {
        text_2d_buffer_r->setPosition( glm::vec2( right_text_placement.x, 0 ) );

        if(!this->obj_vector.at( this->cobj_index )->isAnimationTrackGenerated())
            text_2d_buffer_r->print( "Animation Track "
                + std::to_string(this->animation_track_state.animation_track_index) + "/" + std::to_string(this->obj_vector.at( this->cobj_index )->getAnimationTracks().size()) );
        else
            text_2d_buffer_r->print( "Animation Track Not Present" );
    }

    text_2d_buffer_r->setPosition( glm::vec2( right_text_placement ) );
    text_2d_buffer_r->print( RIGHT_TEXT[0] );
    text_2d_buffer_r->setPosition( glm::vec2( right_text_placement + glm::u32vec2(0, this->font_height ) ) );
    text_2d_buffer_r->print( RIGHT_TEXT[1] );

    if( !this->resource_export_path.empty() ) {
        text_2d_buffer_r->setCenterMode( Graphics::Text2DBuffer::CenterMode::LEFT );
        text_2d_buffer_r->setColor( glm::vec4( 1, 0, 1, 1 ) );
        text_2d_buffer_r->setPosition( glm::vec2( 0, this->font_height ) );
        text_2d_buffer_r->print( "PRESS the " + main_program.controllers_r.at(0)->getInput( Controls::StandardInputSet::ACTION )->getName() + " button to export model." );
    }
}
