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

namespace Data {

bool Accessor::SearchValue::operator< ( const Accessor::SearchValue & operand ) const {
    if( type != operand.type )
        return (type < operand.type);
    else
        return (resource_id < operand.resource_id);
}

Accessor::Accessor( std::vector<Mission::IFF*> resources ) {
    SearchValue search_value;
    std::vector<Mission::Resource*> array;

    for( auto it = resources.begin(); it != resources.end(); it++ ) {
        array = (*it)->getAllResources();

        for( auto r_it = array.begin(); r_it != array.end(); r_it++ ) {
            search_value.type = (*r_it)->getResourceTagID();
            search_value.resource_id = (*r_it)->getResourceID();

            search[ search_value ] = (*r_it);
        }
    }
}

Accessor::~Accessor() {}

Mission::ANMResource* Accessor::getANM( uint32_t resource_id ) const {

}

Mission::BMPResource* Accessor::getBMP( uint32_t resource_id ) const {

}

Mission::DCSResource* Accessor::getDCS( uint32_t resource_id ) const {

}

Mission::FUNResource* Accessor::getFUN( uint32_t resource_id ) const {

}

Mission::FontResource* Accessor::getFNT( uint32_t resource_id ) const {

}

Mission::MSICResource* Accessor::getMISC( uint32_t resource_id ) const {

}

Mission::NetResource* Accessor::getNET( uint32_t resource_id ) const {

}

Mission::ObjResource* Accessor::getOBJ( uint32_t resource_id ) const {
    Mission::Resource *resource_r = nullptr;
    SearchValue search_value = { Mission::ObjResource::IDENTIFIER_TAG, resource_id };

    auto result = search.find(search_value);

    if( result != search.end() ) {
        resource_r = (*result).second;
    }

    return dynamic_cast<Mission::ObjResource*>( resource_r );
}

Mission::PTCResource* Accessor::getPTC( uint32_t resource_id ) const {

}

Mission::PYRResource* Accessor::getPYR( uint32_t resource_id ) const {

}

Mission::RPNSResource* Accessor::getRPNS( uint32_t resource_id ) const {

}

Mission::SNDSResource* Accessor::getSNDS( uint32_t resource_id ) const {

}

Mission::TilResource* Accessor::getTIL( uint32_t resource_id ) const {

}

Mission::WAVResource* Accessor::getWAV( uint32_t resource_id ) const {

}


}
