#ifndef FC_MEDIA_PLAYER_H
#define FC_MEDIA_PLAYER_H

#include "GameState.h"

#include <filesystem>
#include <vector>

#include "Menu.h"
#include "GameState.h"
#include "Utilities/Video.h"
#include "Sound/Stream.h"
#include "Graphics/ExternalImage.h"

class MediaPlayer : public GameState {
public:
    static MediaPlayer media_player;

    std::vector<std::filesystem::path> media_list;
    Menu      *next_menu_r;
    GameState *next_state_r;

protected:
    Utilities::Video *video_p;

    Graphics::ExternalImage *external_image_p;
    Sound::Stream *audio_stream_p;

    unsigned int media_index;
    std::chrono::microseconds picture_display_time;
    std::chrono::microseconds next_picture_count_down;

    bool is_image;

    bool readMedia( MainProgram &main_program, const std::filesystem::path &path );

    void updateMedia( MainProgram &main_program, const std::filesystem::path &path );

    std::chrono::microseconds button_timer;

public:
    MediaPlayer();
    virtual ~MediaPlayer();

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

#endif // FC_MEDIA_PLAYER_H
