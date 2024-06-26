#ifndef SOUND_OPENAL_SPEAKER_H
#define SOUND_OPENAL_SPEAKER_H

#include "Environment.h"
#include "../Speaker.h"
#include "Internal/SoundSource.h"

namespace Sound {
namespace OpenAL {

class Speaker : public Sound::Speaker {
public:
    Environment *environment_r;
    Internal::SoundSource sound_source;

    virtual ~Speaker();

    virtual void setLocation(const Location &location);
    virtual const Location& getLocation() const;

    virtual bool setSpeakerState(PlayerState speaker_state);
    virtual PlayerState getSpeakerState() const;

    virtual void setRepeatMode(bool repeat);
    virtual bool getRepeatMode() const;

    void updateLocation(ALuint source) const;
};

}
}

#endif // SOUND_OPENAL_SPEAKER_H
