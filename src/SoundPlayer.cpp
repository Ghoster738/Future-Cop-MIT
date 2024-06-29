#include "SoundPlayer.h"
#include "MainProgram.h"
#include "MainMenu.h"

#include <glm/vec2.hpp>

SoundPlayer SoundPlayer::sound_player;

SoundPlayer::SoundPlayer() {
    sound_p = nullptr;
}

SoundPlayer::~SoundPlayer() {}

void SoundPlayer::load( MainProgram &main_program ) {
    this->repeat = false;
    this->sound_resource_index = 0;
    this->all_sounds.clear();

    this->all_sounds = main_program.accessor.getAllConstWAV();

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

    main_program.sound_system_p->getListenerReference(Sound::Listener::WhichEar::BOTH)->setEnabled(true);
}

void SoundPlayer::unload( MainProgram &main_program ) {
    this->all_sounds.clear();

    if(this->sound_p != nullptr)
        delete this->sound_p;
    this->sound_p = nullptr;
}

void SoundPlayer::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    if( main_program.getMenu() != nullptr )
        return;

    float delta_f = std::chrono::duration<float, std::ratio<1>>( delta ).count();

    if( main_program.control_system_p->isOrderedToExit() )
        main_program.play_loop = false;

    this->count_down -= delta_f;

    if( !main_program.controllers_r.empty() && main_program.controllers_r[0]->isChanged() )
    {
        auto input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::ACTION );
        if( input_r->isChanged() && this->count_down < 0.0f && !this->all_sounds.empty() ) {
            if(this->sound_p != nullptr)
                delete this->sound_p;

            this->sound_p = main_program.sound_system_p->allocateSpeaker( this->all_sounds.at(this->sound_resource_index)->getResourceID() );

            if(this->sound_p != nullptr) {
                this->sound_p->setRepeatMode(this->repeat);
                this->sound_p->setSpeakerState(Sound::PlayerState::PLAY);
            }

            this->count_down = 0.5;
        }
        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::JUMP );
        if( input_r->isChanged() && this->count_down < 0.0f ) {
            this->repeat = !this->repeat;

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
                this->sound_resource_index += next;

                if( this->sound_resource_index >= this->all_sounds.size() )
                    this->sound_resource_index = 0;
            }
            else
            {
                if( this->sound_resource_index == 0 )
                    this->sound_resource_index = this->all_sounds.size() - 1;
                else
                    this->sound_resource_index += next;
            }

            this->count_down = 0.5f;
        }
    }

    const auto text_2d_buffer_r = main_program.text_2d_buffer_r;

    text_2d_buffer_r->setCenterMode( Graphics::Text2DBuffer::CenterMode::LEFT );

    text_2d_buffer_r->setFont( this->font );
    text_2d_buffer_r->setColor( glm::vec4( 1, 1, 1, 1 ) );
    text_2d_buffer_r->setPosition( glm::vec2( 0, 0 ) );

    if(!this->all_sounds.empty()) {
        std::stringstream new_stream;

        new_stream << "Sound Resource id = " << this->all_sounds.at( this->sound_resource_index )->getResourceID();

        text_2d_buffer_r->print( new_stream.str() );
    }
    else
        text_2d_buffer_r->print( "No sounds are loaded." );

    text_2d_buffer_r->setColor( glm::vec4( 1, 0, 1, 1 ) );
    text_2d_buffer_r->setPosition( glm::vec2( 0, this->font_height ) );

    if(!this->all_sounds.empty() && this->all_sounds.at( this->sound_resource_index )->hasLoop()) {
        std::stringstream new_stream;

        new_stream << "Begin loop offset = " << this->all_sounds.at( this->sound_resource_index )->getLoopBeginSample() << "; ";
        new_stream << "End loop offset = " << this->all_sounds.at( this->sound_resource_index )->getLoopEndSample();

        text_2d_buffer_r->print( new_stream.str() );
    }
    else
        text_2d_buffer_r->print( "Note PS1 sounds are not supported yet." );
}
