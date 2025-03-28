#include "Aircraft.h"

#include "../ObjResource.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::Aircraft::TYPE_ID = 9;

Json::Value Data::Mission::ACT::Aircraft::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"] = BaseEntity::makeJson();
    root["ACT"] = BaseShooterEntity::makeJson();
    root["ACT"][NAME]["uint8_0"] = internal.uint8_0;
    root["ACT"][NAME]["uint8_1"] = internal.uint8_1;
    root["ACT"][NAME]["uint16_0"] = internal.uint16_0;
    root["ACT"][NAME]["uint8_2"] = internal.uint8_2;
    root["ACT"][NAME]["spawn_type"] = internal.spawn_type;
    root["ACT"][NAME]["target_detection_range"] = internal.target_detection_range;
    root["ACT"][NAME]["uint16_2"] = internal.uint16_2;
    root["ACT"][NAME]["uint16_3"] = internal.uint16_3;
    root["ACT"][NAME]["height_offset"] = internal.height_offset;
    root["ACT"][NAME]["time_to_descend"] = internal.time_to_descend;
    root["ACT"][NAME]["turn_rate"] = internal.turn_rate;
    root["ACT"][NAME]["move_speed"] = internal.move_speed;
    root["ACT"][NAME]["orbit_area_x"] = internal.orbit_area_x;
    root["ACT"][NAME]["orbit_area_y"] = internal.orbit_area_y;
    root["ACT"][NAME]["uint16_10"] = internal.uint16_10;
    root["ACT"][NAME]["uint16_11"] = internal.uint16_11;
    root["ACT"][NAME]["spawn_pos_x"] = internal.spawn_pos_x;
    root["ACT"][NAME]["spawn_pos_y"] = internal.spawn_pos_y;

    return root;
}

bool Data::Mission::ACT::Aircraft::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    BaseShooterEntity::readBase(data_reader, endian);

    internal.uint8_0 = data_reader.readU8(); // Values: 1, 3,
    internal.uint8_1 = data_reader.readU8(); // Values: 1, 2, 3, 4,
    internal.uint16_0 = data_reader.readU16( endian ); // Values: 1, 2, 32, 441,
    internal.uint8_2 = data_reader.readU8(); // Values: 0, 1, 2,
    internal.spawn_type = data_reader.readU8(); // Values: 0, 1, 2, 3, 8, 12,
    internal.target_detection_range = data_reader.readU16( endian ); // Values: 8192, 16384, 24576, 28672, 32358,
    internal.uint16_2 = data_reader.readU16( endian ); // Values: 51, 512, 614, 819,
    internal.uint16_3 = data_reader.readU16( endian ); // Values: 102, 409, 614, 737, 819, 1024, 1228,
    internal.height_offset = data_reader.readU16( endian ); // Values: 512, 716, 737, 819, 901, 921, 1126, 1331, 1433,
    internal.time_to_descend = data_reader.readU16( endian ); // Values: 737, 901, 1024, 1228, 1433, 1556, 1638,
    internal.turn_rate = data_reader.readU16( endian ); // Values: 3072, 3276, 4915, 5120, 5324, 8192,
    internal.move_speed = data_reader.readU16( endian ); // Values: 1433, 1638, 1843, 2048, 2457,
    internal.orbit_area_x = data_reader.readU16( endian ); // Values: 4096, 5734, 8192, 12288, 15564, 16384, 20480, 24576, 30720, 31744, 32727, 32763,
    internal.orbit_area_y = data_reader.readU16( endian ); // Values: 3686, 4096, 4915, 8192, 10240, 12288, 16384, 20480, 24576, 28672, 31744, 32763,
    internal.uint16_10 = data_reader.readU16( endian ); // Values: 5120, 25600,
    internal.uint16_11 = data_reader.readU16( endian ); // Values: 256, 1280,
    internal.spawn_pos_x = data_reader.readU16( endian ); // Values: 0, 1168, 1217, 1285, 1343, 1515, 1632, 1637, 1693, 1745, 1873, 1904, 1905, 1971, 1979, 1984, 2144, 2163, 2178,
    internal.spawn_pos_y = data_reader.readU16( endian ); // Values: 0, 1183, 1193, 1203, 1232, 1504, 1600, 1612, 1681, 1983, 1984, 2118, 2173, 2279, 2315, 2317, 2320, 2374, 2392, 2563, 2629,

    return true;
}

Data::Mission::ACT::Aircraft::Aircraft() {}

Data::Mission::ACT::Aircraft::Aircraft( const ACTResource& obj ) : BaseShooterEntity( obj ) {}

Data::Mission::ACT::Aircraft::Aircraft( const BaseShooterEntity& obj ) : BaseShooterEntity( obj ) {}

Data::Mission::ACT::Aircraft::Aircraft( const Aircraft& obj ) : BaseShooterEntity( obj ), internal( obj.internal ) {}

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
