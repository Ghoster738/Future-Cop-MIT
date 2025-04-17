#include "SkyCaptain.h"

#include "../ObjResource.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::SkyCaptain::TYPE_ID = 37;

Json::Value Data::Mission::ACT::SkyCaptain::makeJson() const {
    Json::Value root = Aircraft::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["uint8_0"] = sky_internal.uint8_0;
    root["ACT"][NAME]["zero_0"] = sky_internal.zero_0;
    root["ACT"][NAME]["zero_1"] = sky_internal.zero_1;
    root["ACT"][NAME]["uint16_1"] = sky_internal.uint16_1;
    root["ACT"][NAME]["uint16_2"] = sky_internal.uint16_2;
    root["ACT"][NAME]["uint8_1"] = sky_internal.uint8_1;
    root["ACT"][NAME]["uint8_2"] = sky_internal.uint8_2;
    root["ACT"][NAME]["uint16_3"] = sky_internal.uint16_3;
    root["ACT"][NAME]["uint32_0"] = sky_internal.uint32_0;
    root["ACT"][NAME]["uint16_4"] = sky_internal.uint16_4;
    root["ACT"][NAME]["uint16_5"] = sky_internal.uint16_5;
    root["ACT"][NAME]["uint16_6"] = sky_internal.uint16_6;
    root["ACT"][NAME]["uint16_7"] = sky_internal.uint16_7;
    root["ACT"][NAME]["uint8_3"] = sky_internal.uint8_3;
    root["ACT"][NAME]["uint8_4"] = sky_internal.uint8_4;
    root["ACT"][NAME]["uint16_8"] = sky_internal.uint16_8;
    root["ACT"][NAME]["uint32_1"] = sky_internal.uint32_1;
    root["ACT"][NAME]["uint16_9"] = sky_internal.uint16_9;
    root["ACT"][NAME]["uint16_10"] = sky_internal.uint16_10;

    return root;
}

bool Data::Mission::ACT::SkyCaptain::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    Aircraft::readBase(data_reader, endian);

    sky_internal.uint8_0 = data_reader.readU8(); // Always 1
    sky_internal.zero_0 = data_reader.readU16( endian ); // Always 0
    sky_internal.zero_1 = data_reader.readU8(); // Always 0
    sky_internal.uint16_1 = data_reader.readU16( endian ); // Always 8
    sky_internal.uint16_2 = data_reader.readU16( endian ); // Always 578
    sky_internal.uint8_1 = data_reader.readU8(); // Always 1
    sky_internal.uint8_2 = data_reader.readU8(); // Always 4
    sky_internal.uint16_3 = data_reader.readU16( endian ); // Always 1
    sky_internal.uint32_0 = data_reader.readU32( endian ); // Always 268439552 or 0x10001000
    sky_internal.uint16_4 = data_reader.readU16( endian ); // Always 6144
    sky_internal.uint16_5 = data_reader.readU16( endian ); // Always 16
    sky_internal.uint16_6 = data_reader.readU16( endian ); // Always 9
    sky_internal.uint16_7 = data_reader.readU16( endian ); // Always 578
    sky_internal.uint8_3 = data_reader.readU8(); // Always 1
    sky_internal.uint8_4 = data_reader.readU8(); // Always 4
    sky_internal.uint16_8 = data_reader.readU16( endian ); // Always 1
    sky_internal.uint32_1 = data_reader.readU32( endian ); // Always 268439552 or 0x10001000
    sky_internal.uint16_9 = data_reader.readU16( endian ); // Always 6144
    sky_internal.uint16_10 = data_reader.readU16( endian ); // Always 16

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
