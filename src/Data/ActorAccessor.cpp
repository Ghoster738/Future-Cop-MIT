#include "ActorAccessor.h"

#include "Mission/ACT/Aircraft.h"
#include "Mission/ACT/BaseTurret.h"
#include "Mission/ACT/ItemPickup.h"
#include "Mission/ACT/NeutralTurret.h"
#include "Mission/ACT/Prop.h"
#include "Mission/ACT/SkyCaptain.h"
#include "Mission/ACT/Trigger.h"
#include "Mission/ACT/X1Alpha.h"

#define SEARCH( CLASS_NAME, GET_METHOD_NAME, ALL_METHOD_NAME, GET_CONST_METHOD_NAME, ALL_CONST_METHOD_NAME ) \
\
Mission::ACT::CLASS_NAME* ActorAccessor::GET_METHOD_NAME( uint32_t resource_id ) {\
    Mission::Resource *resource_r = nullptr;\
    SearchValue search_value = { Mission::ACT::CLASS_NAME::TYPE_ID, resource_id };\
\
    auto result = search.find(search_value);\
\
    if( result != search.end() )\
        resource_r = (*result).second.changable_r;\
\
    return dynamic_cast<Mission::ACT::CLASS_NAME*>( resource_r );\
}\
const Mission::ACT::CLASS_NAME* ActorAccessor::GET_CONST_METHOD_NAME( uint32_t resource_id ) const {\
    const Mission::Resource *resource_r = nullptr;\
    SearchValue search_value = { Mission::ACT::CLASS_NAME::TYPE_ID, resource_id };\
\
    auto result = search.find(search_value);\
\
    if( result != search.end() )\
        resource_r = (*result).second.constant_r;\
\
    return dynamic_cast<const Mission::ACT::CLASS_NAME*>( resource_r );\
}\
std::vector<Mission::ACT::CLASS_NAME*> ActorAccessor::ALL_METHOD_NAME() {\
    std::vector<Mission::ACT::CLASS_NAME*> array;\
\
    for( auto r_it = search.begin(); r_it != search.end(); r_it++ ) {\
        if( (*r_it).first.actor_type == Mission::ACT::CLASS_NAME::TYPE_ID ) {\
            assert( dynamic_cast<Mission::ACT::CLASS_NAME*>( (*r_it).second.changable_r ) );\
            array.emplace_back( dynamic_cast<Mission::ACT::CLASS_NAME*>( (*r_it).second.changable_r ) );\
        }\
    }\
\
    return array;\
}\
std::vector<const Mission::ACT::CLASS_NAME*> ActorAccessor::ALL_CONST_METHOD_NAME() const {\
    std::vector<const Mission::ACT::CLASS_NAME*> array;\
\
    for( auto r_it = search.begin(); r_it != search.end(); r_it++ ) {\
        if( (*r_it).first.actor_type == Mission::ACT::CLASS_NAME::TYPE_ID ) {\
            assert( dynamic_cast<const Mission::ACT::CLASS_NAME*>( (*r_it).second.constant_r ) );\
            array.emplace_back( dynamic_cast<const Mission::ACT::CLASS_NAME*>( (*r_it).second.constant_r ) );\
        }\
    }\
\
    return array;\
}

namespace Data {

bool ActorAccessor::SearchValue::operator< ( const ActorAccessor::SearchValue & operand ) const {
    if( actor_type != operand.actor_type )
        return (actor_type < operand.actor_type);
    else
        return (resource_id < operand.resource_id);
}

ActorAccessor::ActorAccessor() {
}

ActorAccessor::~ActorAccessor() {}

void ActorAccessor::emplaceActorConstant( const Mission::ACTResource *constant_resource_r ) {
    SearchValue search_value;

    search_value.actor_type = constant_resource_r->getTypeID();
    search_value.resource_id = constant_resource_r->getResourceID();

    search[ search_value ] = {nullptr, constant_resource_r};
}

void ActorAccessor::emplaceActor( Mission::ACTResource *resource_r ) {
    SearchValue search_value;

    search_value.actor_type = resource_r->getTypeID();
    search_value.resource_id = resource_r->getResourceID();

    search[ search_value ] = {resource_r, resource_r};
}

void ActorAccessor::clear() {
    search.clear();
}

std::vector<Mission::ACTResource*> ActorAccessor::getAll() {
    std::vector<Mission::ACTResource*> array;

    for( auto r_it = search.begin(); r_it != search.end(); r_it++ ) {
        if((*r_it).second.changable_r != nullptr)
            array.emplace_back( (*r_it).second.changable_r );
    }

    return array;
}

std::vector<const Mission::ACTResource*> ActorAccessor::getAllConst() const {
    std::vector<const Mission::ACTResource*> array;

    for( auto r_it = search.begin(); r_it != search.end(); r_it++ ) {
        if((*r_it).second.changable_r != nullptr)
            array.emplace_back( (*r_it).second.changable_r );
    }

    return array;
}

std::vector<Mission::ACTResource*> ActorAccessor::getAllTypeID( uint_fast8_t type_id ) {
    std::vector<Mission::ACTResource*> array;

    for( auto r_it = search.begin(); r_it != search.end(); r_it++ ) {
        if( (*r_it).first.actor_type == type_id ) {
            array.emplace_back( (*r_it).second.changable_r );
        }
    }

    return array;
}

std::vector<const Mission::ACTResource*> ActorAccessor::getAllConstTypeID( uint_fast8_t type_id ) const {
    std::vector<const Mission::ACTResource*> array;

    for( auto r_it = search.begin(); r_it != search.end(); r_it++ ) {
        if( (*r_it).first.actor_type == type_id ) {
            array.emplace_back( (*r_it).second.constant_r );
        }
    }

    return array;
}

SEARCH(Aircraft, getAircraft, getAllAircraft, getConstAircraft, getAllConstAircraft)
SEARCH(BaseTurret, getBaseTurret, getAllBaseTurret, getConstBaseTurret, getAllConstBaseTurret)
SEARCH(ItemPickup, getItemPickup, getAllItemPickup, getConstItemPickup, getAllConstItemPickup)
SEARCH(NeutralTurret, getNeutralTurret, getAllNeutralTurret, getConstNeutralTurret, getAllConstNeutralTurret)
SEARCH(Prop, getProp, getAllProp, getConstProp, getAllConstProp)
SEARCH(SkyCaptain, getSkyCaptain, getAllSkyCaptain, getConstSkyCaptain, getAllConstSkyCaptain)
SEARCH(Trigger, getTrigger, getAllTrigger, getConstTrigger, getAllConstTrigger)
SEARCH(X1Alpha, getX1Alpha, getAllX1Alpha, getConstX1Alpha, getAllConstX1Alpha)

}

#undef SEARCH
