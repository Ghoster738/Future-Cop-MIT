#include "SkyCaptain.h"

#include "../ObjResource.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::SkyCaptain::TYPE_ID = 37;

Json::Value Data::Mission::ACT::SkyCaptain::makeJson() const {
    Json::Value root = Aircraft::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["uint8_12"] = sky_internal.uint8_12;
    root["ACT"][NAME]["zero_3"] = sky_internal.zero_3;
    root["ACT"][NAME]["zero_4"] = sky_internal.zero_4;
    root["ACT"][NAME]["uint16_21"] = sky_internal.uint16_21;
    root["ACT"][NAME]["uint16_22"] = sky_internal.uint16_22;
    root["ACT"][NAME]["uint8_14"] = sky_internal.uint8_14;
    root["ACT"][NAME]["uint8_15"] = sky_internal.uint8_15;
    root["ACT"][NAME]["uint16_23"] = sky_internal.uint16_23;
    root["ACT"][NAME]["uint32_3"] = sky_internal.uint32_3;
    root["ACT"][NAME]["uint16_24"] = sky_internal.uint16_24;
    root["ACT"][NAME]["uint16_25"] = sky_internal.uint16_25;
    root["ACT"][NAME]["uint16_26"] = sky_internal.uint16_26;
    root["ACT"][NAME]["uint16_27"] = sky_internal.uint16_27;
    root["ACT"][NAME]["uint8_16"] = sky_internal.uint8_16;
    root["ACT"][NAME]["uint8_17"] = sky_internal.uint8_17;
    root["ACT"][NAME]["uint16_28"] = sky_internal.uint16_28;
    root["ACT"][NAME]["uint32_4"] = sky_internal.uint32_4;
    root["ACT"][NAME]["uint16_29"] = sky_internal.uint16_29;
    root["ACT"][NAME]["uint16_30"] = sky_internal.uint16_30;

    return root;
}

bool Data::Mission::ACT::SkyCaptain::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    Aircraft::readBase(data_reader, endian);

    sky_internal.uint8_12 = data_reader.readU8(); // Always 1
    sky_internal.zero_3 = data_reader.readU16( endian ); // Always 0
    sky_internal.zero_4 = data_reader.readU8(); // Always 0
    sky_internal.uint16_21 = data_reader.readU16( endian ); // Always 8
    sky_internal.uint16_22 = data_reader.readU16( endian ); // Always 578
    sky_internal.uint8_14 = data_reader.readU8(); // Always 1
    sky_internal.uint8_15 = data_reader.readU8(); // Always 4
    sky_internal.uint16_23 = data_reader.readU16( endian ); // Always 1
    sky_internal.uint32_3 = data_reader.readU32( endian ); // Always 268439552 or 0x10001000
    sky_internal.uint16_24 = data_reader.readU16( endian ); // Always 6144
    sky_internal.uint16_25 = data_reader.readU16( endian ); // Always 16
    sky_internal.uint16_26 = data_reader.readU16( endian ); // Always 9
    sky_internal.uint16_27 = data_reader.readU16( endian ); // Always 578
    sky_internal.uint8_16 = data_reader.readU8(); // Always 1
    sky_internal.uint8_17 = data_reader.readU8(); // Always 4
    sky_internal.uint16_28 = data_reader.readU16( endian ); // Always 1
    sky_internal.uint32_4 = data_reader.readU32( endian ); // Always 268439552 or 0x10001000
    sky_internal.uint16_29 = data_reader.readU16( endian ); // Always 6144
    sky_internal.uint16_30 = data_reader.readU16( endian ); // Always 16

    return true;
}

Data::Mission::ACT::SkyCaptain::SkyCaptain() {}

Data::Mission::ACT::SkyCaptain::SkyCaptain( const ACTResource& obj ) : Aircraft( obj ) {}

Data::Mission::ACT::SkyCaptain::SkyCaptain( const SkyCaptain& obj ) : Aircraft( obj ), sky_internal( obj.sky_internal ) {}

uint_fast8_t Data::Mission::ACT::SkyCaptain::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::SkyCaptain::getTypeIDName() const {
    return "SkyCaptain";
}

size_t Data::Mission::ACT::SkyCaptain::getSize() const {
    return 100; // bytes
}

Data::Mission::Resource* Data::Mission::ACT::SkyCaptain::duplicate() const {
    return new Data::Mission::ACT::SkyCaptain( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::SkyCaptain::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const SkyCaptain*>( &original );

    if( copy_r != nullptr )
        return new Data::Mission::ACT::SkyCaptain( *copy_r );
    else
        return new Data::Mission::ACT::SkyCaptain( original );
}

Data::Mission::ACT::SkyCaptain::Internal Data::Mission::ACT::SkyCaptain::getSkyInternal() const {
    return sky_internal;
}
