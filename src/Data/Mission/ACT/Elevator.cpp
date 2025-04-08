#include "Elevator.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::Elevator::TYPE_ID = 10;

Json::Value Data::Mission::ACT::Elevator::makeJson() const {
    Json::Value root = BaseEntity::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["num_of_stops"]      = internal.num_of_stops;
    root["ACT"][NAME]["starting_position"] = internal.starting_position;
    root["ACT"][NAME]["height_offset_0"]   = internal.height_offset[0];
    root["ACT"][NAME]["height_offset_1"]   = internal.height_offset[1];
    root["ACT"][NAME]["height_offset_2"]   = internal.height_offset[2];
    root["ACT"][NAME]["stop_time_0"]       = internal.stop_time[0];
    root["ACT"][NAME]["stop_time_1"]       = internal.stop_time[1];
    root["ACT"][NAME]["stop_time_2"]       = internal.stop_time[2];
    root["ACT"][NAME]["up_speed"]          = internal.up_speed;
    root["ACT"][NAME]["down_speed"]        = internal.down_speed;
    root["ACT"][NAME]["rotation"]          = internal.rotation;
    root["ACT"][NAME]["trigger_type"]      = internal.trigger_type;
    root["ACT"][NAME]["tile_effect"]       = internal.tile_effect;
    root["ACT"][NAME]["end_sound"]         = internal.end_sound;
    root["ACT"][NAME]["uint8_0"]           = internal.uint8_0;

    return root;
}

bool Data::Mission::ACT::Elevator::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    BaseEntity::readBase(data_reader, endian);

    internal.num_of_stops      = data_reader.readU8(); // Values: 2, 3,
    internal.starting_position = data_reader.readU8(); // Values: 1, 2,
    internal.height_offset[0]  = data_reader.readI16( endian ); // Values: -410, -9, 0, 40, 81, 122, 409, 491, 614, 819,
    internal.height_offset[1]  = data_reader.readI16( endian ); // Values: 204, 409, 512, 614, 696, 729, 737, 798, 819, 860, 901, 921, 942, 983, 1024, 1064, 1105, 1146, 1228, 1331, 1351, 1433, 1536, 1556, 1679, 1720, 1740, 1843, 1925,
    internal.height_offset[2]  = data_reader.readI16( endian ); // Values: 0, 860, 901, 1228, 1331, 1474, 1536, 1638, 1843,
    internal.stop_time[0]      = data_reader.readU16( endian ); // Values: 0, 25, 256, 512, 768, 15360, 32512,
    internal.stop_time[1]      = data_reader.readU16( endian ); // Values: 0, 25, 256, 384, 486, 512, 768, 896, 1280,
    internal.stop_time[2]      = data_reader.readU16( endian ); // Values: 0, 25, 512, 768,
    internal.up_speed          = data_reader.readU16( endian ); // Values: 81, 204, 245, 286, 307, 348, 368, 409, 450, 491, 532,
    internal.down_speed        = data_reader.readU16( endian ); // Values: 40, 409, 491, 512, 573, 655, 1024,
    internal.rotation          = data_reader.readI16( endian ); // Values: 0, 512, 1024, 2048, 3072,
    internal.trigger_type      = data_reader.readU8(); // Values: 0, 1, 2, 3, 4,
    internal.tile_effect       = data_reader.readU8(); // Values: 0, 1, 14, 15,
    internal.end_sound         = data_reader.readU8(); // Values: 0, 71, 75, 81, 83, 99,
    internal.uint8_0           = data_reader.readU8(); // Always 0

    return true;
}

Data::Mission::ACT::Elevator::Elevator() {}

Data::Mission::ACT::Elevator::Elevator( const ACTResource& obj ) : BaseEntity( obj ) {}

Data::Mission::ACT::Elevator::Elevator( const Elevator& obj ) : BaseEntity( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::Elevator::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::Elevator::getTypeIDName() const {
    return "Elevator";
}

size_t Data::Mission::ACT::Elevator::getSize() const {
    return 40; // bytes
}

bool Data::Mission::ACT::Elevator::checkRSL() const { return false; }

Data::Mission::Resource* Data::Mission::ACT::Elevator::duplicate() const {
    return new Data::Mission::ACT::Elevator( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::Elevator::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const Elevator*>( &original );

    if( copy_r != nullptr )
        return new Data::Mission::ACT::Elevator( *copy_r );
    else
        return new Data::Mission::ACT::Elevator( original );
}

Data::Mission::ACT::Elevator::Internal Data::Mission::ACT::Elevator::getInternal() const {
    return internal;
}
