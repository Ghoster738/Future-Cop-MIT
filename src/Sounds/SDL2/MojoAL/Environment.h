#ifndef SOUNDS_ENVIRONMENT_MOJO_AL_H
#define SOUNDS_ENVIRONMENT_MOJO_AL_H

#include "../../Environment.h"

#include <map>

#include "al.h"
#include "alc.h"

namespace Sounds {
namespace SDL2 {
namespace MojoAL {

class Environment : public Sounds::Environment {
public:
    ALCdevice *alc_device_p;
    ALCcontext *alc_context_p;

    std::map<uint32_t, ALuint> tos_to_swvr;

    ALuint music_buffer;
    ALuint music_source;

    static ALenum getFormat(unsigned int number_of_channels, unsigned int bits_per_sample);
public:
    Environment();
    virtual ~Environment();

    static int initSystem();
    static int deinitEntireSystem();

    virtual std::string getEnvironmentIdentifier() const;
    virtual int loadResources( const Data::Accessor &accessor );

    virtual bool setMusicState(Sounds::PlayerState player_state);
    virtual PlayerState getMusicState() const;

    virtual bool queueTrack(uint32_t track_offset);
    virtual void clearTrackQueue();
    virtual bool setTrackPlayerState(PlayerState player_state);
    virtual PlayerState getTrackPlayerState() const;

    virtual void advanceTime( std::chrono::high_resolution_clock::duration duration );
};

}
}
}

#endif // SOUNDS_ENVIRONMENT_MOJO_AL_H
