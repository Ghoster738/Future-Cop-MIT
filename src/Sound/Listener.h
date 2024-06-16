#ifndef SOUND_LISTENER_H
#define SOUND_LISTENER_H

#include "Location.h"

namespace Sound {

class Listener {
public:
    enum WhichEar {
        RIGHT,
        LEFT,
        BOTH
    };

private:
    Location location;
    WhichEar p_which_ear;

public:
    /**
     * This is the constructor for this class. Only Environment should worry about this.
     */
    Listener(WhichEar which_ear = WhichEar::BOTH);

    /**
     * This is the destructor. It is pure virtual on purpose to make this class only allocatable with Environment.
     */
    virtual ~Listener() = 0;

    /**
     * Set the position, velocity, and orientation of this listener.
     * @param location This sets the location of the listener.
     */
    virtual void setLocation(Location location);

    /**
     * Set the position, velocity, and orientation of this listener.
     * @param location This sets the location of the listener.
     */
    virtual void setLocation(const Location &location);

    /**
     * This returns the location of the listener.
     * @return The location which contains the position, velocity and orientation of the listener.
     */
    virtual const Location& getLocation() const;

    const WhichEar &which_ear = p_which_ear;
};

}

#endif
