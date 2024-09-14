#include "Accessor.h"

#include "Mission/ANMResource.h"
#include "Mission/BMPResource.h"
#include "Mission/DCSResource.h"
#include "Mission/FUNResource.h"
#include "Mission/FontResource.h"
#include "Mission/MSICResource.h"
#include "Mission/NetResource.h"
#include "Mission/ObjResource.h"
#include "Mission/PTCResource.h"
#include "Mission/PYRResource.h"
#include "Mission/RPNSResource.h"
#include "Mission/SHDResource.h"
#include "Mission/SNDSResource.h"
#include "Mission/TilResource.h"
#include "Mission/TOSResource.h"
#include "Mission/WAVResource.h"

#include "Mission/ACTResource.h"

#define SEARCH( CLASS_NAME, GET_METHOD_NAME, ALL_METHOD_NAME, GET_CONST_METHOD_NAME, ALL_CONST_METHOD_NAME ) \
\
Mission::CLASS_NAME* Accessor::GET_METHOD_NAME( uint32_t resource_id ) {\
    Mission::Resource *resource_r = nullptr;\
    SearchValue search_value = { Mission::CLASS_NAME::IDENTIFIER_TAG, resource_id };\
\
    auto result = search.find(search_value);\
\
    if( result != search.end() )\
        resource_r = (*result).second.changable_r;\
\
    return dynamic_cast<Mission::CLASS_NAME*>( resource_r );\
}\
const Mission::CLASS_NAME* Accessor::GET_CONST_METHOD_NAME( uint32_t resource_id ) const {\
    const Mission::Resource *resource_r = nullptr;\
    SearchValue search_value = { Mission::CLASS_NAME::IDENTIFIER_TAG, resource_id };\
\
    auto result = search.find(search_value);\
\
    if( result != search.end() )\
        resource_r = (*result).second.constant_r;\
\
    return dynamic_cast<const Mission::CLASS_NAME*>( resource_r );\
}\
std::vector<Mission::CLASS_NAME*> Accessor::ALL_METHOD_NAME() {\
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
std::vector<const Mission::CLASS_NAME*> Accessor::ALL_CONST_METHOD_NAME() const {\
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

bool Accessor::SearchValue::operator< ( const Accessor::SearchValue & operand ) const {
    if( type != operand.type )
        return (type < operand.type);
    else
        return (resource_id < operand.resource_id);
}

void Accessor::emplaceConstant( const Mission::Resource *constant_resource_r ) {
    assert(constant_resource_r != nullptr);

    SearchValue search_value;

    search_value.type = constant_resource_r->getResourceTagID();
    search_value.resource_id = constant_resource_r->getResourceID();

    search[ search_value ] = {nullptr, constant_resource_r};
}

void Accessor::emplace( Mission::Resource *resource_r ) {
    assert(resource_r != nullptr);

    SearchValue search_value;

    search_value.type = resource_r->getResourceTagID();
    search_value.resource_id = resource_r->getResourceID();

    search[ search_value ] = {resource_r, resource_r};
}

Accessor::Accessor() {}

Accessor::~Accessor() {}

void Accessor::loadConstant( const Mission::IFF &iff ) {
    SearchValue search_value;

    for( auto constant_resource_r : iff.getAllResources() ) {
        const Mission::ACTResource *actor_resource_r = dynamic_cast<const Mission::ACTResource*>(constant_resource_r);

        if(actor_resource_r != nullptr)
            actor_accessor.emplaceActorConstant(actor_resource_r);
        else {

            search_value.type = constant_resource_r->getResourceTagID();
            search_value.resource_id = constant_resource_r->getResourceID();

            search[ search_value ] = {nullptr, constant_resource_r};
        }
    }

    if(iff.getMSICResource() != nullptr) {
        auto constant_resource_r = iff.getMSICResource();

        search_value.type = constant_resource_r->getResourceTagID();
        search_value.resource_id = constant_resource_r->getResourceID();

        search[ search_value ] = {nullptr, constant_resource_r};
    }

    auto tos_array = getAllConstTOS();

    if(!tos_array.empty()) {
        for(auto tos_offset : tos_array[0]->getOffsets()) {
            for( auto constant_resource_r : iff.getAllSWVRResources( tos_offset ) ) {
                if(swvr_files.find(tos_offset) == swvr_files.end())
                    swvr_files[tos_offset] = Accessor();

                swvr_files[tos_offset].emplaceConstant(constant_resource_r);
            }
        }
    }
}

void Accessor::load( Mission::IFF &iff ) {
    SearchValue search_value;

    for( auto resource_r : iff.getAllResources() ) {
        Mission::ACTResource *actor_resource_r = dynamic_cast<Mission::ACTResource*>(resource_r);

        if(actor_resource_r != nullptr)
            actor_accessor.emplaceActor(actor_resource_r);
        else {
            search_value.type = resource_r->getResourceTagID();
            search_value.resource_id = resource_r->getResourceID();

            search[ search_value ] = {resource_r, resource_r};
        }
    }

    if(iff.getMSICResource() != nullptr) {
        auto resource_r = iff.getMSICResource();

        search_value.type = resource_r->getResourceTagID();
        search_value.resource_id = resource_r->getResourceID();

        search[ search_value ] = {resource_r, resource_r};
    }

    auto tos_array = getAllConstTOS();

    if(!tos_array.empty()) {
        for(auto tos_offset : tos_array[0]->getOffsets()) {
            for( auto resource_r : iff.getAllSWVRResources( tos_offset ) ) {
                if(swvr_files.find(tos_offset) == swvr_files.end())
                    swvr_files[tos_offset] = Accessor();

                swvr_files[tos_offset].emplace(resource_r);
            }
        }
    }
}

void Accessor::clear() {
    search.clear();
    actor_accessor.clear();
    swvr_files.clear();
}


Accessor* Accessor::getSWVRAccessor(uint32_t tos_offset) {
    if(swvr_files.find(tos_offset) == swvr_files.end())
        return nullptr;
    return &swvr_files[tos_offset];
}

const Accessor* Accessor::getSWVRAccessor(uint32_t tos_offset) const {
    auto swvr_accessor = swvr_files.find(tos_offset);

    if(swvr_accessor == swvr_files.end())
        return nullptr;

    return const_cast<const Accessor*>(&(*swvr_accessor).second);
}

SEARCH(ANMResource,   getANM,  getAllANM, getConstANM, getAllConstANM)
SEARCH(BMPResource,   getBMP,  getAllBMP, getConstBMP, getAllConstBMP)
SEARCH(DCSResource,   getDCS,  getAllDCS, getConstDCS, getAllConstDCS)
SEARCH(FUNResource,   getFUN,  getAllFUN, getConstFUN, getAllConstFUN)
SEARCH(FontResource,  getFNT,  getAllFNT, getConstFNT, getAllConstFNT)
SEARCH(MSICResource, getMSIC, getAllMSIC, getConstMSIC, getAllConstMSIC)
SEARCH(NetResource,   getNET,  getAllNET, getConstNET, getAllConstNET)
SEARCH(ObjResource,   getOBJ,  getAllOBJ, getConstOBJ, getAllConstOBJ)
SEARCH(PTCResource,   getPTC,  getAllPTC, getConstPTC, getAllConstPTC)
SEARCH(PYRResource,   getPYR,  getAllPYR, getConstPYR, getAllConstPYR)
SEARCH(RPNSResource, getRPNS, getAllRPNS, getConstRPNS, getAllConstRPNS)
SEARCH(SNDSResource, getSNDS, getAllSNDS, getConstSNDS, getAllConstSNDS)
SEARCH(SHDResource,   getSHD,  getAllSHD, getConstSHD, getAllConstSHD)
SEARCH(TilResource,   getTIL,  getAllTIL, getConstTIL, getAllConstTIL)
SEARCH(TOSResource,   getTOS,  getAllTOS, getConstTOS, getAllConstTOS)
SEARCH(WAVResource,   getWAV,  getAllWAV, getConstWAV, getAllConstWAV)

}

#undef SEARCH
