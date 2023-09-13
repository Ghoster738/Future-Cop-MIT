#include "BaseTurret.h"

#include "../ObjResource.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::BaseTurret::TYPE_ID = 8;

Json::Value Data::Mission::ACT::BaseTurret::makeJson() const {
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
    root["ACT"][NAME]["uint8_5"] = internal.uint8_5;
    root["ACT"][NAME]["uint8_6"] = internal.uint8_6;
    root["ACT"][NAME]["uint8_7"] = internal.uint8_7;
    root["ACT"][NAME]["uint16_2"] = internal.uint16_2;
    root["ACT"][NAME]["uint16_3"] = internal.uint16_3;
    root["ACT"][NAME]["uint8_8"] = internal.uint8_8;
    root["ACT"][NAME]["uint8_9"] = internal.uint8_9;
    root["ACT"][NAME]["uint16_4"] = internal.uint16_4;
    root["ACT"][NAME]["uint32_1"] = internal.uint32_1;
    root["ACT"][NAME]["uint16_5"] = internal.uint16_5;
    root["ACT"][NAME]["uint16_6"] = internal.uint16_6;
    root["ACT"][NAME]["uint8_10"] = internal.uint8_10;
    root["ACT"][NAME]["uint8_11"] = internal.uint8_11;
    root["ACT"][NAME]["uint16_7"] = internal.uint16_7;
    root["ACT"][NAME]["uint16_8"] = internal.uint16_8;
    root["ACT"][NAME]["uint16_9"] = internal.uint16_9;
    root["ACT"][NAME]["uint16_10"] = internal.uint16_10;
    root["ACT"][NAME]["uint16_11"] = internal.uint16_11;
    root["ACT"][NAME]["uint8_12"] = internal.uint8_12;
    root["ACT"][NAME]["zero_0"] = internal.zero_0;
    root["ACT"][NAME]["zero_1"] = internal.zero_1;
    root["ACT"][NAME]["uint8_14"] = internal.uint8_14;
    root["ACT"][NAME]["zero_2"] = internal.zero_2;
    root["ACT"][NAME]["uint16_13"] = internal.uint16_13;

    return root;
}

bool Data::Mission::ACT::BaseTurret::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    internal.uint32_0 = data_reader.readU32( endian ); // Values: 0, 64, 193, 328, 1088, 1217, 1249, 2112, 65536, 65600, 65601, 65729, 131152, 131264, 131265, 131648, 196800, 524288, 524354, 528384, 856130, 2097216, 2621504, 
    internal.uint16_0 = data_reader.readU16( endian ); // Values: 0, 1, 35, 50, 55, 75, 80, 90, 99, 100, 125, 150, 175, 200, 300, 400, 500, 1500, 
    internal.uint16_1 = data_reader.readU16( endian ); // Values: 0, 1, 4, 5, 10, 25, 100, 
    internal.uint8_0 = data_reader.readU8(); // Values: 0, 1, 2, 3, 6, 14, 
    internal.uint8_1 = data_reader.readU8(); // Values: 0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12, 13, 24, 28, 41, 61, 
    internal.uint8_2 = data_reader.readU8(); // Values: 0, 1, 2, 9, 10, 25, 
    internal.uint8_3 = data_reader.readU8(); // Values: 0, 3, 6, 7, 9, 10, 15, 
    internal.uint8_4 = data_reader.readU8(); // Values: 0, 26, 91, 100, 101, 102, 200, 201, 213, 216, 
    internal.uint8_5 = data_reader.readU8(); // Values: 0, 85, 89, 96, 98, 108, 

    // JOKE 100 or 50. The rest 0.
    internal.uint8_6 = data_reader.readU8(); // Values: 0, 50, 100, 

    // Precint Assualt maps seems to be 24, Crime War 0
    internal.uint8_7 = data_reader.readU8(); // Values: 0, 24, 

    internal.uint16_2 = data_reader.readU16( endian ); // Values: 1, 2, 3, 5, 6, 8, 11, 12, 13, 14, 16, 18, 95, 
    internal.uint16_3 = data_reader.readU16( endian ); // Values: 6, 14, 38, 46, 166, 256, 262, 288, 13574, 16966, 

    // Always seems to be zero in Precint Assult maps.
    internal.uint8_8 = data_reader.readU8(); // Values: 0, 1, 3, 

    internal.uint8_9 = data_reader.readU8(); // Values: 0, 1, 3, 4, 
    internal.uint16_4 = data_reader.readU16( endian ); // Values: 0, 1, 2, 13, 

    // uint16_1b == uint16_2b Maybe current/default health? Never Zero.
    // Maybe current/default health?
    internal.uint32_1 = data_reader.readU32( endian ); // Values: 134219776, 268374015, 268439552, 

    internal.uint16_5 = data_reader.readU16( endian ); // Values: 1228, 4096, 4505, 4915, 5120, 6144, 
    internal.uint16_6 = data_reader.readU16( endian ); // Values: 16, 24, 32, 48, 80, 
    internal.uint8_10 = data_reader.readU8(); // Values: 0, 1, 
    internal.uint8_11 = data_reader.readU8(); // Values: 0, 1, 4, 
    internal.uint16_7 = data_reader.readU16( endian ); // Values: 0, 1, 2, 
    internal.uint16_8 = data_reader.readU16( endian ); // Values: 0, 200, 400, 512, 1024, 1100, 1400, 1536, 1800, 2048, 2560, 2900, 3072, 3200, 3400, 3584, 3600, 3700, 65535, 
    internal.uint16_9 = data_reader.readU16( endian ); // Values: 0, 40, 327, 409, 491, 532, 1925, 2129, 
    internal.uint16_10 = data_reader.readU16( endian ); // Values: 0, 409, 819, 1024, 1228, 1638, 2048, 2457, 3276, 4096, 
    internal.uint16_11 = data_reader.readU16( endian ); // Values: 2048, 6144, 7782, 8192, 10240, 12288, 
    internal.uint8_12 = data_reader.readU8(); // Values: 0, 1, 3, 4, 5, 7, 35, 69, 71, 128, 132, 133, 
    internal.zero_0 = data_reader.readU16( endian ); // Always 0
    internal.zero_1 = data_reader.readU8(); // Always 0
    internal.uint8_14 = data_reader.readU8(); // Values: 0, 1, 
    internal.zero_2 = data_reader.readU8(); // Always 0
    internal.uint16_13 = data_reader.readU16( endian ); // Values: 0, 200, 256, 300, 400, 512, 1024, 1100, 1400, 1536, 1800, 2048, 2560, 2900, 3060, 3072, 3200, 3400, 3572, 3584, 3600, 3700, 

    return true;
}

