#include "Environment.h"

#include "../../../Data/Mission/MSICResource.h"
#include "../../../Data/Mission/SNDSResource.h"

namespace Sounds {
namespace SDL2 {
namespace MojoAL {

Environment::Environment() : alc_device_p(nullptr), alc_context_p(nullptr), music_buffer(0), music_source(0) {}

Environment::~Environment() {
    alcMakeContextCurrent(nullptr);

    if(alc_context_p != nullptr)
        alcDestroyContext(alc_context_p);

    if(alc_device_p != nullptr)
        alcCloseDevice(alc_device_p);
}

int Environment::initSystem() {
    return 1;
}

int Environment::deinitEntireSystem() {
    return 1;
}

std::string Environment::getEnvironmentIdentifier() const {
    return SDL2_WITH_MOJO_AL;
}

int Environment::loadResources( const Data::Accessor &accessor ) {
    if(alc_device_p == nullptr) {
        alc_device_p = alcOpenDevice(nullptr);

        if(alc_device_p == nullptr)
            return -2;
    }

    if(alc_context_p == nullptr) {
        alc_context_p = alcCreateContext(alc_device_p, nullptr);

        if(alc_context_p == nullptr) {
            return -3;
        }
    }

    if(alcMakeContextCurrent(alc_context_p) == ALC_FALSE)
        return -4;

    ALenum error_state = alGetError();

    const Data::Mission::MSICResource* misc_r = accessor.getSWVRAccessor().getConstMSIC();

    if(music_source != 0)
        alDeleteSources(1, &music_source);
    error_state = music_buffer.deallocate();

    if(error_state != AL_NO_ERROR)
        return -5;

    if(misc_r != nullptr) {
        const Data::Mission::WAVResource *const sound_r = misc_r->soundAccessor();

        error_state = music_buffer.allocate(*sound_r);

        switch(error_state) {
            case AL_INVALID_ENUM:
                return -6;
            case AL_INVALID_VALUE:
                return -7;
            case AL_OUT_OF_MEMORY:
                return -8;
            default:
                return -9;
            case AL_NO_ERROR:
                ; // Do nothing.
        }
    }

    alGenSources(1,&music_source);

    error_state = alGetError();

    if(error_state != AL_NO_ERROR) {
        return -10;
    }

    alSourcei(music_source, AL_BUFFER, music_buffer.buffer_index);

    alSourcei(music_source, AL_LOOPING, AL_TRUE);

    alSourcei(music_source, AL_SOURCE_RELATIVE, AL_TRUE);

    error_state = alGetError();

    if(error_state != AL_NO_ERROR) {
        return -11;
    }

    sound_queue.reset();

    for(auto key: tos_to_swvr) {
        ALenum current_error_state = key.second.deallocate();

        if(current_error_state != AL_NO_ERROR) {
            error_state = current_error_state;
        }
    }

    if(error_state != AL_NO_ERROR) {
        return -12;
    }

    tos_to_swvr.clear();

    auto snds_r = accessor.getSWVRAccessor().getAllConstSNDS();

    for(const Data::Mission::SNDSResource *track_r : snds_r) {
        uint32_t tos_offset = track_r->getSWVREntry().tos_offset;

        if(tos_to_swvr.find(tos_offset) == tos_to_swvr.end()) {
            const Data::Mission::WAVResource *const sound_r = track_r->soundAccessor();

            tos_to_swvr[tos_offset] = Internal::SoundBuffer();

            ALenum current_error_state = tos_to_swvr[tos_offset].allocate(*sound_r);

            if(current_error_state != AL_NO_ERROR) {
                error_state = current_error_state;
            }
        }
    }

    if(error_state != AL_NO_ERROR) {
        return -13;
    }

    error_state = alGetError();

    if(error_state != AL_NO_ERROR) {
        return -14;
    }

    sound_queue.setPlayerState(Sounds::PlayerState::STOP);

    error_state = alGetError();

    if(error_state != AL_NO_ERROR) {
        return -15;
    }

    error_state = sound_queue.initialize();

    if(error_state != AL_NO_ERROR) {
        return -16;
    }

    return 1;
}

bool Environment::setMusicState(Sounds::PlayerState player_state) {
    if(music_source == 0)
        return false;

    switch(player_state) {
        case Sounds::PlayerState::STOP:
            alSourceStop(music_source);
            break;
        case Sounds::PlayerState::PAUSE:
            alSourcePause(music_source);
            break;
        case Sounds::PlayerState::PLAY:
            alSourcePlay(music_source);
            break;
        default:
            return false;
    }

    ALenum error_state = alGetError();

    if(error_state == AL_NO_ERROR)
        return true;
    else
        return false;
}

Sounds::PlayerState Environment::getMusicState() const {
    if(music_source == 0)
        return Sounds::PlayerState::STOP;

    ALint state = -1;

    alGetSourcei(music_source, AL_SOURCE_STATE, &state);

    switch(state) {
        case AL_STOPPED:
            return Sounds::PlayerState::STOP;
            break;
        case AL_PAUSED:
            return Sounds::PlayerState::PAUSE;
            break;
        case AL_PLAYING:
            return Sounds::PlayerState::PLAY;
            break;
        default:
            return Sounds::PlayerState::STOP;
    }
}

bool Environment::queueTrack(uint32_t track_offset) {
    auto find = tos_to_swvr.find(track_offset);

    if(find == tos_to_swvr.end()) {
        return false;
    }
    else {
        Internal::SoundBuffer sound_buffer = (*find).second;

        sound_queue.push( sound_buffer );
        return true;
    }
}

bool Environment::setTrackPlayerState(PlayerState player_state) {
    sound_queue.setPlayerState(player_state);

    return true;
}

Sounds::PlayerState Environment::getTrackPlayerState() const {
    return sound_queue.getPlayerState();
}

void Environment::advanceTime(std::chrono::high_resolution_clock::duration duration) {
    sound_queue.update(duration);
}

}
}
}
