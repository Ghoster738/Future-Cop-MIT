#include "MediaPlayer.h"

#include "MainMenu.h"
#include "MainProgram.h"
#include "PrimaryGame.h"
#include "Utilities/ImageFormat/Chooser.h"

#define PL_MPEG_IMPLEMENTATION
#include <pl_mpeg.h>

// TODO Remove this in favor of a simpiler video format.
namespace {
    plm_t *pl_video_p = nullptr;
}

MediaPlayer MediaPlayer::media_player;

bool MediaPlayer::readMedia( const std::string &path ) {
    if(pl_video_p != nullptr)
        plm_destroy(pl_video_p);

    pl_video_p = plm_create_with_filename(path.c_str());

    if(pl_video_p != nullptr) {
        plm_set_audio_enabled(pl_video_p, 0);

        this->is_image = false;

        int width  = plm_get_width( pl_video_p);
        int height = plm_get_height(pl_video_p);

        this->external_image_p->image_2d.setDimensions( width, height );

        return true;
    }

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
    if(this->is_image) {
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
    else {
        plm_frame_t *frame_p = plm_decode_video(pl_video_p);

        if(frame_p == nullptr) {
            if(this->media_index == media_list.size()) {
                // Exit out of the media player
                main_program.switchMenu( &MainMenu::main_menu );
                main_program.switchPrimaryGame( &PrimaryGame::primary_game );
                return;
            }

            readMedia( this->media_list.at(this->media_index) );
            this->media_index++;
        }
        else {
            plm_frame_to_rgb(frame_p, this->external_image_p->image_2d.getDirectGridData(), 3 * this->external_image_p->image_2d.getWidth());
            this->external_image_p->upload();
        }
    }
}
