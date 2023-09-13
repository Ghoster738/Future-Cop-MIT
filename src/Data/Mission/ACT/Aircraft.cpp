#include "Aircraft.h"

#include "../ObjResource.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::Aircraft::TYPE_ID = 9;

Json::Value Data::Mission::ACT::Aircraft::makeJson() const {
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
    root["ACT"][NAME]["zero"] = internal.zero;
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
    root["ACT"][NAME]["uint8_12"] = internal.uint8_12;
    root["ACT"][NAME]["uint8_13"] = internal.uint8_13;
    root["ACT"][NAME]["uint16_8"] = internal.uint16_8;
    root["ACT"][NAME]["uint16_9"] = internal.uint16_9;
    root["ACT"][NAME]["uint16_10"] = internal.uint16_10;
    root["ACT"][NAME]["uint16_11"] = internal.uint16_11;
    root["ACT"][NAME]["uint16_12"] = internal.uint16_12;
    root["ACT"][NAME]["uint16_13"] = internal.uint16_13;
    root["ACT"][NAME]["uint16_14"] = internal.uint16_14;
    root["ACT"][NAME]["uint16_15"] = internal.uint16_15;
    root["ACT"][NAME]["uint16_16"] = internal.uint16_16;
    root["ACT"][NAME]["uint16_17"] = internal.uint16_17;
    root["ACT"][NAME]["uint16_18"] = internal.uint16_18;
    root["ACT"][NAME]["uint16_19"] = internal.uint16_19;
    root["ACT"][NAME]["uint16_20"] = internal.uint16_20;

    return root;
}

bool Data::Mission::ACT::Aircraft::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    internal.uint32_0 = data_reader.readU32( endian ); // Values: 524288, 524352, 524616, 589824, 589888, 590664, 598016, 598080, 2621440, 
    internal.uint16_0 = data_reader.readU16( endian ); // Values: 24, 25, 30, 35, 50, 100, 150, 200, 2000, 
    internal.uint16_1 = data_reader.readU16( endian ); // Values: 0, 1, 3, 4, 10, 
    internal.uint8_0 = data_reader.readU8(); // Values: 0, 1, 2, 4, 6, 31, 
    internal.uint8_1 = data_reader.readU8(); // Values: 0, 2, 3, 4, 8, 12, 13, 32, 64, 70, 
    internal.uint8_2 = data_reader.readU8(); // Values: 1, 2, 4, 8, 9, 68, 72, 
    internal.uint8_3 = data_reader.readU8(); // Values: 0, 5, 6, 10, 
    internal.uint8_4 = data_reader.readU8(); // Values: 23, 26, 101, 222, 
    internal.uint8_5 = data_reader.readU8(); // Values: 0, 70, 71, 75, 77, 87, 88, 98, 101, 107, 
    internal.zero = data_reader.readU8(); // Always 0
    internal.uint8_7 = data_reader.readU8(); // Values: 0, 50, 51, 76, 78, 
    internal.uint16_2 = data_reader.readU16( endian ); // Values: 1, 3, 4, 5, 6, 9, 11, 12, 14, 
    internal.uint16_3 = data_reader.readU16( endian ); // Values: 0, 2, 6, 38, 64, 70, 514, 544, 
    internal.uint8_8 = data_reader.readU8(); // Values: 1, 2, 3, 
    internal.uint8_9 = data_reader.readU8(); // Values: 1, 2, 3, 4, 
    internal.uint16_4 = data_reader.readU16( endian ); // Values: 1, 2, 32, 441, 
    internal.uint32_1 = data_reader.readU32( endian ); // Always 268439552 or 0x10001000
    internal.uint16_5 = data_reader.readU16( endian ); // Values: 6144, 8192, 
    internal.uint16_6 = data_reader.readU16( endian ); // Values: 8, 16, 32, 48, 80, 
    internal.uint8_10 = data_reader.readU8(); // Values: 1, 3, 
    internal.uint8_11 = data_reader.readU8(); // Values: 1, 2, 3, 4, 
    internal.uint16_7 = data_reader.readU16( endian ); // Values: 1, 2, 32, 441, 
    internal.uint8_12 = data_reader.readU8(); // Values: 0, 1, 2, 
    internal.uint8_13 = data_reader.readU8(); // Values: 0, 1, 2, 3, 8, 12, 
    internal.uint16_8 = data_reader.readU16( endian ); // Values: 8192, 16384, 24576, 28672, 32358, 
    internal.uint16_9 = data_reader.readU16( endian ); // Values: 51, 512, 614, 819, 
    internal.uint16_10 = data_reader.readU16( endian ); // Values: 102, 409, 614, 737, 819, 1024, 1228, 
    internal.uint16_11 = data_reader.readU16( endian ); // Values: 512, 716, 737, 819, 901, 921, 1126, 1331, 1433, 
    internal.uint16_12 = data_reader.readU16( endian ); // Values: 737, 901, 1024, 1228, 1433, 1556, 1638, 
    internal.uint16_13 = data_reader.readU16( endian ); // Values: 3072, 3276, 4915, 5120, 5324, 8192, 
    internal.uint16_14 = data_reader.readU16( endian ); // Values: 1433, 1638, 1843, 2048, 2457, 
    internal.uint16_15 = data_reader.readU16( endian ); // Values: 4096, 5734, 8192, 12288, 15564, 16384, 20480, 24576, 30720, 31744, 32727, 32763, 
    internal.uint16_16 = data_reader.readU16( endian ); // Values: 3686, 4096, 4915, 8192, 10240, 12288, 16384, 20480, 24576, 28672, 31744, 32763, 
    internal.uint16_17 = data_reader.readU16( endian ); // Values: 5120, 25600, 
    internal.uint16_18 = data_reader.readU16( endian ); // Values: 256, 1280, 
    internal.uint16_19 = data_reader.readU16( endian ); // Values: 0, 1168, 1217, 1285, 1343, 1515, 1632, 1637, 1693, 1745, 1873, 1904, 1905, 1971, 1979, 1984, 2144, 2163, 2178, 
    internal.uint16_20 = data_reader.readU16( endian ); // Values: 0, 1183, 1193, 1203, 1232, 1504, 1600, 1612, 1681, 1983, 1984, 2118, 2173, 2279, 2315, 2317, 2320, 2374, 2392, 2563, 2629, 

    return true;
}

