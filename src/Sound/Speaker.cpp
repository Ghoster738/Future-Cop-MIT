#include "Speaker.h"

namespace Sound {

void Speaker::setLocation(const Location &location) {
    this->location = location;
}

const Location& Speaker::getLocation() const {
    return this->location;
}

bool Speaker::setSpeakerState(PlayerState speaker_state) {
    this->speaker_state = speaker_state;
    return true;
}

PlayerState Speaker::getSpeakerState() const {
    return speaker_state;
}

void Speaker::setRepeatMode(bool repeat) {
    this->repeat = repeat;
}

bool Speaker::getRepeatMode() const {
    return this->repeat;
}

}
