#include "MediaPlayer.h"

#include "MainMenu.h"
#include "MainProgram.h"
#include "PrimaryGame.h"
#include "Utilities/ImageFormat/Chooser.h"

namespace {
void decode_audio(float *data_r, unsigned channels, unsigned samples_per_channel, void *user_r) {
    auto *audio_stream_r = reinterpret_cast<Sound::Stream*>(user_r);

    audio_stream_r->appendSamples(data_r, channels, samples_per_channel);

    if(audio_stream_r->getSpeakerState() != Sound::PlayerState::PLAY)
        audio_stream_r->setSpeakerState(Sound::PlayerState::PLAY);
}
}

MediaPlayer MediaPlayer::media_player;

bool MediaPlayer::readMedia( MainProgram &main_program, const std::filesystem::path &path ) {
    this->video_p = Utilities::Video::allocateVideo(path);

    if(this->video_p != nullptr) {
        this->is_image = false;

        this->video_p->setupImage2D(this->external_image_p->image_2d);

        if(this->video_p->hasAudio()) {
            if(this->audio_stream_p != nullptr) {
                const auto num_channels        = this->audio_stream_p->getNumOfChannels();
                const auto samples_per_channel = this->audio_stream_p->getSamplesPerChannel();
                const auto frequency           = this->audio_stream_p->getFrequency();

                if(num_channels != this->video_p->getNumOfAudioChannels() || samples_per_channel != this->video_p->getAudioSamplesPerChannel() || frequency != this->video_p->getAudioFrequency()) {
                    delete this->audio_stream_p;
                    this->audio_stream_p = nullptr;
                }
            }

            if(this->audio_stream_p == nullptr)
                this->audio_stream_p = main_program.sound_system_p->allocateStream(this->video_p->getNumOfAudioChannels(), this->video_p->getAudioSamplesPerChannel(), this->video_p->getAudioFrequency());

            if(this->audio_stream_p != nullptr)
                this->video_p->setAudioCallback(decode_audio, this->audio_stream_p);
        }

        return true;
    }

    Utilities::Buffer image_buffer;

    if(image_buffer.read( path.string() )) {
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

void MediaPlayer::updateMedia( MainProgram &main_program, const std::filesystem::path &path ) {
    if(this->external_image_p == nullptr) {
        this->next_picture_count_down = std::chrono::microseconds(0);
        return; // There is nothing to render to.
    }

    bool successful_read = readMedia( main_program, path );

    if(successful_read) {
        if(this->external_image_p->image_2d.getWidth() == 0 || this->external_image_p->image_2d.getHeight() == 0) {
            return; // Do nothing.
        }
        const double i_aspect_ratio = static_cast<double>(this->external_image_p->image_2d.getWidth()) / static_cast<double>(this->external_image_p->image_2d.getHeight());
        const double adjusted_width  = main_program.getWindowScale().y * i_aspect_ratio / 1.0;
        const double adjusted_height = main_program.getWindowScale().x * 1.0 / i_aspect_ratio;

        if(adjusted_width <= main_program.getWindowScale().x) {
            this->external_image_p->positions[0] = glm::vec2(0, 0);
            this->external_image_p->positions[1].x = adjusted_width;
            this->external_image_p->positions[1].y = main_program.getWindowScale().y;

            const double center = 0.5 * (main_program.getWindowScale().x - this->external_image_p->positions[1].x);
            this->external_image_p->positions[0].x += center;
            this->external_image_p->positions[1].x += center;

            this->external_image_p->update();
        }
        else {
            this->external_image_p->positions[0] = glm::vec2(0, 0);
            this->external_image_p->positions[1].x = main_program.getWindowScale().x;
            this->external_image_p->positions[1].y = adjusted_height;

            const double center = 0.5 * (main_program.getWindowScale().y - this->external_image_p->positions[1].y);
            this->external_image_p->positions[0].y += center;
            this->external_image_p->positions[1].y += center;

            this->external_image_p->update();
        }
    }
}

MediaPlayer::MediaPlayer() {
    this->next_menu_r  = &MainMenu::main_menu;
    this->next_state_r = &PrimaryGame::primary_game;
    this->external_image_p = nullptr;
    this->audio_stream_p   = nullptr;
    this->picture_display_time = std::chrono::microseconds(5000000);
}

MediaPlayer::~MediaPlayer() {}

void MediaPlayer::clearMediaPaths() {
    this->media_list.clear();
    this->next_media_list.clear();
}

void MediaPlayer::appendMediaPaths(std::vector<std::filesystem::path> &media_paths) {
    this->media_list.insert( this->media_list.end(), media_paths.begin(), media_paths.end() );
}

void MediaPlayer::appendNextMediaPaths(std::vector<std::filesystem::path> &media_paths) {
    this->next_media_list.insert( this->next_media_list.end(), media_paths.begin(), media_paths.end() );
}

void MediaPlayer::load( MainProgram &main_program ) {
    main_program.loadGraphics( false );
    main_program.loadSound();

    this->media_index = 0;
    this->next_picture_count_down = std::chrono::microseconds(0);

    this->is_image = true;

    this->button_timer = std::chrono::microseconds(0);

    if(this->external_image_p != nullptr)
        delete this->external_image_p;
    this->external_image_p = main_program.environment_p->allocateExternalImage();

    if(this->external_image_p == nullptr)
        return;

    this->external_image_p->positions[0] =               glm::vec2(0, 0); // Origin
    this->external_image_p->positions[1] = main_program.getWindowScale(); // End
    this->external_image_p->is_visable = true;

    this->external_image_p->update();
}

void MediaPlayer::unload( MainProgram &main_program ) {
    if(this->external_image_p != nullptr)
        delete this->external_image_p;
    this->external_image_p = nullptr;

    if(this->audio_stream_p != nullptr)
        delete this->audio_stream_p;
    this->audio_stream_p = nullptr;

    if(this->video_p != nullptr)
        delete this->video_p;
    this->video_p = nullptr;
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
            this->media_list = this->next_media_list;
            this->next_media_list.clear();
            main_program.switchMenu( this->next_menu_r );
            main_program.switchPrimaryGame( this->next_state_r );
            if(this->next_menu_r == nullptr && this->next_menu_r == this->next_state_r)
                main_program.play_loop = false;
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
                this->media_list = this->next_media_list;
                this->next_media_list.clear();
                main_program.switchMenu( this->next_menu_r );
                main_program.switchPrimaryGame( this->next_state_r );
                if(this->next_menu_r == nullptr && this->next_menu_r == this->next_state_r)
                    main_program.play_loop = false;
                return;
            }

            updateMedia(main_program, this->media_list.at(this->media_index));
            this->media_index++;
        }
    }
    else {
        if(this->video_p->update(delta))
            this->external_image_p->upload();

        if(this->video_p->hasEnded() || end_video) {
            delete this->video_p;
            this->video_p = nullptr;

            if(this->media_index == media_list.size()) {
                // Exit out of the media player
                this->media_list = this->next_media_list;
                this->next_media_list.clear();
                main_program.switchMenu( this->next_menu_r );
                main_program.switchPrimaryGame( this->next_state_r );
                if(this->next_menu_r == nullptr && this->next_menu_r == this->next_state_r)
                    main_program.play_loop = false;
                return;
            }

            updateMedia(main_program, this->media_list.at(this->media_index));
            this->media_index++;
        }
    }
}
