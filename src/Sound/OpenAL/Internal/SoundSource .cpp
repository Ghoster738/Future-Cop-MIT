#include "SoundSource.h"

namespace Sound {
namespace OpenAL {
namespace Internal {

ALenum SoundSource::allocate(const Data::Mission::WAVResource &sound) {
    this->no_repeat_offset = true;

    ALenum return_state = this->buffer_indexes[0].allocate(sound);

    if(return_state != AL_NO_ERROR)
        return return_state;

    if(sound.hasLoop()) {
        this->no_repeat_offset = false;

        this->buffer_indexes[1].allocate(sound, 0, sound.getLoopBeginSample());
        this->buffer_indexes[2].allocate(sound, sound.getLoopBeginSample(), sound.getLoopEndSample());
    }

    return AL_NO_ERROR;
}

ALenum SoundSource::deallocate() {
    ALenum return_state = this->buffer_indexes[0].deallocate();

    if(!this->no_repeat_offset ) {
        this->buffer_indexes[1].deallocate();
        this->buffer_indexes[2].deallocate();
    }

    if(return_state != AL_NO_ERROR)
        return return_state;

    return AL_NO_ERROR;
}

}
}
}
