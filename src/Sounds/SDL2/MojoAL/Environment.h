#ifndef SOUNDS_ENVIRONMENT_MOJO_AL_H
#define SOUNDS_ENVIRONMENT_MOJO_AL_H

#include "../../Environment.h"

#include "al.h"
#include "alc.h"

namespace Sounds {
namespace SDL2 {
namespace MojoAL {

class Environment : public Sounds::Environment {
public:
    ALCdevice *alc_device_p;
    ALCcontext *alc_context_p;

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
    virtual bool setMusic(Sounds::PlayerState player_state);
};

}
}
}

#endif // SOUNDS_ENVIRONMENT_MOJO_AL_H
