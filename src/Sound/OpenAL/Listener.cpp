#include "Listener.h"

namespace Sound {
namespace OpenAL {

Listener::Listener(WhichEar which_ear) : Sound::Listener(which_ear) {
}

Listener::~Listener() {
}

void Listener::setEnabled(bool enabled) {
    Sound::Listener::setEnabled(enabled);
}

bool Listener::getEnabled() const {
    return Sound::Listener::getEnabled();
}

void Listener::setLocation(const Location &location) {
    Sound::Listener::setLocation(location);
}

const Location& Listener::getLocation() const {
    return Sound::Listener::getLocation();
}

}
}
