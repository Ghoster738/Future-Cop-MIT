#include "SoundBuffer.h"

namespace Sound {
namespace OpenAL {
namespace MojoAL {
namespace Internal {

ALenum SoundBuffer::allocate(const Data::Mission::WAVResource &sound) {
    const ALenum format = Internal::SoundBuffer::getFormat(sound.getChannelNumber(), sound.getBitsPerSample());

    if(format == AL_INVALID_ENUM)
        return AL_INVALID_ENUM;

    alGenBuffers(1, &buffer_index);

    ALenum error_state = alGetError();

    if(error_state != AL_NO_ERROR) {
        return error_state; // Posiable values AL_INVALID_VALUE, AL_OUT_OF_MEMORY and etc.
    }

    alBufferData(buffer_index, format, sound.getPCMData(), sound.getTotalPCMBytes(), sound.getSampleRate());

    error_state = alGetError();

    if(error_state != AL_NO_ERROR) {
        return error_state;
    }

    uint64_t size_of_demonator = sound.getChannelNumber() * (sound.getBitsPerSample() / 8) * sound.getSampleRate();

    std::chrono::duration<double> duration_second(static_cast<double>(sound.getTotalPCMBytes()) / static_cast<double>(size_of_demonator));

    auto conversion = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(duration_second);

    duration = conversion;

    return AL_NO_ERROR;
}

ALenum SoundBuffer::deallocate() {
    if(buffer_index != 0) {
        alDeleteBuffers(1, &buffer_index);

        ALenum error_state = alGetError();

        if(error_state != AL_NO_ERROR) {
            return error_state;
        }
    }

    buffer_index = 0;

    return AL_NO_ERROR;
}

ALenum SoundBuffer::getFormat(unsigned int number_of_channels, unsigned int bits_per_sample) {
    if(number_of_channels == 1) {
        if(bits_per_sample == 8)
            return AL_FORMAT_MONO8;
        else if(bits_per_sample == 16)
            return AL_FORMAT_MONO16;
        else
            return AL_INVALID_ENUM;
    }
    else if(number_of_channels == 2) {
        if(bits_per_sample == 8)
            return AL_FORMAT_STEREO8;
        else if(bits_per_sample == 16)
            return AL_FORMAT_STEREO16;
        else
            return AL_INVALID_ENUM;
    }
    else
        return AL_INVALID_ENUM;
}

}
}
}
}
