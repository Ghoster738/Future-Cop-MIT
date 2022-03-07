#include "CursorInputSet.h"
#include "../CursorInputSet.h"
#include <assert.h>

Controls::CursorInputSet::CursorInputSet( std::string name_param ) : InputSetInternal( name_param ) {
    this->states[ Inputs::LEFT_BUTTON   ].declare( "LEFT_BUTTON",   &changed );
    this->states[ Inputs::MIDDLE_BUTTON ].declare( "MIDDLE_BUTTON", &changed );
    this->states[ Inputs::RIGHT_BUTTON  ].declare( "RIGHT_BUTTON",  &changed );
    this->states[ Inputs::MOTION_X      ].declare( "MOTION_X",      &changed );
    this->states[ Inputs::MOTION_Y      ].declare( "MOTION_Y",      &changed );
    this->states[ Inputs::WHEEL_X       ].declare( "WHEEL_X",       &changed );
    this->states[ Inputs::WHEEL_Y       ].declare( "WHEEL_Y",       &changed );
}

Controls::CursorInputSet::~CursorInputSet() {
    // input_set_internal_data_p is not used.
    assert( input_set_internal_data_p == nullptr );
}
