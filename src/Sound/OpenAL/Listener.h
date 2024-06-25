#ifndef SOUND_OPENAL_LISTENER_H
#define SOUND_OPENAL_LISTENER_H

#include "../Listener.h"
#include "Speaker.h"

#include <vector>

namespace Sound {
namespace OpenAL {

class Listener : public Sound::Listener {
public:
    struct Source {
        Speaker *speaker_r;
        ALuint queue_source;
    };

    size_t source_max_length;
    std::vector<Source> sources;

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
