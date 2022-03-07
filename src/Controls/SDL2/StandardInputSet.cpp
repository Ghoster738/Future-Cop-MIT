#include "StandardInputSet.h"
#include "../StandardInputSet.h"
#include <assert.h>

Controls::StandardInputSet::StandardInputSet( std::string name_param ) : InputSet( name_param ) {
    this->states[ Buttons::TOGGLE_COMMAND_LINE ].declare( "TOGGLE_COMMAND_LINE", &changed );
    this->states[ Buttons::UP ].declare(            "UP", &changed );
    this->states[ Buttons::DOWN ].declare(          "DOWN", &changed );
    this->states[ Buttons::LEFT ].declare(          "LEFT", &changed );
    this->states[ Buttons::RIGHT ].declare(         "RIGHT", &changed );
    this->states[ Buttons::ROTATE_LEFT ].declare(   "ROTATE_LEFT", &changed );
    this->states[ Buttons::ROTATE_RIGHT ].declare(  "ROTATE_RIGHT", &changed );
    this->states[ Buttons::WEAPON_LIGHT ].declare(  "WEAPON_LIGHT", &changed );
    this->states[ Buttons::WEAPON_MEDIUM ].declare( "WEAPON_MEDIUM", &changed );
    this->states[ Buttons::WEAPON_HEAVY ].declare(  "WEAPON_HEAVY", &changed );
    this->states[ Buttons::ACTION ].declare(        "ACTION", &changed );
    this->states[ Buttons::JUMP ].declare(          "JUMP", &changed );
    this->states[ Buttons::CAMERA ].declare(        "CAMERA", &changed );
    this->states[ Buttons::CHANGE_TARGET ].declare( "CHANGE_TARGET", &changed );
    this->states[ Buttons::MAP ].declare(           "MAP",  &changed );
}

Controls::StandardInputSet::~StandardInputSet() {
    // input_set_internal_data_p is not used.
    assert( input_set_internal_data_p == nullptr );
}
