#ifndef MISSION_ACT_MANAGER_HEADER
#define MISSION_ACT_MANAGER_HEADER

#include <map>
#include <vector>
#include "Resource.h"
#include "ACTResource.h"

namespace Data {

namespace Mission {

class ACTManager {
protected:
    // This holds an array of types holding the act resources.
    std::map<uint_fast8_t, std::vector<ACTResource*>> act_type_map;

    // This holds an array of identification numbers for the given act resource.
    // TODO Check if there will be no conflicting IDs.
    std::map<uint_fast32_t, ACTResource*> act_id_map;
public:
    ACTManager( const std::vector<ACTResource*> &acts_array );
    virtual ~ACTManager();

    std::vector<ACTResource*> getACTs( uint_fast8_t type );
    const std::vector<ACTResource*> getACTs( uint_fast8_t type ) const;

    ACTResource* getACTID( uint_fast32_t type );
    const ACTResource* getACTID( uint_fast32_t type ) const;
};

}

}

#endif
