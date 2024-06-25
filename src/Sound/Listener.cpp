#include "Listener.h"

namespace Sound {

Listener::Listener(WhichEar which_ear) : location(), p_which_ear(which_ear) {}

Listener::~Listener() {}

void Listener::setEnabled(bool enabled) {
    this->enabled = enabled;
}

bool Listener::getEnabled() const {
    return enabled;
}

void Listener::setLocation(const Location &location) {
    this->location = location;
}

const Location& Listener::getLocation() const {
    return this->location;
}

}
