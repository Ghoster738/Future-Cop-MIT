#ifndef SOUND_OPENAL_STREAM_H
#define SOUND_OPENAL_STREAM_H

#include "Environment.h"
#include "../Stream.h"

namespace Sound::OpenAL {

class Stream : public Sound::Stream {
public:
    Environment *environment_r;

    ALuint source;
    std::vector<ALuint> buffers;

    unsigned num_of_channels;
    ALsizei audio_samples_per_channel;
    ALsizei frequency;

    std::vector<int16_t> resample_buffer;

    virtual ~Stream();

    virtual bool appendSamples(float *interleaved_samples_r, unsigned num_of_channels, int audio_samples_per_channel);

    virtual bool setSpeakerState(PlayerState speaker_state);

    virtual PlayerState getSpeakerState() const;

    virtual unsigned getNumOfChannels() const;

    virtual unsigned getSamplesPerChannel() const;

    virtual size_t getFrequency() const;
};

}

#endif // SOUND_OPENAL_STREAM_H
