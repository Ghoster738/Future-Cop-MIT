#include "SkyCaptain.h"

#include "../ObjResource.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::SkyCaptain::TYPE_ID = 37;

Json::Value Data::Mission::ACT::SkyCaptain::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["uint32_0"] = internal.uint32_0;
    root["ACT"][NAME]["uint16_0"] = internal.uint16_0;
    root["ACT"][NAME]["zero_0"] = internal.zero_0;
    root["ACT"][NAME]["uint8_0"] = internal.uint8_0;
    root["ACT"][NAME]["zero_1"] = internal.zero_1;
    root["ACT"][NAME]["uint8_2"] = internal.uint8_2;
    root["ACT"][NAME]["uint8_3"] = internal.uint8_3;
    root["ACT"][NAME]["uint8_4"] = internal.uint8_4;
    root["ACT"][NAME]["uint8_5"] = internal.uint8_5;
    root["ACT"][NAME]["zero_2"] = internal.zero_2;
    root["ACT"][NAME]["uint16_3"] = internal.uint16_3;
    root["ACT"][NAME]["uint16_4"] = internal.uint16_4;
    root["ACT"][NAME]["uint8_6"] = internal.uint8_6;
    root["ACT"][NAME]["uint8_7"] = internal.uint8_7;
    root["ACT"][NAME]["uint16_5"] = internal.uint16_5;
    root["ACT"][NAME]["uint32_1"] = internal.uint32_1;
    root["ACT"][NAME]["uint16_6"] = internal.uint16_6;
    root["ACT"][NAME]["uint16_7"] = internal.uint16_7;
    root["ACT"][NAME]["uint8_8"] = internal.uint8_8;
    root["ACT"][NAME]["uint8_9"] = internal.uint8_9;
    root["ACT"][NAME]["uint16_8"] = internal.uint16_8;
    root["ACT"][NAME]["uint8_10"] = internal.uint8_10;
    root["ACT"][NAME]["position_bitfield"] = internal.position_bitfield;
    root["ACT"][NAME]["uint16_9"] = internal.uint16_9;
    root["ACT"][NAME]["uint16_10"] = internal.uint16_10;
    root["ACT"][NAME]["uint16_11"] = internal.uint16_11;
    root["ACT"][NAME]["uint16_12"] = internal.uint16_12;
    root["ACT"][NAME]["uint16_13"] = internal.uint16_13;
    root["ACT"][NAME]["uint16_14"] = internal.uint16_14;
    root["ACT"][NAME]["uint16_15"] = internal.uint16_15;
    root["ACT"][NAME]["uint32_2"] = internal.uint32_2;
    root["ACT"][NAME]["uint16_16"] = internal.uint16_16;
    root["ACT"][NAME]["uint16_17"] = internal.uint16_17;
    root["ACT"][NAME]["alt_position_x"] = internal.alt_position_x;
    root["ACT"][NAME]["alt_position_y"] = internal.alt_position_y;
    root["ACT"][NAME]["uint8_12"] = internal.uint8_12;
    root["ACT"][NAME]["zero_3"] = internal.zero_3;
    root["ACT"][NAME]["zero_4"] = internal.zero_4;
    root["ACT"][NAME]["uint16_21"] = internal.uint16_21;
    root["ACT"][NAME]["uint16_22"] = internal.uint16_22;
    root["ACT"][NAME]["uint8_14"] = internal.uint8_14;
    root["ACT"][NAME]["uint8_15"] = internal.uint8_15;
    root["ACT"][NAME]["uint16_23"] = internal.uint16_23;
    root["ACT"][NAME]["uint32_3"] = internal.uint32_3;
    root["ACT"][NAME]["uint16_24"] = internal.uint16_24;
    root["ACT"][NAME]["uint16_25"] = internal.uint16_25;
    root["ACT"][NAME]["uint16_26"] = internal.uint16_26;
    root["ACT"][NAME]["uint16_27"] = internal.uint16_27;
    root["ACT"][NAME]["uint8_16"] = internal.uint8_16;
    root["ACT"][NAME]["uint8_17"] = internal.uint8_17;
    root["ACT"][NAME]["uint16_28"] = internal.uint16_28;
    root["ACT"][NAME]["uint32_4"] = internal.uint32_4;
    root["ACT"][NAME]["uint16_29"] = internal.uint16_29;
    root["ACT"][NAME]["uint16_30"] = internal.uint16_30;

    return root;
}

