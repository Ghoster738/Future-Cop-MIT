#ifndef SOUND_SPEAKER_H
#define SOUND_SPEAKER_H

#include "Location.h"
#include "PlayerState.h"

namespace Sound {

class Speaker {
protected:
    Location location;
    PlayerState speaker_state;
    bool repeat;

public:
    virtual ~Speaker() = 0;

    /**
     * Set the position, velocity, and direction of this speaker.
     * @param location This sets the location of the speaker.
     */
    virtual void setLocation(Location location);

    /**
     * Set the position, velocity, and direction of this speaker.
     * @param location This sets the location of the speaker.
     */
    virtual void setLocation(const Location &location);

    /**
     * This returns the location of the speaker.
     * @return The location which contains the position, velocity and direction of the speaker.
     */
    virtual const Location& getLocation() const;

    /**
     * This sets the state of the speaker.
     * @param player_state This STOP, PAUSE, and PLAY are the possible states.
     * @return true if the speaker state is successfully set.
     */
    virtual bool setSpeakerState(PlayerState speaker_state);

    /**
     * @return The state of the speaker.
     */
    virtual PlayerState getSpeakerState() const;

    /**
     * Play the audio again?
     * @param repeat true if the programmer wants to repeat the audio.
     */
    virtual void setRepeatMode(bool repeat);

    /**
     * @return true if the Speaker is set to repeat mode.
     */
    virtual bool getRepeatMode() const;
};

}

#endif
