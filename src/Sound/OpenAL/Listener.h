#ifndef SOUND_OPENAL_LISTENER_H
#define SOUND_OPENAL_LISTENER_H

#include "../Listener.h"

namespace Sound {
namespace OpenAL {

class Listener : public Sound::Listener {
public:
    Listener(WhichEar which_ear = WhichEar::BOTH);

    virtual ~Listener();

    virtual void setEnabled( bool enabled );
    virtual bool getEnabled() const;

    virtual void setLocation(const Location &location);
    virtual const Location& getLocation() const;
};

}
}

#endif // SOUND_OPENAL_LISTENER_H
