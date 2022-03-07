#include "InputSet.h"
#include "../InputSet.h"

Controls::Input* Controls::InputSetInternal::getInput( unsigned int index, unsigned int size_of_array, Controls::Input *array_r ) {
    if( index < size_of_array )
        return array_r + index;
    else
        return nullptr;
}

Controls::Input *const Controls::InputSetInternal::getInput( unsigned int index, unsigned int size_of_array, Controls::Input *const array_r ) const {
    if( index < size_of_array )
        return array_r + index;
    else
        return nullptr;
}

Controls::InputSetInternal::InputSetInternal( std::string name_param ) : input_set_internal_data_p( nullptr ), name( name_param ), changed( false ) {
}

Controls::InputSetInternal::~InputSetInternal() {
    // Do nothing it is up to the successor to delete input_set_internal_data_p
}
