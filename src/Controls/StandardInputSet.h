#ifndef CONTROLS_STANDARD_H
#define CONTROLS_STANDARD_H

#include "Input.h"
#include "InputSet.h"

namespace Controls {

/**
 * These are the standard controls for the mech walker.
 */
class StandardInputSet : public InputSet {
public:
    enum Buttons {
        TOGGLE_COMMAND_LINE = 0,
        UP,
        DOWN,
        LEFT,
        RIGHT,
        ROTATE_LEFT,
        ROTATE_RIGHT,
        WEAPON_LIGHT,
        WEAPON_MEDIUM,
        WEAPON_HEAVY,
        ACTION,
        JUMP,
        CAMERA,
        CHANGE_TARGET,
        MAP,
        TOTAL_BUTTONS
    };
private:
    Input states[ Buttons::TOTAL_BUTTONS ];
public:
    StandardInputSet( std::string name );
    virtual ~StandardInputSet();

    /**
     * This is the accessor for the button states stored within the standard class.
     * @param which is which button would need to be accessed.
     * @return the pointer to the button state if the button is accessed.
     */
    Input* getInput( unsigned int index ) {
        return InputSet::getInput( index, Buttons::TOTAL_BUTTONS, states );
    }

    /**
     * This is the accessor for the button states stored within the standard class.
     * @param which is which button would need to be accessed.
     * @return the pointer to the button state if the button is accessed.
     */
    Input *const getInput( unsigned int index ) const {
        return InputSet::getInput( index, Buttons::TOTAL_BUTTONS, const_cast<Input *const>( states ) );
    }
    
    Input* getInput( std::string name ) {
        return InputSet::getInput( name, Buttons::TOTAL_BUTTONS, states );
    }
    
    Input *const getInput( std::string name ) const {
        return InputSet::getInput( name, Buttons::TOTAL_BUTTONS, const_cast<Input *const>( states ) );
    }
};

};

#endif // CONTROLS_STANDARD_H
