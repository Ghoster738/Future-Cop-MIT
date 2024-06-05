#include "Environment.h"

namespace Sound {
namespace Dummy {

Environment::Environment() {}
Environment::~Environment() {}

int Environment::initSystem() {
    return 1;
}

int Environment::deinitEntireSystem() {
    return 1;
}

std::string Environment::getEnvironmentIdentifier() const {
    return NO_AUDIO;
}

int Environment::loadResources( const Data::Accessor &accessor ) {
    return 0;
}

bool Environment::setMusicState(Sound::PlayerState player_state) {
    return false;
}

Sound::PlayerState Environment::getMusicState() const {
    return Sound::PlayerState::STOP;
}

bool Environment::queueTrack(uint32_t track_offset) {
    return false;
}

bool Environment::setTrackPlayerState(PlayerState player_state) {
    return false;
}

Sound::PlayerState Environment::getTrackPlayerState() const {
    return Sound::PlayerState::STOP;
}

void Environment::advanceTime(std::chrono::high_resolution_clock::duration duration) {
}

}
}
