#include "Listener.h"

namespace Sound {

void Listener::setEnabled( bool enabled ) {
    this->enabled = enabled;
}

bool Listener::getEnabled() const {
    return enabled;
}

void Listener::setLocation(Location location) {
    this->location = location;
}

void Listener::setLocation(const Location &location) {
    this->location = location;
}

const Location& Listener::getLocation() const {
    return this->location;
}

}
