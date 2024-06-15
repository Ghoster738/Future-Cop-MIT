#ifndef SOUND_SPEAKER_H
#define SOUND_SPEAKER_H

#include "Environment.h"
#include "Location.h"
#include "PlayerState.h"

namespace Sound {

class Speaker {
private:
    Location location;
    PlayerState speaker_state;

public:
    /**
     * This allocates the speaker.
     * @note To actually play the speaker set its location then after this set the speaker to PLAY.
     * @param env This is the environment to read from.
     * @param sound_identifier This is the specific sound to load.
     * @return If everything is valid then a speaker pointer would be returned.
     */
    static Sound::Speaker* alloc( Environment &env, uint32_t sound_identifier );

    virtual ~Speaker();

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
};

}

#endif
