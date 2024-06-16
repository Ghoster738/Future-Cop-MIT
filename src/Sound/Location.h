#ifndef SOUND_LOCATION_H
#define SOUND_LOCATION_H

#include <glm/vec3.hpp>

namespace Sound {

/**
 * This struct holds the position, velocity and direction of everything that an audio element with position needs.
 */
struct Location { // TODO Find a more proper name.
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 direction;
    glm::vec3 &orientation = direction;

    Location& operator=(const Location& location) {
        position  = location.position;
        velocity  = location.velocity;
        direction = location.direction;
        return *this;
    }
};

}

#endif
