#ifndef CONTROLS_INPUT_SET_H
#define CONTROLS_INPUT_SET_H

#include "Input.h"
#include <string>

namespace Controls {

/**
 * This is the interface class for the inputs.
 * This is meant to make the Controls more expandable.
 */
class InputSetInternal {
protected:
    void *input_set_internal_data_p;
    std::string name; // This is useful for configuration file reading and writing.
    bool changed; // This indicates that there is a change to one of the Input classes stored in its successor.

    /**
     * This gets the input however it is up to the successor class to call this method.
     * @param index This is the index to the
     * @param size_of_array This is amount of entries in array_r.
     * @param array_r This is the array itself.
     * @return Either a nullptr, or a valid Input to use in order to access the available button.
     */
    Input* getInput( unsigned int index, unsigned int size_of_array, Input * array_r );

    /**
     * This gets the input however it is up to the successor class to call this method.
     * @param index This is the index to the
     * @param size_of_array This is amount of entries in array_r.
     * @param array_r This is the array itself.
     * @return Either a nullptr, or a valid Input to use in order to access the available button.
     */
    Input *const getInput( unsigned int index, unsigned int size_of_array, Input *const array_r ) const;
    
    
    Input * getInput( std::string name, unsigned int size_of_array, Input *const array_r );
    Input *const getInput( std::string name, unsigned int size_of_array, Input* array_r ) const;
public:
    /**
     * This sets the first defaults for the input set.
     * @param name This is the name of the class.
     */
    InputSetInternal( std::string name );

    /**
     * This will handle the deletion of the InputSetInternal.
     */
    virtual ~InputSetInternal();

    /**
     * This is the accessor for the button states stored within the InputSetInternal class.
     * @param index is index to the Input class would need to be accessed.
     * @return the pointer to the button state if the button is accessed.
     */
    virtual Input* getInput( unsigned int index ) = 0;

    /**
     * This is the accessor for the button states stored within the InputSet class.
     * @param index is index to the Input class would need to be accessed.
     * @return the pointer to the button state if the button is accessed.
     */
    virtual Input *const getInput( unsigned int index ) const = 0;
    
    /**
     * This is the accessor for the button states stored within the InputSetInternal class.
     * @param index is index to the Input class would need to be accessed.
     * @return the pointer to the button state if the button is accessed.
     */
    virtual Input* getInput( std::string name ) = 0;

    /**
     * This is the accessor for the button states stored within the InputSet class.
     * @param index is index to the Input class would need to be accessed.
     * @return the pointer to the button state if the button is accessed.
     */
    virtual Input *const getInput( std::string name ) const = 0;

    /**
     * Indicate if there are changes regarding this InputSet input.
     * @return if there is a change of controls then the InputSet has been changed.
     */
    bool isChanged() const { return changed; }

    std::string getName() const { return name; }

    /**
     * This gets the Control API variables for use in the internal code for the Controls.
     */
    void* getInternalData() { return input_set_internal_data_p; }
};

class InputSet : public InputSetInternal {
public:
    InputSet( std::string name ) : InputSetInternal( name ) {}
};

}

#endif // CONTROLS_INPUT_SET_H