bool Data::Mission::ACT::SkyCaptain::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    internal.uint32_0 = data_reader.readU32( endian ); // Always 598856
    internal.uint16_0 = data_reader.readU16( endian ); // Always 1000
    internal.zero_0 = data_reader.readU16( endian ); // Always 0
    internal.uint8_0 = data_reader.readU8(); // Always 2
    internal.zero_1 = data_reader.readU8(); // Always 0
    internal.uint8_2 = data_reader.readU8(); // Always 50
    internal.uint8_3 = data_reader.readU8(); // Always 5
    internal.uint8_4 = data_reader.readU8(); // Values: 220, 221, 222, 223, 
    internal.uint8_5 = data_reader.readU8(); // Always 77
    internal.zero_2 = data_reader.readU16( endian ); // Always 0
    internal.uint16_3 = data_reader.readU16( endian ); // Always 7
    internal.uint16_4 = data_reader.readU16( endian ); // Always 578
    internal.uint8_6 = data_reader.readU8(); // Always 1
    internal.uint8_7 = data_reader.readU8(); // Always 4
    internal.uint16_5 = data_reader.readU16( endian ); // Always 1
    internal.uint32_1 = data_reader.readU32( endian ); // Always 268439552 or 0x10001000
    internal.uint16_6 = data_reader.readU16( endian ); // Always 6144
    internal.uint16_7 = data_reader.readU16( endian ); // Always 16
    internal.uint8_8 = data_reader.readU8(); // Always 1
    internal.uint8_9 = data_reader.readU8(); // Always 4
    internal.uint16_8 = data_reader.readU16( endian ); // Always 1
    internal.uint8_10 = data_reader.readU8(); // Always 1
    internal.position_bitfield = data_reader.readU8(); // Values: 0, 8, 12,
    internal.uint16_9 = data_reader.readU16( endian ); // Always 32358
    internal.uint16_10 = data_reader.readU16( endian ); // Always 512
    internal.uint16_11 = data_reader.readU16( endian ); // Values: 614, 675, 
    internal.uint16_12 = data_reader.readU16( endian ); // Values: 655, 716, 
    internal.uint16_13 = data_reader.readU16( endian ); // Values: 716, 757, 
    internal.uint16_14 = data_reader.readU16( endian ); // Always 4096
    internal.uint16_15 = data_reader.readU16( endian ); // Always 1843
    internal.uint32_2 = data_reader.readU32( endian ); // Always 268439552 or 0x10001000
    internal.uint16_16 = data_reader.readU16( endian ); // Always 7680
    internal.uint16_17 = data_reader.readU16( endian ); // Always 1280
    internal.alt_position_x = data_reader.readU16( endian ); // Values: 0, 1920, 2049, 3303,
    internal.alt_position_y = data_reader.readU16( endian ); // Values: 0, 1912, 2799, 3537,
    internal.uint8_12 = data_reader.readU8(); // Always 1
    internal.zero_3 = data_reader.readU16( endian ); // Always 0
    internal.zero_4 = data_reader.readU8(); // Always 0
    internal.uint16_21 = data_reader.readU16( endian ); // Always 8
    internal.uint16_22 = data_reader.readU16( endian ); // Always 578
    internal.uint8_14 = data_reader.readU8(); // Always 1
    internal.uint8_15 = data_reader.readU8(); // Always 4
    internal.uint16_23 = data_reader.readU16( endian ); // Always 1
    internal.uint32_3 = data_reader.readU32( endian ); // Always 268439552 or 0x10001000
    internal.uint16_24 = data_reader.readU16( endian ); // Always 6144
    internal.uint16_25 = data_reader.readU16( endian ); // Always 16
    internal.uint16_26 = data_reader.readU16( endian ); // Always 9
    internal.uint16_27 = data_reader.readU16( endian ); // Always 578
    internal.uint8_16 = data_reader.readU8(); // Always 1
    internal.uint8_17 = data_reader.readU8(); // Always 4
    internal.uint16_28 = data_reader.readU16( endian ); // Always 1
    internal.uint32_4 = data_reader.readU32( endian ); // Always 268439552 or 0x10001000
    internal.uint16_29 = data_reader.readU16( endian ); // Always 6144
    internal.uint16_30 = data_reader.readU16( endian ); // Always 16

    return true;
}

Data::Mission::ACT::SkyCaptain::SkyCaptain() {}

Data::Mission::ACT::SkyCaptain::SkyCaptain( const ACTResource& obj ) : ACTResource( obj ) {}

Data::Mission::ACT::SkyCaptain::SkyCaptain( const SkyCaptain& obj ) : ACTResource( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::SkyCaptain::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::SkyCaptain::getTypeIDName() const {
    return "SkyCaptain";
}

size_t Data::Mission::ACT::SkyCaptain::getSize() const {
    return 100; // bytes
}

bool Data::Mission::ACT::SkyCaptain::checkRSL() const {
    return rsl_data.size() == 2 && rsl_data[0].type == Data::Mission::ObjResource::IDENTIFIER_TAG && rsl_data[1].type == RSL_NULL_TAG;
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

Data::Mission::ACT::SkyCaptain::Internal Data::Mission::ACT::SkyCaptain::getInternal() const {
    return internal;
}

glm::vec2 Data::Mission::ACT::SkyCaptain::getSpawnPosition() const {
    if((internal.position_bitfield & 0x08) == 0)
        return Data::Mission::ACTResource::getPosition();

    return (1.f / 16.f) * glm::vec2(internal.alt_position_x, internal.alt_position_y);
}
