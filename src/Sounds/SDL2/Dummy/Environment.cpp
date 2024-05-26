#include "Environment.h"

namespace Sounds {
namespace SDL2 {
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
    return SDL2_WITH_NO_AUDIO;
}

int Environment::loadResources( const Data::Accessor &accessor ) {
    return 0;
}

bool Environment::setMusic(Sounds::PlayerState player_state) {
    return false;
}

}
}
}
