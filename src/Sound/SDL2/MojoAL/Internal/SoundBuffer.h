#ifndef SOUND_OPENAL_INTERNAL_SOUND_BUFFER
#define SOUND_OPENAL_INTERNAL_SOUND_BUFFER

#include "../../../../Data/Mission/WAVResource.h"

#include <chrono>
#include "al.h"

namespace Sound {
namespace SDL2 {
namespace MojoAL {
namespace Internal {

struct SoundBuffer {
    ALuint buffer_index;
    std::chrono::high_resolution_clock::duration duration;

    SoundBuffer(ALuint p_buffer_index = 0, std::chrono::high_resolution_clock::duration p_duration = std::chrono::high_resolution_clock::duration()) :
        buffer_index(p_buffer_index), duration(p_duration) {}

    ALenum allocate(const Data::Mission::WAVResource &sound);
    ALenum deallocate();

    static ALenum getFormat(unsigned int number_of_channels, unsigned int bits_per_sample);
};

}
}
}
}


#endif // SOUND_OPENAL_INTERNAL_SOUND_BUFFER
