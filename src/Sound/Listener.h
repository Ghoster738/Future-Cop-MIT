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

protected:
    Location location;
    WhichEar p_which_ear;
    bool enabled;

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
     * This enables or disables the listener.
     * @warning This method is important as each listener will be disabled by default.
     * @param enabled The value used. If true then the listener will listen to audio. If false then the listener will be deaf.
     */
    virtual void setEnabled( bool enabled );

    /**
     * Use this to see if the listener is enabled.
     * @return return the enabled state.
     */
    virtual bool getEnabled() const;

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
