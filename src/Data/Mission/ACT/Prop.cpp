#include "Prop.h"

#include "../ObjResource.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::Prop::TYPE_ID = 96;

Json::Value Data::Mission::ACT::Prop::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["rotation_y"]       = internal.rotation_y;
    root["ACT"][NAME]["rotation_z"]       = internal.rotation_z;
    root["ACT"][NAME]["rotation_x"]       = internal.rotation_x;
    root["ACT"][NAME]["height_offset"]    = internal.height_offset;
    root["ACT"][NAME]["ground_cast_type"] = internal.ground_cast_type;
    root["ACT"][NAME]["uint8_1"]          = internal.uint8_1;
    root["ACT"][NAME]["animation_speed"]  = internal.animation_speed;
    root["ACT"][NAME]["scale_y"]          = internal.scale_y;
    root["ACT"][NAME]["scale_z"]          = internal.scale_z;
    root["ACT"][NAME]["scale_x"]          = internal.scale_x;
    root["ACT"][NAME]["spin_speed"]       = internal.spin_speed;
    root["ACT"][NAME]["spin_angle"]       = internal.spin_angle;

    return root;
}

bool Data::Mission::ACT::Prop::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    internal.rotation_y = data_reader.readI16( endian ); // Values: 0, 512, 800, 900, 1000, 1024, 1111, 1200, 1256, 1536, 1650, 2000, 2048, 2100, 2222, 2300, 3000, 3072, 3076, 3333, 3584, 3600, 3700, 3900, 65535,
    internal.rotation_z = data_reader.readI16( endian ); // Values: 0, 84, 85, 86, 87, 88, 89, 90, 100, 3072, 3700,
    internal.rotation_x = data_reader.readI16( endian ); // Values: 0, 13, 15, 17, 18, 19, 23, 24, 27, 200, 300, 340, 700, 3396,
    internal.height_offset = data_reader.readU16( endian ); // Values: 0, 40, 81, 122, 163, 204, 245, 250, 276, 286, 327, 450, 491, 512, 532, 573, 614, 696, 737, 819, 901, 983, 1024, 1044, 1126, 1187, 1228, 1310, 1351, 1433, 1536, 1638, 3276, 63692, 64512, 65208, 65249, 65290, 65331, 65372, 65413, 65433, 65454,
    internal.ground_cast_type = data_reader.readU8(); // Values: 0, 1, 255,
    internal.uint8_1 = data_reader.readU8(); // Values: 0, 1, 2, 4, 16, 48, 64, 66, 68, 
    internal.animation_speed = data_reader.readU8(); // Values: 0, 8, 10, 13, 14, 15, 16,
    internal.scale_y = data_reader.readU8(); // Values: 19, 20, 21, 22, 23, 24, 25, 32, 38, 41, 44, 48, 51, 60, 64, 67, 70, 76, 83, 89, 112, 121,
    internal.scale_z = data_reader.readU8(); // Values: 19, 20, 21, 23, 24, 25, 32, 38, 41, 44, 48, 51, 60, 64, 67, 76, 83, 96, 105, 108, 112, 115, 121,
    internal.scale_x = data_reader.readU8(); // Values: 19, 20, 21, 23, 24, 25, 32, 38, 41, 44, 48, 51, 57, 60, 64, 67, 70, 76, 83, 89, 112, 121,
    internal.spin_speed = data_reader.readU8(); // Values: 0, 1, 3, 4, 5, 7, 8, 16, 254,
    internal.spin_angle = data_reader.readU8(); // Values: 0, 30, 40, 45, 60,

    return true;
}

Data::Mission::ACT::Prop::Prop() {}

Data::Mission::ACT::Prop::Prop( const ACTResource& obj ) : ACTResource( obj ) {}

Data::Mission::ACT::Prop::Prop( const Prop& obj ) : ACTResource( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::Prop::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::Prop::getTypeIDName() const {
    return "Prop";
}

size_t Data::Mission::ACT::Prop::getSize() const {
    return 16; // bytes
}

bool Data::Mission::ACT::Prop::checkRSL() const {
    return rsl_data.size() == 1 && rsl_data[0].type == Data::Mission::ObjResource::IDENTIFIER_TAG;
}

Data::Mission::Resource* Data::Mission::ACT::Prop::duplicate() const {
    return new Data::Mission::ACT::Prop( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::Prop::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const Prop*>( &original );

    if( copy_r != nullptr )
        return new Data::Mission::ACT::Prop( *copy_r );
    else
        return new Data::Mission::ACT::Prop( original );
}

Data::Mission::ACT::Prop::Internal Data::Mission::ACT::Prop::getInternal() const {
    return internal;
}

glm::quat Data::Mission::ACT::Prop::getRotationQuaternion() const {
    return glm::quat( glm::vec3(getRotation(0), getRotation(internal.rotation_y - 1024), getRotation(0)) );
}