Data::Mission::ACT::BaseTurret::BaseTurret() {}

Data::Mission::ACT::BaseTurret::BaseTurret( const ACTResource& obj ) : ACTResource( obj ) {}

Data::Mission::ACT::BaseTurret::BaseTurret( const BaseTurret& obj ) : ACTResource( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::BaseTurret::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::BaseTurret::getTypeIDName() const {
    return "BaseTurret";
}

size_t Data::Mission::ACT::BaseTurret::getSize() const {
    return 52; // bytes
}

bool Data::Mission::ACT::BaseTurret::checkRSL() const {
    // rsl_data[0] has alive gun.
    // rsl_data[1] has destroyed gun?
    // rsl_data[2] has alive base.
    // rsl_data[3] has destroyed base.
    return rsl_data.size() == 4 && rsl_data[0].type == ObjResource::IDENTIFIER_TAG && rsl_data[1].type == RSL_NULL_TAG && rsl_data[2].type == ObjResource::IDENTIFIER_TAG && (rsl_data[3].type == RSL_NULL_TAG || rsl_data[3].type == ObjResource::IDENTIFIER_TAG);
}

Data::Mission::Resource* Data::Mission::ACT::BaseTurret::duplicate() const {
    return new Data::Mission::ACT::BaseTurret( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::BaseTurret::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const BaseTurret*>( &original );

    if( copy_r != nullptr )
        return new Data::Mission::ACT::BaseTurret( *copy_r );
    else
        return new Data::Mission::ACT::BaseTurret( original );
}

Data::Mission::ACT::BaseTurret::Internal Data::Mission::ACT::BaseTurret::getInternal() const {
    return internal;
}

std::vector<Data::Mission::ACT::BaseTurret*> Data::Mission::ACT::BaseTurret::getVector( Data::Mission::ACTManager& act_manager ) {
    std::vector<ACTResource*> to_copy = act_manager.getACTs( Data::Mission::ACT::BaseTurret::TYPE_ID );

    std::vector<BaseTurret*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<BaseTurret*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::ACT::BaseTurret*> Data::Mission::ACT::BaseTurret::getVector( const Data::Mission::ACTManager& act_manager ) {
    return Data::Mission::ACT::BaseTurret::getVector( const_cast< Data::Mission::ACTManager& >( act_manager ) );
}
