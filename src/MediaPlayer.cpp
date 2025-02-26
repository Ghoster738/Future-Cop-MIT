#include "MediaPlayer.h"

#include "MainMenu.h"
#include "MainProgram.h"
#include "PrimaryGame.h"
#include "Utilities/ImageFormat/Chooser.h"

MediaPlayer MediaPlayer::media_player;

bool MediaPlayer::readMedia( const std::string &path ) {
    Utilities::Buffer image_buffer;

    if(image_buffer.read( path )) {
        Utilities::ImageFormat::Chooser chooser;
        auto the_choosen_r = chooser.getReaderReference( image_buffer );

        if( the_choosen_r != nullptr && this->external_image_p != nullptr &&
            the_choosen_r->read( image_buffer, this->external_image_p->image_2d )) {
                this->external_image_p->upload();
                this->is_image = true;
                return true;
        }
    }

    this->next_picture_count_down = std::chrono::microseconds(0);

    return false;
}

MediaPlayer::MediaPlayer() {
    this->external_image_p = nullptr;
    this->picture_display_time = std::chrono::microseconds(5000000);
}

MediaPlayer::~MediaPlayer() {}

void MediaPlayer::load( MainProgram &main_program ) {
    main_program.loadGraphics( false );
    main_program.loadSound();

    this->media_index = 0;
    this->next_picture_count_down = std::chrono::microseconds(0);

    if(this->external_image_p != nullptr)
        delete this->external_image_p;
    this->external_image_p = main_program.environment_p->allocateExternalImage();

    glm::u32vec2 scale = main_program.getWindowScale();
    this->external_image_p->positions[0] = glm::vec2(0, 0); // Origin
    this->external_image_p->positions[1] =           scale; // End
    this->external_image_p->is_visable = true;

    this->external_image_p->update();

    this->is_image = true;
}

void MediaPlayer::unload( MainProgram &main_program ) {
    if(this->external_image_p != nullptr)
        delete this->external_image_p;
    this->external_image_p = nullptr;
}

void MediaPlayer::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    this->next_picture_count_down -= delta;

    if(this->next_picture_count_down < std::chrono::microseconds(0)) {
        this->next_picture_count_down = this->picture_display_time;

        if(this->media_index == media_list.size()) {
            // Exit out of the media player
            main_program.switchMenu( &MainMenu::main_menu );
            main_program.switchPrimaryGame( &PrimaryGame::primary_game );
            return;
        }

        readMedia( this->media_list.at(this->media_index) );

        this->media_index++;
    }
}
