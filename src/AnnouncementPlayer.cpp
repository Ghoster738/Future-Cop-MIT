#include "AnnouncementPlayer.h"
#include "MainProgram.h"
#include "MainMenu.h"

#include <glm/vec2.hpp>
#include "Data/Mission/TOSResource.h"

AnnouncementPlayer AnnouncementPlayer::announcement_player;

AnnouncementPlayer::AnnouncementPlayer() {
    this->anm_p = nullptr;
}

AnnouncementPlayer::~AnnouncementPlayer() {}

void AnnouncementPlayer::load( MainProgram &main_program ) {
    this->announcement_index = 0;
    this->announcements.clear();

    main_program.loadGraphics( false );
    main_program.loadSound();

    glm::u32vec2 scale = main_program.getWindowScale();
    this->font_height = (1. / 30.) * static_cast<float>( scale.y );

    if( !main_program.text_2d_buffer_r->selectFont( this->font, 0.8 * this->font_height, this->font_height ) ) {
        this->font = 1;

        main_program.text_2d_buffer_r->scaleFont( this->font, this->font_height );

        // Small bitmap font should not be shrunk.
        if( this->font.scale < 1 ) {
            this->font_height = static_cast<float>(this->font_height) / this->font.scale;
            this->font.scale = 1;
        }
    }

    auto tos_resource_r = main_program.accessor.getConstTOS( 1 );

    if(tos_resource_r == nullptr)
        return;

    for( const uint32_t tos_offset: tos_resource_r->getOffsets()) {
        const Data::Accessor *swvr_accessor_r = main_program.accessor.getSWVRAccessor(tos_offset);

        // assert(swvr_accessor_r != nullptr);

        if(swvr_accessor_r != nullptr) {
            auto snds_array_r = swvr_accessor_r->getAllConstSNDS();

            if(!snds_array_r.empty())
                this->announcements.push_back(snds_array_r[0]);
        }
    }

    if(this->anm_p != nullptr)
        delete this->anm_p;
    this->anm_p = nullptr;
}

void AnnouncementPlayer::unload( MainProgram &main_program ) {
    this->announcements.clear();

    if(this->anm_p != nullptr)
        delete this->anm_p;
    this->anm_p = nullptr;
}

void AnnouncementPlayer::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    if( main_program.getMenu() != nullptr )
        return;

    float delta_f = std::chrono::duration<float, std::ratio<1>>( delta ).count();

    if( main_program.control_system_p->isOrderedToExit() )
        main_program.play_loop = false;

    this->count_down -= delta_f;

    if( !main_program.controllers_r.empty() && main_program.controllers_r[0]->isChanged() )
    {
        auto input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::ACTION );
        if( input_r->isChanged() && this->count_down < 0.0f && !this->announcements.empty() ) {
            main_program.sound_system_p->setTrackPlayerState(Sound::PlayerState::PLAY);

            const auto tos_offset = this->announcements.at(this->announcement_index)->getSWVREntry().tos_offset;

            main_program.sound_system_p->queueTrack(tos_offset);

            if(this->anm_p != nullptr)
                delete this->anm_p;
            this->anm_p = main_program.environment_p->allocateVideoANM(tos_offset);

            if(this->anm_p != nullptr) {
                this->anm_p->positions[0] = glm::vec2(   0.0, -400.0);
                this->anm_p->positions[1] = glm::vec2( 400.0,  000.0);
                this->anm_p->is_visable = true;

                this->anm_p->upload();
                this->anm_p->update();
            }

            this->count_down = 0.5;
        }

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::MENU );
        if( input_r->isChanged() )
        {
            MainMenu::main_menu.is_game_on = true;

            main_program.switchMenu( &MainMenu::main_menu );
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
                this->announcement_index += next;

                if( this->announcement_index >= this->announcements.size() )
                    this->announcement_index = 0;
            }
            else
            {
                if( this->announcement_index == 0 )
                    this->announcement_index = this->announcements.size() - 1;
                else
                    this->announcement_index += next;
            }

            this->count_down = 0.5f;
        }
    }

    const auto text_2d_buffer_r = main_program.text_2d_buffer_r;

    text_2d_buffer_r->setCenterMode( Graphics::Text2DBuffer::CenterMode::LEFT );

    text_2d_buffer_r->setFont( this->font );
    text_2d_buffer_r->setColor( glm::vec4( 1, 1, 1, 1 ) );
    text_2d_buffer_r->setPosition( glm::vec2( 0, 0 ) );

    if(!this->announcements.empty())
        text_2d_buffer_r->print( "Subfile name = " + this->announcements.at( this->announcement_index )->getSWVREntry().name );
    else
        text_2d_buffer_r->print( "No Announcements are loaded." );

    text_2d_buffer_r->setColor( glm::vec4( 1, 0, 1, 1 ) );
    text_2d_buffer_r->setPosition( glm::vec2( 0, this->font_height ) );

    if(!this->announcements.empty()) {
        std::stringstream new_stream;

        new_stream << "SWVR tos offset = 0x" << std::hex << this->announcements.at( this->announcement_index )->getSWVREntry().tos_offset;

        text_2d_buffer_r->print( new_stream.str() );
    }
    else
        text_2d_buffer_r->print( "Note PS1 file sounds are not supported yet." );

    if(!this->announcements.empty()) {
        text_2d_buffer_r->setColor( glm::vec4( 0, 1, 0, 1 ) );
        text_2d_buffer_r->setPosition( glm::vec2( 0, 2 * this->font_height ) );

        std::stringstream new_stream;

        new_stream << "SWVR offset = 0x" << std::hex << this->announcements.at( this->announcement_index )->getSWVREntry().offset;

        text_2d_buffer_r->print( new_stream.str() );
    }
}
