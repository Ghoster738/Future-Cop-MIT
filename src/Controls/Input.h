#ifndef CONTROLS_INPUT_H
#define CONTROLS_INPUT_H

#include <string>

namespace Controls {

/**
 * This class stores the Input from the keyboard, or mouse, or gamepade, etc.
 */
class Input {
protected:
    void *input_internals_p;
    std::string name; // This is useful for configuration file reading and writing.
public:
    Input();

    /**
     * When the Input is no longer needed, this will be called.
     */
    virtual ~Input();

    /**
     * @param name This is the utf-8 name for the input.
     * @param global_change This is the pointer to the InputSet boolean.
     */
    void declare( std::string name, bool *global_change );

    /**
     * If there is a change in state for this input, this will return true.
     * @return True if there is a change to the member variable state.
     */
    bool isChanged() const;

    /**
     * This will return true if the state is changed.
     * @return The state from the Input.
     */
    float getState() const;

    /**
     * @return Get the name of the Input
     */
    std::string getName() const { return name; }

    /**
     * This gets the Control API variables for use in the internal code for the Controls.
     */
    void* getInternalData() { return input_internals_p; }
};

};

#endif // CONTROLS_INPUT_H
