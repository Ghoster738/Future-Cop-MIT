#include "ACTManager.h"

Data::Mission::ACTManager::ACTManager( const std::vector<ACTResource*> &acts_array ) {
    for( auto actor_r : acts_array ) {
        act_type_map[ actor_r->getTypeID() ].push_back( actor_r );

        act_id_map[ actor_r->getID() ] = actor_r;
    }
}

Data::Mission::ACTManager::~ACTManager() {}

std::vector<Data::Mission::ACTResource*> Data::Mission::ACTManager::getACTs( uint_fast8_t type ) {
    // O( log n ) for n = the amount of ID's.
    auto map_it = act_type_map.find( type );

    if( map_it != act_type_map.end() )
        return (*map_it).second;
    else
        return std::vector<Data::Mission::ACTResource*>();
}

const std::vector<Data::Mission::ACTResource*> Data::Mission::ACTManager::getACTs( uint_fast8_t type ) const {
    return const_cast<Data::Mission::ACTManager*>( this )->getACTs( type );
}

Data::Mission::ACTResource* Data::Mission::ACTManager::getACTID( uint_fast32_t id ) {
    // O( log n ) for n = the amount of ID's.
    auto map_it = act_id_map.find( id );

    if( map_it != act_id_map.end() )
        return (*map_it).second;
    else
        return nullptr;
}

const Data::Mission::ACTResource* Data::Mission::ACTManager::getACTID( uint_fast32_t id ) const {
    return const_cast<Data::Mission::ACTManager*>( this )->getACTID( id );
}
