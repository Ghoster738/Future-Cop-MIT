#ifndef CONTROLS_INPUT_INTERNALS_H
#define CONTROLS_INPUT_INTERNALS_H

#include "SDL_events.h"

namespace Controls {

namespace SDL2 {

struct Input {
    bool *global_change; // This indicates an InputSet change.
    bool changed; // Indicates that there is a change in state for this input.
    float state; // A real number which is greater than or equal to 0 and less than or equal to 1.

    // Everything below this point is all related to SDL2.
    SDL_Event sdl_event;
    enum Axis {
        X = 0,
        Y,
        Z,
        NONE
    } axis;
};

}

}

#endif // CONTROLS_INPUT_INTERNALS_H
