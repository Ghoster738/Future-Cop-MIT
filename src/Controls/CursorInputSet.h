#ifndef CONTROLS_CURSOR_INPUT_SET_H
#define CONTROLS_CURSOR_INPUT_SET_H

#include "Input.h"
#include "InputSet.h"

namespace Controls {

/**
 * This is the control for the mouse.
 *
 * This is more for editors rather than game play.
 * @warning the control layer only uses one mouse.
 */
class CursorInputSet : public InputSetInternal {
public:
    enum Inputs {
        LEFT_BUTTON = 0,
        MIDDLE_BUTTON,
        RIGHT_BUTTON,
        MOTION_X,
        MOTION_Y,
        WHEEL_X, // This most likely will not be available on most mouses.
        WHEEL_Y,
        TOTAL_BUTTONS
    };
private:
    Input states[ Inputs::TOTAL_BUTTONS ];
public:
    CursorInputSet( std::string name );
    virtual ~CursorInputSet();

    /**
     * This does nothing as the keybindings has been predefined.
     */
    virtual void clearInputs();

    /**
     * This is the accessor for the button states stored within the cursor class.
     * @param which is which button would need to be accessed.
     * @return the pointer to the button state if the button is accessed.
     */
    Input* getInput( unsigned int index ) {
        return InputSetInternal::getInput( index, Inputs::TOTAL_BUTTONS, states );
    }

    /**
     * This is the accessor for the button states stored within the cursor class.
     * @param which is which button would need to be accessed.
     * @return the pointer to the button state if the button is accessed.
     */
    Input *const getInput( unsigned int index ) const {
        return InputSetInternal::getInput( index, Inputs::TOTAL_BUTTONS, const_cast<Input *const>( states ) );
    }
    
    Input* getInput( std::string name ) {
        return InputSetInternal::getInput( name, Inputs::TOTAL_BUTTONS, states );
    }
    
    Input *const getInput( std::string name ) const {
        return InputSetInternal::getInput( name, Inputs::TOTAL_BUTTONS, const_cast<Input *const>( states ) );
    }
};

};

#endif // CONTROLS_CURSOR_INPUT_SET_H
