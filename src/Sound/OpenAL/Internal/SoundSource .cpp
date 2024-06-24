#include "SoundSource.h"

namespace Sound {
namespace OpenAL {
namespace Internal {

ALenum SoundSource::allocate(const Data::Mission::WAVResource &sound) {
    if(!sound.hasLoop()) {
        this->no_repeat_offset = true;
    }
    else {
        this->no_repeat_offset = false;
    }
}

ALenum SoundSource::deallocate() {}

}
}
}
