#include "SoundBuffer.h"

namespace Sound {
namespace OpenAL {
namespace Internal {

ALenum SoundBuffer::allocate(const Data::Mission::WAVResource &sound, uint_fast32_t start_sample_offset, uint_fast32_t end_sample_offset) {
    const uint64_t data_block = sound.getChannelNumber() * (sound.getBitsPerSample() / 8);
    const uint64_t full_audio_samples = sound.getTotalPCMBytes() / data_block;

    if(start_sample_offset > end_sample_offset)
        return AL_INVALID_VALUE;

    if(end_sample_offset > full_audio_samples)
        return AL_INVALID_VALUE;

    const ALenum format = Internal::SoundBuffer::getFormat(sound.getChannelNumber(), sound.getBitsPerSample());

    if(format == AL_INVALID_ENUM)
        return AL_INVALID_ENUM;

    alGenBuffers(1, &buffer_index);

    ALenum error_state = alGetError();

    if(error_state != AL_NO_ERROR) {
        return error_state; // Posiable values AL_INVALID_VALUE, AL_OUT_OF_MEMORY and etc.
    }

    if(start_sample_offset == end_sample_offset)
        alBufferData(buffer_index, format, sound.getPCMData(), sound.getTotalPCMBytes(), sound.getSampleRate());
    else
        alBufferData(buffer_index, format, sound.getPCMData() + start_sample_offset * data_block, (end_sample_offset - start_sample_offset) * data_block, sound.getSampleRate());

    error_state = alGetError();

    if(error_state != AL_NO_ERROR) {
        return error_state;
    }

    if(start_sample_offset == end_sample_offset)
        duration = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(std::chrono::duration<double>(full_audio_samples / static_cast<double>(sound.getSampleRate())));
    else
        duration = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(std::chrono::duration<double>((end_sample_offset - start_sample_offset) / static_cast<double>(sound.getSampleRate())));

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
