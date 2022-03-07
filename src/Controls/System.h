#ifndef CONTROLS_CONTROLS_MASTER_H
#define CONTROLS_CONTROLS_MASTER_H

#include "Input.h"
#include "InputSet.h"
#include "CursorInputSet.h"
#include <vector>

namespace Controls {

class System {
private:
    static System *singleton_p;

    /**
     * This declares the entire controls system.
     * @warning do not declare System until System is fully intialized.
     */
    System();

    void *system_internal_data_p;

    // Where all the InputSets are stored.
     std::vector<InputSet*> input_sets;
     CursorInputSet *cursor_p;
public:
    /**
     * This gives the singleton's pointer
     * If the singleton is not allocated yet then this will initialize the abstracted control library used by this control system.
     * @return 1 for success. 0 for failure.
     */
    static System* getSingleton();

    /**
     * This declares the entire controls system.
     * This also deinitializes the abstracted control library.
     * @warning delete System before calling the deinitSystem from this class, or deinitEntireSystem from the graphics Environment.
     */
    virtual ~System();

    /**
     * This actually updates the System.
     * @note It checks what controls were pressed and unpressed. It also updates all of the InputSets.
     * @param seconds_delta The amount of seconds passed.
     */
    void advanceTime( float seconds_delta );

    /**
     * This indicates that the program has been ordered to close for one reason or another.
     * @return true if the program is marked to exit.
     */
    bool isOrderedToExit() const;

    /**
     * This read a json file containing all of the controls.
     * @param filepath The location of the configuration file.
     * @return -1 if the configuration file being read is not compatible, 0 if this operation is not supported, 1 file is successfully read.
     */
    int read( std::string filepath );

    /**
     * This writes a json file containing all of the controls.
     * @param filepath The location of the configuration file.
     * @return -1 if the file cannot be written, 0 if the operation is not suppored, 1 file is successfully written.
     */
    int write( std::string filepath ) const;

    /**
     * Add the InputSet to the System.
     * @note The InputSet would now belong to this class, so it will be deleted by the deletion of this class.
     * @param input_set_p The InputSet interface class to add to the controls.
     * @return A positive number indicating the index for the input_set_p, or simply negative -1 for failure.
     */
    int addInputSet( InputSet *input_set_p );

    /**
     * @return The amount of input sets in the controls.
     */
    unsigned int amountOfInputSets() const { return input_sets.size(); }

    /**
     * Get the InputSet class interface pointer from the this class.
     * @param index The index of the InputSet
     * @return A pointer to the InputSet for success, and nullptr for out of bounds.
     */
    InputSet *getInputSet( unsigned int index );

    /**
     * Get the InputSet class interface pointer from the this class.
     * @note this one is for getting the contant version of InputSet for reading.
     * @param index The index of the InputSet
     * @return A pointer to the InputSet for success, and nullptr for out of bounds.
     */
    InputSet *const getInputSet( unsigned int index ) const;

    /**
     * This method waits for an input to set
     * @param input_set_index The index to the InputSet to change.
     * @param input_index The index to the Input to set the index to the
     * @return 1 if the input in the InputSet was successfully set, 0 if there is no input at this moment, -1 for input conflict, -2 if the input_set_index is out of bounds, or -3 if the input_index is out of bounds.
     */
    int pollEventForInputSet( unsigned int input_set_index, unsigned int input_index );

    /**
     * This allocates a single cursor if one is not allocated yet.
     * @return 1 if the cursor is successfully allocated, 0 if the cursor is already allocated, -1 if the cursor cannot be allocated.
     */
    int allocateCursor();

    /**
     * This gets the cursor from the system.
     */
    CursorInputSet* getCursor() const;

    /**
     * This gets the Control API variables for use in the internal code for the System.
     */
    void* getInternalData() { return system_internal_data_p; }
};

}
#endif // CONTROLS_CONTROLS_MASTER_H
