#ifndef SOUND_OPENAL_INTERNAL_SOUND_SOURCE
#define SOUND_OPENAL_INTERNAL_SOUND_SOURCE

#include "SoundBuffer.h"

#include <chrono>
#include "al.h"

namespace Sound {
namespace OpenAL {
namespace Internal {

struct SoundSource {
    bool no_repeat_offset;
    SoundBuffer buffer_indexes[2];

    SoundSource() : no_repeat_offset(false), buffer_indexes{SoundBuffer(), SoundBuffer()} {}

    ALenum allocate(const Data::Mission::WAVResource &sound);
    ALenum deallocate();
};

}
}
}


#endif // SOUND_OPENAL_INTERNAL_SOUND_SOURCE
