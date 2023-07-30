#include "ACTManager.h"

Data::Mission::ACTManager::ACTManager() {}

Data::Mission::ACTManager::~ACTManager() {}

bool Data::Mission::ACTManager::addACT( Data::Mission::ACTResource *act_r ) {
    if( act_r == nullptr )
        return false;
    if( act_type_map.find( act_r->getID() ) != act_type_map.end() )
        return false;

    act_type_map[ act_r->getTypeID() ].push_back( act_r );

    act_id_map[ act_r->getID() ] = act_r;

    return true;
}

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
