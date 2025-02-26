#include "MediaPlayer.h"

#include "MainMenu.h"
#include "MainProgram.h"
#include "PrimaryGame.h"
#include "Utilities/ImageFormat/Chooser.h"

#define PL_MPEG_IMPLEMENTATION
#include <pl_mpeg.h>

// TODO Remove this in favor of abstraction.
namespace {
    plm_t *pl_video_p = nullptr;

    void decode_video(plm_t *self_r, plm_frame_t *frame_r, void *user_r) {
        MediaPlayer *media_player_r = reinterpret_cast<MediaPlayer*>(user_r);

        plm_frame_to_rgb(frame_r, media_player_r->external_image_p->image_2d.getDirectGridData(), 3 * media_player_r->external_image_p->image_2d.getWidth());
        media_player_r->external_image_p->upload();
    }
}

MediaPlayer MediaPlayer::media_player;

bool MediaPlayer::readMedia( const std::string &path ) {
    if(pl_video_p != nullptr)
        plm_destroy(pl_video_p);
    pl_video_p = nullptr;

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

    pl_video_p = plm_create_with_filename(path.c_str());

    if(pl_video_p != nullptr) {
        plm_set_audio_enabled(pl_video_p, 0);

        this->is_image = false;

        int width  = plm_get_width( pl_video_p);
        int height = plm_get_height(pl_video_p);

        plm_set_video_decode_callback(pl_video_p, decode_video, this);

        this->external_image_p->image_2d.setDimensions( width, height );

        return true;
    }
    return false;
}

void MediaPlayer::updateMedia( MainProgram &main_program, const std::string &path ) {
    bool successful_read = readMedia( path );

    if(successful_read) {
        if(this->external_image_p->image_2d.getWidth() == 0 || this->external_image_p->image_2d.getHeight() == 0) {
            return; // Do nothing.
        }
        double w_aspect_ratio = static_cast<double>(main_program.getWindowScale().x) / static_cast<double>(main_program.getWindowScale().y);
        double i_aspect_ratio = static_cast<double>(this->external_image_p->image_2d.getWidth()) / static_cast<double>(this->external_image_p->image_2d.getHeight());

        if(w_aspect_ratio > i_aspect_ratio) {
            this->external_image_p->positions[0] = glm::vec2(0, 0);
            this->external_image_p->positions[1].x = main_program.getWindowScale().y * 2.0 / i_aspect_ratio;
            this->external_image_p->positions[1].y = main_program.getWindowScale().y;
            this->external_image_p->update();
        }
        else {
            this->external_image_p->positions[0] = glm::vec2(0, 0);
            this->external_image_p->positions[1].x = main_program.getWindowScale().x;
            this->external_image_p->positions[1].y = main_program.getWindowScale().x * i_aspect_ratio / 2.0;
            this->external_image_p->update();
        }
    }
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

    button_timer = std::chrono::microseconds(0);
}

void MediaPlayer::unload( MainProgram &main_program ) {
    if(this->external_image_p != nullptr)
        delete this->external_image_p;
    this->external_image_p = nullptr;

    if(pl_video_p != nullptr)
        plm_destroy(pl_video_p);
    pl_video_p = nullptr;
}

void MediaPlayer::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    if( main_program.control_system_p->isOrderedToExit() )
        main_program.play_loop = false;

    button_timer -= delta;
    if(button_timer < std::chrono::microseconds(0)) {
        button_timer = std::chrono::microseconds(0);
    }

    bool end_video = false;

    if( !main_program.controllers_r.empty() && main_program.controllers_r[0]->isChanged() && button_timer == std::chrono::microseconds(0)) {
        if(this->media_index == media_list.size()) {
            // Exit out of the media player
            main_program.switchMenu( &MainMenu::main_menu );
            main_program.switchPrimaryGame( &PrimaryGame::primary_game );
            return;
        }

        updateMedia(main_program, this->media_list.at(this->media_index));
        this->media_index++;

        button_timer = std::chrono::microseconds(500000);

        return;
    }

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

            updateMedia(main_program, this->media_list.at(this->media_index));
            this->media_index++;
        }
    }
    else {
        plm_decode(pl_video_p, std::chrono::duration<float, std::ratio<1>>( delta ).count() );

        if(plm_has_ended(pl_video_p) || end_video) {
            if(this->media_index == media_list.size()) {
                // Exit out of the media player
                main_program.switchMenu( &MainMenu::main_menu );
                main_program.switchPrimaryGame( &PrimaryGame::primary_game );
                return;
            }

            updateMedia(main_program, this->media_list.at(this->media_index));
            this->media_index++;
        }
    }
}
