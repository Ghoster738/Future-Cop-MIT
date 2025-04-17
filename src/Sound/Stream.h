#ifndef SOUND_STREAM_H
#define SOUND_STREAM_H

#include "Location.h"
#include "PlayerState.h"

#include <stddef.h>

namespace Sound {

class Stream {
protected:
    PlayerState speaker_state;

public:
    virtual ~Stream() = 0;

    /**
     * Function to append the audio data to stream.
     * @param interleaved_samples_r The pointer to the audio samples.
     * @param num_of_channels The number of channels that interleaved_samples_r has.
     * @param audio_samples_per_channel The samples per channel to the speaker.
     * @return If the parameters match what was declared then return true. If this fails then return false.
     */
    virtual bool appendSamples(float *interleaved_samples_r, unsigned num_of_channels, int audio_samples_per_channel) = 0;

    /**
     * This sets the state of the speaker.
     * @param player_state This STOP, PAUSE, and PLAY are the possible states.
     * @return true if the speaker state is successfully set.
     */
    virtual bool setSpeakerState(PlayerState speaker_state);

    /**
     * @return The state of the speaker.
     */
    virtual PlayerState getSpeakerState() const = 0;

    /**
     * @return The number of audio channels that the Video is using.
     */
    virtual unsigned getNumOfChannels() const = 0;

    /**
     * @return The audio samples per channel of an **audio buffer** given to the stream by appendSamples method.
     */
    virtual unsigned getSamplesPerChannel() const = 0;

    /**
     * @return The stream's frequency.
     */
    virtual size_t getFrequency() const = 0;
};

}

#endif // SOUND_STREAM_H
