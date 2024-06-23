#ifndef SOUNDS_ENVIRONMENT_DUMMY_H
#define SOUNDS_ENVIRONMENT_DUMMY_H

#include "../Environment.h"

namespace Sound {
namespace Dummy {

class Environment : public Sound::Environment {
public:
    Environment();
    virtual ~Environment();

    static int initSystem();
    static int deinitEntireSystem();

    virtual uint32_t getEnvironmentIdentifier() const;
    virtual int loadResources( const Data::Accessor &accessor );

    virtual bool setMusicState(Sound::PlayerState player_state);
    virtual PlayerState getMusicState() const;

    virtual bool queueTrack(uint32_t track_offset);
    virtual bool setTrackPlayerState(PlayerState player_state);
    virtual PlayerState getTrackPlayerState() const;

    virtual Listener* getListenerReference(Listener::WhichEar listener_type);

    virtual Speaker* allocateSpeaker();

    virtual void advanceTime(std::chrono::high_resolution_clock::duration duration);
};

}
}

#endif // SOUNDS_ENVIRONMENT_DUMMY_H
