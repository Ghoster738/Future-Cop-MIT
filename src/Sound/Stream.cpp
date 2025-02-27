#include "Stream.h"

namespace Sound {

Stream::~Stream() {}

bool Stream::setSpeakerState(PlayerState speaker_state) {
    this->speaker_state = speaker_state;
    return true;
}

PlayerState Stream::getSpeakerState() const {
    return this->speaker_state;
}

unsigned Stream::getNumOfChannels() const {
    return this->num_of_channels;
}

unsigned Stream::getSamplesPerChannel() const {
    return this->audio_samples_per_channel;
}

}
