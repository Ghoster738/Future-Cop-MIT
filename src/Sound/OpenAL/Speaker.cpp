#include "Speaker.h"

#include "Environment.h"

namespace Sound {
namespace OpenAL {

Sound::Speaker* Environment::allocateSpeaker(uint32_t resource_id) {
    auto result = id_to_sound.find(resource_id);

    if(result == id_to_sound.end())
        return nullptr;

    Speaker *speaker_p = new Speaker();

    speaker_p->sound_source = result->second;

    return speaker_p;
}

Speaker::~Speaker() {
    // Speaker does not need to call deallocate on speaker_p->sound_source.
    // Environment is responsiable for that.
}

void Speaker::setLocation(const Location &location) {
    Sound::Speaker::setLocation(location);
}

const Location& Speaker::getLocation() const {
    return Sound::Speaker::getLocation();
}

bool Speaker::setSpeakerState(PlayerState speaker_state) {
    return Sound::Speaker::setSpeakerState(speaker_state);
}

PlayerState Speaker::getSpeakerState() const {
    return Sound::Speaker::getSpeakerState();
}

void Speaker::setRepeatMode(bool repeat) {
    Sound::Speaker::setRepeatMode(repeat);
}

bool Speaker::getRepeatMode() const {
    return Sound::Speaker::getRepeatMode();
}

}
}