Data::Mission::ACT::Aircraft::Aircraft() {}

Data::Mission::ACT::Aircraft::Aircraft( const ACTResource& obj ) : ACTResource( obj ) {}

Data::Mission::ACT::Aircraft::Aircraft( const Aircraft& obj ) : ACTResource( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::Aircraft::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::Aircraft::getTypeIDName() const {
    return "Aircraft";
}

size_t Data::Mission::ACT::Aircraft::getSize() const {
    return 64; // bytes
}

bool Data::Mission::ACT::Aircraft::checkRSL() const {
    // rsl_data[0] is model
    // rsl_data[1] is probably destroyed model?
    return rsl_data.size() == 2 && rsl_data[0].type == ObjResource::IDENTIFIER_TAG && rsl_data[1].type == RSL_NULL_TAG;
}

Data::Mission::Resource* Data::Mission::ACT::Aircraft::duplicate() const {
    return new Data::Mission::ACT::Aircraft( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::Aircraft::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const Aircraft*>( &original );

    if( copy_r != nullptr )
        return new Data::Mission::ACT::Aircraft( *copy_r );
    else
        return new Data::Mission::ACT::Aircraft( original );
}

Data::Mission::ACT::Aircraft::Internal Data::Mission::ACT::Aircraft::getInternal() const {
    return internal;
}

std::vector<Data::Mission::ACT::Aircraft*> Data::Mission::ACT::Aircraft::getVector( Data::Mission::ACTManager& act_manager ) {
    std::vector<ACTResource*> to_copy = act_manager.getACTs( Data::Mission::ACT::Aircraft::TYPE_ID );

    std::vector<Aircraft*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<Aircraft*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::ACT::Aircraft*> Data::Mission::ACT::Aircraft::getVector( const Data::Mission::ACTManager& act_manager ) {
    return Data::Mission::ACT::Aircraft::getVector( const_cast< Data::Mission::ACTManager& >( act_manager ) );
}
