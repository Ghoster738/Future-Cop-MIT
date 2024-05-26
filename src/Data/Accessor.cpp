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
#include "Mission/TOSResource.h"
#include "Mission/WAVResource.h"

#define SEARCH( CLASS_NAME, GET_METHOD_NAME, ALL_METHOD_NAME ) \
\
const Mission::CLASS_NAME* Accessor::GET_METHOD_NAME( uint32_t resource_id ) const {\
    Mission::Resource *resource_r = nullptr;\
    SearchValue search_value = { Mission::CLASS_NAME::IDENTIFIER_TAG, resource_id };\
\
    auto result = search.find(search_value);\
\
    if( result != search.end() )\
        resource_r = (*result).second;\
\
    return dynamic_cast<Mission::CLASS_NAME*>( resource_r );\
}\
std::vector<const Mission::CLASS_NAME*> Accessor::ALL_METHOD_NAME() const {\
    std::vector<const Mission::CLASS_NAME*> array;\
\
    for( auto r_it = search.begin(); r_it != search.end(); r_it++ ) {\
        if( (*r_it).first.type == Mission::CLASS_NAME::IDENTIFIER_TAG ) {\
            assert( dynamic_cast<const Mission::CLASS_NAME*>( (*r_it).second ) );\
            array.emplace_back( dynamic_cast<const Mission::CLASS_NAME*>( (*r_it).second ) );\
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

SEARCH(ANMResource, getConstANM, getAllConstANM)
SEARCH(BMPResource, getConstBMP, getAllConstBMP)
SEARCH(DCSResource, getConstDCS, getAllConstDCS)
SEARCH(FUNResource, getConstFUN, getAllConstFUN)
SEARCH(FontResource, getConstFNT, getAllConstFNT)
SEARCH(MSICResource, getConstMISC, getAllConstMISC)
SEARCH(NetResource, getConstNET, getAllConstNET)
SEARCH(ObjResource, getConstOBJ, getAllConstOBJ)
SEARCH(PTCResource, getConstPTC, getAllConstPTC)
SEARCH(PYRResource, getConstPYR, getAllConstPYR)
SEARCH(RPNSResource, getConstRPNS, getAllConstRPNS)
SEARCH(SNDSResource, getConstSNDS, getAllConstSNDS)
SEARCH(TilResource, getConstTIL, getAllConstTIL)
SEARCH(TOSResource, getConstTOS, getAllConstTOS)
SEARCH(WAVResource, getConstWAV, getAllConstWAV)

}

#undef SEARCH
