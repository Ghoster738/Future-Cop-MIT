#include "NeutralTurret.h"

#include "../ObjResource.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::NeutralTurret::TYPE_ID = 36;

Json::Value Data::Mission::ACT::NeutralTurret::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["uint32_0"] = internal.uint32_0;
    root["ACT"][NAME]["uint16_0"] = internal.uint16_0;
    root["ACT"][NAME]["uint16_1"] = internal.uint16_1;
    root["ACT"][NAME]["uint8_0"] = internal.uint8_0;
    root["ACT"][NAME]["uint8_1"] = internal.uint8_1;
    root["ACT"][NAME]["uint8_2"] = internal.uint8_2;
    root["ACT"][NAME]["uint8_3"] = internal.uint8_3;
    root["ACT"][NAME]["uint8_4"] = internal.uint8_4;
    root["ACT"][NAME]["uint16_2"] = internal.uint16_2;
    root["ACT"][NAME]["uint8_5"] = internal.uint8_5;
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
    root["ACT"][NAME]["value_0"] = internal.value_0;
    root["ACT"][NAME]["uint16_10"] = internal.uint16_10;
    root["ACT"][NAME]["uint16_11"] = internal.uint16_11;
    root["ACT"][NAME]["uint16_12"] = internal.uint16_12;
    root["ACT"][NAME]["uint8_10"] = internal.uint8_10;
    root["ACT"][NAME]["uint32_2"] = internal.uint32_2;
    root["ACT"][NAME]["uint8_11"] = internal.uint8_11;
    root["ACT"][NAME]["value_1"] = internal.value_1;
    root["ACT"][NAME]["uint8_12"] = internal.uint8_12;
    root["ACT"][NAME]["uint8_13"] = internal.uint8_13;
    root["ACT"][NAME]["uint8_14"] = internal.uint8_14;
    root["ACT"][NAME]["uint8_15"] = internal.uint8_15;
    root["ACT"][NAME]["value_2"] = internal.value_2;
    root["ACT"][NAME]["value_3"] = internal.value_3;
    root["ACT"][NAME]["uint16_14"] = internal.uint16_14;

    return root;
}

bool Data::Mission::ACT::NeutralTurret::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    internal.uint32_0 = data_reader.readU32( endian ); // Always 1114440
    internal.uint16_0 = data_reader.readU16( endian ); // Always 500
    internal.uint16_1 = data_reader.readU16( endian ); // Always 0
    internal.uint8_0 = data_reader.readU8(); // Always 0
    internal.uint8_1 = data_reader.readU8(); // Always 50
    internal.uint8_2 = data_reader.readU8(); // Always 3
    internal.uint8_3 = data_reader.readU8(); // Always 0
    internal.uint8_4 = data_reader.readU8(); // Always 101
    internal.uint16_2 = data_reader.readU16( endian ); // Always 0
    internal.uint8_5 = data_reader.readU8(); // Always 0
    internal.uint16_3 = data_reader.readU16( endian ); // Always 3
    internal.uint16_4 = data_reader.readU16( endian ); // Always 518
    internal.uint8_6 = data_reader.readU8(); // Always 3
    internal.uint8_7 = data_reader.readU8(); // Always 4
    internal.uint16_5 = data_reader.readU16( endian ); // Always 10
    internal.uint32_1 = data_reader.readU32( endian ); // Always 268439552 or 0x10001000
    internal.uint16_6 = data_reader.readU16( endian ); // Always 6144
    internal.uint16_7 = data_reader.readU16( endian ); // Always 32
    internal.uint8_8 = data_reader.readU8(); // Always 0
    internal.uint8_9 = data_reader.readU8(); // Always 4
    internal.uint16_8 = data_reader.readU16( endian ); // Always 1
    internal.value_0 = data_reader.readU16( endian ); // Values: 0, 512, 1024, 1536, 2048, 2560, 3072, 3584,
    internal.uint16_10 = data_reader.readU16( endian ); // Always 0
    internal.uint16_11 = data_reader.readU16( endian ); // Always 2457
    internal.uint16_12 = data_reader.readU16( endian ); // Always 6144
    internal.uint8_10 = data_reader.readU8(); // Always 7
    internal.uint32_2 = data_reader.readU32( endian ); // Always 0
    internal.uint8_11 = data_reader.readU8(); // Always 0
    internal.value_1 = data_reader.readU16( endian ); // Values: 0, 512, 1024, 1536, 2048, 2560, 3072, 3584,
    internal.uint8_12 = data_reader.readU8(); // Always 1
    internal.uint8_13 = data_reader.readU8(); // Always 2
    internal.uint8_14 = data_reader.readU8(); // Always 1
    internal.uint8_15 = data_reader.readU8(); // Always 2
    internal.value_2 = data_reader.readU8(); // Values: 0, 50, 60,
    internal.value_3 = data_reader.readU8(); // Values: 0, 33,
    internal.uint16_14 = data_reader.readU16( endian ); // Values: 1024, 1638, 1843, 

    return true;
}

Data::Mission::ACT::NeutralTurret::NeutralTurret() {}

Data::Mission::ACT::NeutralTurret::NeutralTurret( const ACTResource& obj ) : ACTResource( obj ) {}

Data::Mission::ACT::NeutralTurret::NeutralTurret( const NeutralTurret& obj ) : ACTResource( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::NeutralTurret::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::NeutralTurret::getTypeIDName() const {
    return "NeutralTurret";
}

size_t Data::Mission::ACT::NeutralTurret::getSize() const {
    return 60; // bytes
}

bool Data::Mission::ACT::NeutralTurret::checkRSL() const {
    // rsl_data[0] has alive gun.
    // rsl_data[1] has destroyed gun?
    // rsl_data[2] has alive base.
    // rsl_data[3] has destroyed base.
    return rsl_data.size() == 4 && rsl_data[0].type == ObjResource::IDENTIFIER_TAG && rsl_data[1].type == RSL_NULL_TAG && rsl_data[2].type == ObjResource::IDENTIFIER_TAG && (rsl_data[3].type == RSL_NULL_TAG || rsl_data[3].type == ObjResource::IDENTIFIER_TAG);
}

Data::Mission::Resource* Data::Mission::ACT::NeutralTurret::duplicate() const {
    return new Data::Mission::ACT::NeutralTurret( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::NeutralTurret::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const NeutralTurret*>( &original );

    if( copy_r != nullptr )
        return new Data::Mission::ACT::NeutralTurret( *copy_r );
    else
        return new Data::Mission::ACT::NeutralTurret( original );
}

Data::Mission::ACT::NeutralTurret::Internal Data::Mission::ACT::NeutralTurret::getInternal() const {
    return internal;
}

std::vector<Data::Mission::ACT::NeutralTurret*> Data::Mission::ACT::NeutralTurret::getVector( Data::Mission::ACTManager& act_manager ) {
    std::vector<ACTResource*> to_copy = act_manager.getACTs( Data::Mission::ACT::NeutralTurret::TYPE_ID );

    std::vector<NeutralTurret*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<NeutralTurret*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::ACT::NeutralTurret*> Data::Mission::ACT::NeutralTurret::getVector( const Data::Mission::ACTManager& act_manager ) {
    return Data::Mission::ACT::NeutralTurret::getVector( const_cast< Data::Mission::ACTManager& >( act_manager ) );
}
