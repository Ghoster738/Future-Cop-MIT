#ifndef SOUND_OPENAL_SPEAKER_H
#define SOUND_OPENAL_SPEAKER_H

#include "../Speaker.h"

namespace Sound {
namespace OpenAL {
namespace Internal {

class Speaker : public Sound::Speaker {
public:
    virtual ~Speaker();

    virtual void setLocation(Location location);
    virtual void setLocation(const Location &location);
    virtual const Location& getLocation() const;

    virtual bool setSpeakerState(PlayerState speaker_state);
    virtual PlayerState getSpeakerState() const;

    virtual void setRepeatMode(bool repeat);
    virtual bool getRepeatMode() const;
};

}
}
}

#endif // SOUND_OPENAL_SPEAKER_H
