#include "Stream.h"

#define AL_FORMAT_MONO_FLOAT32 0x10010
#define AL_FORMAT_STEREO_FLOAT32 0x10011

namespace Sound::OpenAL {

Sound::Stream* Environment::allocateStream(unsigned num_of_channels, size_t audio_samples_per_channel, size_t frequency) {
    // Detect errors early.
    if(num_of_channels != 1 && num_of_channels != 2)
        return NULL;

    // Attempt to allocate required OpenAL stuff.
    ALenum error_state;
    ALuint source;

    // Generate the source.
    alGenSources(1, &source);

    error_state = alGetError();
    if(error_state != AL_NO_ERROR)
        return NULL;

    // Make the source follow the listener.
    alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);

    error_state = alGetError();
    if(error_state != AL_NO_ERROR) {
        alDeleteSources(1, &source);

        error_state = alGetError();

        return NULL;
    }

    // Finally, allocate this class.
    auto stream_p = new Stream();

    stream_p->source = source;
    // stream_p->buffers = buffers; // appendSamples will generate buffers anyways.

    stream_p->num_of_channels = num_of_channels;
    stream_p->audio_samples_per_channel = audio_samples_per_channel;
    stream_p->frequency = frequency;

    return stream_p;
}

Stream::~Stream() {
    alDeleteSources(1, &this->source);

    if(!this->buffers.empty())
        alDeleteBuffers(this->buffers.size(), &this->buffers[0]);
}

bool Stream::appendSamples(float *interleaved_samples_r, unsigned num_of_channels, unsigned audio_samples_per_channel) {
    ALenum error_state;
    ALint buffer_processed_amount;

    if(interleaved_samples_r == NULL)
        return false;

    if(this->num_of_channels != num_of_channels)
        return false;

    if(this->audio_samples_per_channel != audio_samples_per_channel)
        return false;

    alGetSourcei(this->source, AL_BUFFERS_PROCESSED, &buffer_processed_amount);

    ALuint buffer;

    error_state = alGetError();
    if(buffer_processed_amount != 0) {
        alSourceUnqueueBuffers(this->source, 1, &buffer);

        error_state = alGetError();
    }
    else {
        alGenBuffers(1, &buffer);

        this->buffers.push_back(buffer);
    }

    if(num_of_channels == 1)
        alBufferData(buffer,   AL_FORMAT_MONO_FLOAT32, interleaved_samples_r,     sizeof(float) * audio_samples_per_channel, this->frequency);
    else
        alBufferData(buffer, AL_FORMAT_STEREO_FLOAT32, interleaved_samples_r, 2 * sizeof(float) * audio_samples_per_channel, this->frequency);

    error_state = alGetError();
    if(error_state != AL_NO_ERROR)
        return false;

    alSourceQueueBuffers(this->source, 1, &buffer);

    error_state = alGetError();
    if(error_state != AL_NO_ERROR)
        return false;

    return true;
}

bool Stream::setSpeakerState(PlayerState speaker_state) {
    if(speaker_state == PlayerState::PLAY) {
        alSourcePlay(this->source);
    }
    else if(speaker_state == PlayerState::STOP) {
        alSourceStop(this->source);
    }
    else if(speaker_state == PlayerState::PAUSE) {
        alSourcePause(this->source);
    }

    ALenum error_state = alGetError();

    if(error_state != AL_NO_ERROR) {
        return false;
    }

    return Sound::Stream::setSpeakerState(speaker_state);
}

PlayerState Stream::getSpeakerState() const {
    return Sound::Stream::getSpeakerState();
}

size_t Stream::getFrequency() const {
    return this->frequency;
}

}
