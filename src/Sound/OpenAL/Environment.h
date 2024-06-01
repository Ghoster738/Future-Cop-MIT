#ifndef SOUNDS_ENVIRONMENT_MOJO_AL_H
#define SOUNDS_ENVIRONMENT_MOJO_AL_H

#include "../Environment.h"

#include "Internal/SoundQueue.h"

#include <map>

#include "al.h"
#include "alc.h"

namespace Sound {
namespace OpenAL {

class Environment : public Sound::Environment {
public:
    ALCdevice *alc_device_p;
    ALCcontext *alc_context_p;

    std::map<uint32_t, Internal::SoundBuffer> tos_to_swvr;

    Internal::SoundQueue sound_queue;

    Internal::SoundBuffer music_buffer;
    ALuint music_source;
public:
    Environment();
    virtual ~Environment();

    static int initSystem();
    static int deinitEntireSystem();

    virtual std::string getEnvironmentIdentifier() const;
    virtual int loadResources( const Data::Accessor &accessor );

    virtual bool setMusicState(Sound::PlayerState player_state);
    virtual PlayerState getMusicState() const;

    virtual bool queueTrack(uint32_t track_offset);
    virtual bool setTrackPlayerState(PlayerState player_state);
    virtual PlayerState getTrackPlayerState() const;

    virtual void advanceTime( std::chrono::high_resolution_clock::duration duration );
};

}
}

#endif // SOUNDS_ENVIRONMENT_MOJO_AL_H
