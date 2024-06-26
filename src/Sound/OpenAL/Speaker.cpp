#include "Speaker.h"

namespace Sound {
namespace OpenAL {

Sound::Speaker* Environment::allocateSpeaker(uint32_t resource_id) {
    auto result = id_to_sound.find(resource_id);

    if(result == id_to_sound.end())
        return nullptr;

    Speaker *speaker_p = new Speaker();

    speaker_p->environment_r = this;
    speaker_p->sound_source = result->second;

    return speaker_p;
}

Speaker::~Speaker() {
    // Speaker does not need to call deallocate on speaker_p->sound_source.
    // Environment is responsiable for that.

    environment_r->listener_both.dequeueSpeaker(*this);
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

void Speaker::updateLocation(ALuint source) const {
    // TODO Upgrade this for left and right ear case.
    alSource3f(source,  AL_POSITION,  location.position.x,  location.position.y,  location.position.z);
    alSource3f(source,  AL_VELOCITY,  location.velocity.x,  location.velocity.y,  location.velocity.z);
    alSource3f(source, AL_DIRECTION, location.direction.x, location.direction.y, location.direction.z);
}

}
}
