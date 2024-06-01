#include "SoundQueue.h"

namespace Sounds {
namespace SDL2 {
namespace MojoAL {
namespace Internal {

SoundQueue::SoundQueue(unsigned p_queue_limit) : queue_limit(p_queue_limit), sound_queue(), current_sound_element(), player_state(Sounds::PlayerState::STOP), allocated_queue_source(false), queue_source() {
    current_sound_element.buffer_index = 0;
    current_sound_element.duration = std::chrono::high_resolution_clock::duration(0);
}

ALenum SoundQueue::initialize() {
    if(allocated_queue_source)
        return AL_NO_ERROR;

    ALenum error_state;

    alGenSources(1,&queue_source);

    error_state = alGetError();

    if(error_state != AL_NO_ERROR) {
        return error_state;
    }

    alSourcei(queue_source, AL_SOURCE_RELATIVE, AL_TRUE);

    error_state = alGetError();

    if(error_state != AL_NO_ERROR) {
        return error_state;
    }

    allocated_queue_source = true;

    return AL_NO_ERROR;
}

ALenum SoundQueue::reset() {
    if(!allocated_queue_source)
        return AL_NO_ERROR;

    ALenum error_state = alGetError();

    alSourceStop(queue_source);

    error_state = alGetError();

    alSourcei(queue_source, AL_BUFFER, 0);

    error_state = alGetError();

    while(!sound_queue.empty())
        sound_queue.pop();

    current_sound_element.buffer_index = 0;
    current_sound_element.duration = std::chrono::high_resolution_clock::duration(0);

    return error_state;
}

void SoundQueue::push(SoundBuffer new_sound) {
    if(sound_queue.size() > queue_limit)
        sound_queue.pop();

    sound_queue.push(new_sound);
}

void SoundQueue::setPlayerState(Sounds::PlayerState player_state) {
    if(!allocated_queue_source)
        return;

    switch(player_state) {
        case Sounds::PlayerState::STOP:
            {
                alSourceStop(queue_source);

                while(!sound_queue.empty())
                    sound_queue.pop();

                current_sound_element.buffer_index = 0;
                current_sound_element.duration = std::chrono::high_resolution_clock::duration(0);
            }
            break;
        case Sounds::PlayerState::PAUSE:
            {
                if(this->player_state != player_state)
                    alSourcePause(queue_source);
            }
            break;
        case Sounds::PlayerState::PLAY:
            {
                if(this->player_state != player_state)
                    alSourcePlay(queue_source);
            }
            break;
    }

    this->player_state = player_state;
}

void SoundQueue::update(std::chrono::high_resolution_clock::duration duration) {
    alGetError();

    // There is nothing to update.
    if(duration.count() == 0 || player_state != Sounds::PlayerState::PLAY)
        return;
    else if(duration.count() >= current_sound_element.duration.count()) {
        if(!sound_queue.empty()) {
            current_sound_element = sound_queue.back();
            sound_queue.pop();

            alSourceStop(queue_source);
            alSourcei(queue_source, AL_BUFFER, current_sound_element.buffer_index);
            alSourcePlay(queue_source);
        }
        else {
            current_sound_element.buffer_index = 0;
            current_sound_element.duration = std::chrono::high_resolution_clock::duration(0);
        }
    }
    else
        current_sound_element.duration -= duration;
}

}
}
}
}
