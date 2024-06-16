#include "Listener.h"

namespace Sound {

Listener::Listener(WhichEar which_ear) : location(), p_which_ear(which_ear) {}

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
