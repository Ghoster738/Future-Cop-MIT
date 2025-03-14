#include "StandardInputSet.h"
#include "../StandardInputSet.h"
#include <assert.h>

void Controls::StandardInputSet::clearInputs() {
    for( size_t i = 0; i < Buttons::TOTAL_BUTTONS; i++ ) {
        states[ i ].clear();
    }
}

Controls::StandardInputSet::StandardInputSet( std::string name_param ) : InputSet( name_param ) {
    this->states[ Buttons::MENU ].declare(          "menu", &changed );
    this->states[ Buttons::UP ].declare(            "up", &changed );
    this->states[ Buttons::DOWN ].declare(          "down", &changed );
    this->states[ Buttons::LEFT ].declare(          "left", &changed );
    this->states[ Buttons::RIGHT ].declare(         "right", &changed );
    this->states[ Buttons::ROTATE_LEFT ].declare(   "rotate_left", &changed );
    this->states[ Buttons::ROTATE_RIGHT ].declare(  "rotate_right", &changed );
    this->states[ Buttons::WEAPON_LIGHT ].declare(  "weapon_light", &changed );
    this->states[ Buttons::WEAPON_MEDIUM ].declare( "weapon_medium", &changed );
    this->states[ Buttons::WEAPON_HEAVY ].declare(  "weapon_heavy", &changed );
    this->states[ Buttons::ACTION ].declare(        "action", &changed );
    this->states[ Buttons::JUMP ].declare(          "jump", &changed );
    this->states[ Buttons::CAMERA ].declare(        "camera", &changed );
    this->states[ Buttons::CHANGE_TARGET ].declare( "change_target", &changed );
    this->states[ Buttons::MAP ].declare(           "map",  &changed );
}

Controls::StandardInputSet::~StandardInputSet() {
    // input_set_internal_data_p is not used.
    assert( input_set_internal_data_p == nullptr );
}
