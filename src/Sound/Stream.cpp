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

}
