#include "SWVRAccessor.h"

#include "Mission/ANMResource.h"
#include "Mission/MSICResource.h"
#include "Mission/SNDSResource.h"

#include <cassert>

#define SEARCH( CLASS_NAME, GET_METHOD_NAME, ALL_METHOD_NAME, GET_CONST_METHOD_NAME, ALL_CONST_METHOD_NAME ) \
\
Mission::CLASS_NAME* SWVRAccessor::GET_METHOD_NAME( uint32_t tos_offset ) {\
    Mission::Resource *resource_r = nullptr;\
    SearchValue search_value = { Mission::CLASS_NAME::IDENTIFIER_TAG, tos_offset };\
\
    auto result = search.find(search_value);\
\
    if( result != search.end() )\
        resource_r = (*result).second.changable_r;\
\
    return dynamic_cast<Mission::CLASS_NAME*>( resource_r );\
}\
const Mission::CLASS_NAME* SWVRAccessor::GET_CONST_METHOD_NAME( uint32_t tos_offset ) const {\
    const Mission::Resource *resource_r = nullptr;\
    SearchValue search_value = { Mission::CLASS_NAME::IDENTIFIER_TAG, tos_offset };\
\
    auto result = search.find(search_value);\
\
    if( result != search.end() )\
        resource_r = (*result).second.constant_r;\
\
    return dynamic_cast<const Mission::CLASS_NAME*>( resource_r );\
}\
std::vector<Mission::CLASS_NAME*> SWVRAccessor::ALL_METHOD_NAME() {\
    std::vector<Mission::CLASS_NAME*> array;\
\
    for( auto r_it = search.begin(); r_it != search.end(); r_it++ ) {\
        if( (*r_it).first.type == Mission::CLASS_NAME::IDENTIFIER_TAG ) {\
            assert( dynamic_cast<Mission::CLASS_NAME*>( (*r_it).second.changable_r ) );\
            array.emplace_back( dynamic_cast<Mission::CLASS_NAME*>( (*r_it).second.changable_r ) );\
        }\
    }\
\
    return array;\
}\
std::vector<const Mission::CLASS_NAME*> SWVRAccessor::ALL_CONST_METHOD_NAME() const {\
    std::vector<const Mission::CLASS_NAME*> array;\
\
    for( auto r_it = search.begin(); r_it != search.end(); r_it++ ) {\
        if( (*r_it).first.type == Mission::CLASS_NAME::IDENTIFIER_TAG ) {\
            assert( dynamic_cast<const Mission::CLASS_NAME*>( (*r_it).second.constant_r ) );\
            array.emplace_back( dynamic_cast<const Mission::CLASS_NAME*>( (*r_it).second.constant_r ) );\
        }\
    }\
\
    return array;\
}

namespace Data {

bool SWVRAccessor::SearchValue::operator< ( const SWVRAccessor::SearchValue & operand ) const {
    if( type != operand.type )
        return (type < operand.type);
    else
        return (tos_offset < operand.tos_offset);
}

SWVRAccessor::SWVRAccessor() : msic_resource({nullptr, nullptr}) {}

SWVRAccessor::~SWVRAccessor() {}

void SWVRAccessor::emplaceConstant( const Mission::Resource *constant_resource_r ) {
    assert(constant_resource_r != nullptr);
    assert(constant_resource_r->getSWVREntry().isPresent());

    if(dynamic_cast<const Mission::MSICResource*>(constant_resource_r) != nullptr) {
        msic_resource.changable_r = nullptr;
        msic_resource.constant_r = dynamic_cast<const Mission::MSICResource*>(constant_resource_r);
    }

    SearchValue search_value;

    search_value.type = constant_resource_r->getResourceTagID();
    search_value.tos_offset = constant_resource_r->getSWVREntry().tos_offset;

    search[ search_value ] = {nullptr, constant_resource_r};
}

void SWVRAccessor::emplace( Mission::Resource *resource_r ) {
    assert(resource_r != nullptr);
    assert(resource_r->getSWVREntry().isPresent());

    if(dynamic_cast<Mission::MSICResource*>(resource_r) != nullptr) {
        msic_resource.changable_r = dynamic_cast<Mission::MSICResource*>(resource_r);
        msic_resource.constant_r = msic_resource.changable_r;
    }

    SearchValue search_value;

    search_value.type = resource_r->getResourceTagID();
    search_value.tos_offset = resource_r->getSWVREntry().tos_offset;

    search[ search_value ] = {resource_r, resource_r};
}

void SWVRAccessor::clear() {
    search.clear();
    msic_resource.changable_r = nullptr;
    msic_resource.constant_r = nullptr;
}

std::vector<Mission::Resource*> SWVRAccessor::getAll() {
    std::vector<Mission::Resource*> array;

    for( auto r_it = search.begin(); r_it != search.end(); r_it++ ) {
        if((*r_it).second.changable_r != nullptr)
            array.emplace_back( (*r_it).second.changable_r );
    }

    return array;
}
std::vector<const Mission::Resource*> SWVRAccessor::getAllConst() const {
    std::vector<const Mission::Resource*> array;

    for( auto r_it = search.begin(); r_it != search.end(); r_it++ ) {
        if((*r_it).second.constant_r != nullptr)
            array.emplace_back( (*r_it).second.constant_r );
    }

    return array;
}

std::vector<Mission::Resource*> SWVRAccessor::getAllType( uint32_t type ) {
    std::vector<Mission::Resource*> array;

    for( auto r_it = search.begin(); r_it != search.end(); r_it++ ) {
        if((*r_it).second.changable_r != nullptr && (*r_it).second.changable_r->getResourceTagID() == type)
            array.emplace_back( (*r_it).second.changable_r );
    }

    return array;
}
std::vector<const Mission::Resource*> SWVRAccessor::getAllConstType( uint32_t type ) const {
    std::vector<const Mission::Resource*> array;

    for( auto r_it = search.begin(); r_it != search.end(); r_it++ ) {
        if((*r_it).second.constant_r != nullptr && (*r_it).second.constant_r->getResourceTagID() == type)
            array.emplace_back( (*r_it).second.constant_r );
    }

    return array;
}

SEARCH(ANMResource,   getANM,  getAllANM, getConstANM,  getAllConstANM)
SEARCH(SNDSResource, getSNDS, getAllSNDS, getConstSNDS, getAllConstSNDS)

}

#undef SEARCH
