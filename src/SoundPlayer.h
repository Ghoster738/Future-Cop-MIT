#ifndef FC_SOUND_PLAYER_H
#define FC_SOUND_PLAYER_H

#include "GameState.h"

#include <vector>
#include "Data/Mission/WAVResource.h"
#include "Graphics/Text2DBuffer.h"

#include "Sound/Speaker.h"

class SoundPlayer : public GameState {
public:
    static SoundPlayer sound_player;

private:
    bool repeat;
    unsigned int sound_resource_index;
    std::vector<const Data::Mission::WAVResource*> all_sounds;
    Sound::Speaker *sound_p;

    float count_down;

    Graphics::Text2DBuffer::Font font;
    unsigned font_height;

public:
    SoundPlayer();
    virtual ~SoundPlayer();

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

#endif // FC_SOUND_PLAYER_H
