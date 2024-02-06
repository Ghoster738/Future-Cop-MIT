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
#include "Mission/SNDSResource.h"
#include "Mission/TilResource.h"
#include "Mission/WAVResource.h"

#define SEARCH( CLASS_NAME, METHOD_NAME ) \
Mission::CLASS_NAME* Accessor::METHOD_NAME( uint32_t resource_id ) const {\
    Mission::Resource *resource_r = nullptr;\
    SearchValue search_value = { Mission::CLASS_NAME::IDENTIFIER_TAG, resource_id };\
\
    auto result = search.find(search_value);\
\
    if( result != search.end() )\
        resource_r = (*result).second;\
\
    return dynamic_cast<Mission::CLASS_NAME*>( resource_r );\
}

namespace Data {

bool Accessor::SearchValue::operator< ( const Accessor::SearchValue & operand ) const {
    if( type != operand.type )
        return (type < operand.type);
    else
        return (resource_id < operand.resource_id);
}

Accessor::Accessor() {
}

Accessor::~Accessor() {}

void Accessor::load( Mission::IFF &resource_r ) {
    SearchValue search_value;
    std::vector<Mission::Resource*> array = resource_r.getAllResources();

    for( auto r_it = array.begin(); r_it != array.end(); r_it++ ) {
        search_value.type = (*r_it)->getResourceTagID();
        search_value.resource_id = (*r_it)->getResourceID();

        search[ search_value ] = (*r_it);
    }
}

void Accessor::clear() {
    search.clear();
}

SEARCH(ANMResource, getANM)
SEARCH(BMPResource, getBMP)
SEARCH(DCSResource, getDCS)
SEARCH(FUNResource, getFUN)
SEARCH(FontResource, getFNT)
SEARCH(MSICResource, getMISC)
SEARCH(NetResource, getNET)
SEARCH(ObjResource, getOBJ)
SEARCH(PTCResource, getPTC)
SEARCH(PYRResource, getPYR)
SEARCH(RPNSResource, getRPNS)
SEARCH(SNDSResource, getSNDS)
SEARCH(TilResource, getTIL)
SEARCH(WAVResource, getWAV)

}

#undef SEARCH
