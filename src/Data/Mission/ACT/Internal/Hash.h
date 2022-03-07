#ifndef DATA_MISSION_ACT_HASH_H
#define DATA_MISSION_ACT_HASH_H

#include "../../ACTResource.h"

namespace Data {
namespace Mission {
namespace ACT {
namespace Hash {

ACTResource* generateAct( const ACTResource *const obj_r, uint_fast8_t Type_ID );
bool isActValid( uint_fast8_t Type_ID );

} // Hash
} // ACT
} // Mission
} // Data

#endif // DATA_MISSION_ACT_HASH_H
