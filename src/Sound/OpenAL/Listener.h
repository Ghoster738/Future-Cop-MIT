#ifndef SOUND_OPENAL_LISTENER_H
#define SOUND_OPENAL_LISTENER_H

#include "../Listener.h"
#include "Speaker.h"

#include <chrono>
#include <vector>

namespace Sound {
namespace OpenAL {

class Listener : public Sound::Listener {
public:
    struct Source {
        Speaker *speaker_r;
        std::chrono::high_resolution_clock::duration time_limit;
        ALuint queue_source;

        Source();
        ~Source();
    };

    size_t source_max_length;
    std::vector<Source> sources;

    Listener(WhichEar which_ear = WhichEar::BOTH);

    virtual ~Listener();

    virtual void setEnabled( bool enabled );
    virtual bool getEnabled() const;

    virtual void setLocation(const Location &location);
    virtual const Location& getLocation() const;

    bool enqueueSpeaker(Speaker &speaker);

    bool dequeueSpeaker(Speaker &speaker);

    void process(std::chrono::high_resolution_clock::duration delta);
};

}
}

#endif // SOUND_OPENAL_LISTENER_H
