#ifndef SOUND_OPENAL_INTERNAL_SOUND_QUEUE
#define SOUND_OPENAL_INTERNAL_SOUND_QUEUE

#include <chrono>
#include <queue>
#include "al.h"

#include "SoundBuffer.h"

#include "../../PlayerState.h"

namespace Sound {
namespace OpenAL {
namespace Internal {

class SoundQueue {
protected:
    ALfloat p_gain;

public:
    unsigned queue_limit;

protected:

    std::queue<SoundBuffer> sound_queue;

    SoundBuffer current_sound_element;
    Sound::PlayerState player_state;

    bool allocated_queue_source;
public:
    ALuint queue_source;
    const ALfloat& gain = p_gain;

    SoundQueue(ALfloat gain, unsigned queue_limit = 32);

    ALenum initialize();

    ALenum reset();

    void push(SoundBuffer sound_buffer);

    void setPlayerState(Sound::PlayerState player_state);
    Sound::PlayerState getPlayerState() const { return player_state; }

    void update(std::chrono::high_resolution_clock::duration duration);

    ALenum setGain(ALfloat gain);
};

}
}
}

#endif // SOUND_OPENAL_INTERNAL_SOUND_QUEUE
