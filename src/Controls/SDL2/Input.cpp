#include "Input.h"
#include "../Input.h"
#include <assert.h>

Controls::Input::Input() {
    auto input_p = new Controls::SDL2::Input;

    // SDL_USEREVENT is used to indicate that the Input is not set.
    input_p->sdl_event.type = SDL_USEREVENT;

    input_internals_p = reinterpret_cast<void*>( input_p );
}

Controls::Input::~Input() {
    // Never should input_internals_p be nullptr!
    assert( input_internals_p != nullptr );
    delete reinterpret_cast<Controls::SDL2::Input*>( input_internals_p );
}

void Controls::Input::declare( std::string name, bool *global_change ) {
    auto input_r = reinterpret_cast<Controls::SDL2::Input*>( input_internals_p );

    this->name = name;

    input_r->global_change = global_change;
    input_r->changed = false;
    input_r->state = 0.0f;
}

bool Controls::Input::isChanged() const {
    auto input_p = reinterpret_cast<Controls::SDL2::Input*>( input_internals_p );

    return input_p->changed;
}

float Controls::Input::getState() const {
    auto input_p = reinterpret_cast<Controls::SDL2::Input*>( input_internals_p );

    return input_p->state;
}
