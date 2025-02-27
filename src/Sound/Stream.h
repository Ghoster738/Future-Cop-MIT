#ifndef SOUND_STREAM_H
#define SOUND_STREAM_H

#include "Location.h"
#include "PlayerState.h"

namespace Sound {

class Stream {
protected:
    size_t audio_samples_per_channel;
    PlayerState speaker_state;
    unsigned num_of_channels;

public:
    virtual ~Stream() = 0;

    virtual bool appendSamples(float *interleaved_samples_r, unsigned num_of_channels, unsigned audio_samples_per_channel) = 0;

    /**
     * This sets the state of the speaker.
     * @param player_state This STOP, PAUSE, and PLAY are the possible states.
     * @return true if the speaker state is successfully set.
     */
    virtual bool setSpeakerState(PlayerState speaker_state);

    /**
     * @return The state of the speaker.
     */
    virtual PlayerState getSpeakerState() const;

    virtual unsigned getNumOfChannels() const;

    virtual unsigned getSamplesPerChannel() const;

    virtual size_t getFrequency() const = 0;
};

}

#endif // SOUND_STREAM_H
