#include "DCSQuad.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::DCSQuad::TYPE_ID = 97;

Json::Value Data::Mission::ACT::DCSQuad::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["tags"]          = internal.tags;
    root["ACT"][NAME]["dcs_id"]        = internal.dcs_id;
    root["ACT"][NAME]["height_offset"] = internal.height_offset;
    root["ACT"][NAME]["width"]         = internal.width;
    root["ACT"][NAME]["height"]        = internal.height;
    root["ACT"][NAME]["y_rotation"]    = internal.y_rotation;
    root["ACT"][NAME]["z_rotation"]    = internal.z_rotation;
    root["ACT"][NAME]["x_rotation"]    = internal.x_rotation;
    root["ACT"][NAME]["uint8_2"]       = internal.uint8_2;
    root["ACT"][NAME]["red"]           = internal.red;
    root["ACT"][NAME]["green"]         = internal.green;
    root["ACT"][NAME]["blue"]          = internal.blue;
    root["ACT"][NAME]["zero"]          = internal.zero;

    return root;
}

bool Data::Mission::ACT::DCSQuad::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    internal.tags = data_reader.readU8(); // Values: 0, 2, 16, 18,
    internal.dcs_id = data_reader.readU8(); // Values: 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 16, 17, 18, 19, 20, 21, 27, 28, 29, 31, 32, 35, 36,
    internal.height_offset = data_reader.readU16( endian ); // Values: 0, 4, 40, 61, 122, 163, 204, 245, 266, 286, 307, 327, 348, 368, 389, 409, 430, 450, 491, 512, 528, 532, 573, 593, 614, 634, 655, 675, 696, 716, 737, 757, 819, 860, 901, 942, 1024, 1064, 1105, 1126, 1167, 1208, 1228, 1269, 1310, 1392, 1433, 1536, 1638, 1720, 64512,
    internal.width = data_reader.readU16( endian ); // Values: 102, 163, 204, 245, 286, 307, 327, 368, 409, 491, 512, 532, 696, 819, 1024, 1228, 1351, 1433, 1638, 1761, 1802, 1843, 1925, 1966, 2007, 2048, 2375, 2457, 2662, 2744, 2867, 2990, 3072, 3276,
    internal.height = data_reader.readU16( endian ); // Values: 40, 204, 286, 307, 327, 348, 380, 409, 491, 512, 532, 614, 819, 983, 1024, 1228, 1351, 1433, 1638, 1761, 1802, 1843, 1925, 1966, 2007, 2048, 2375, 2457, 2662, 2744, 2867, 2990, 3072, 3276,
    internal.y_rotation = data_reader.readU16( endian ); // Values: 0, 30, 110, 512, 545, 800, 1024, 1200, 1235, 1280, 1333, 1536, 1793, 2000, 2048, 2200, 2222, 2560, 2750, 2900, 3072, 3333,
    internal.z_rotation = data_reader.readU16( endian ); // Values: 0, 200, 512, 1024, 2900, 3072, 3100, 3400, 3500, 3600, 3900,
    internal.x_rotation = data_reader.readU16( endian ); // Values: 0, 1024, 3072,
    internal.uint8_2 = data_reader.readU8(); // Values: 0, 1, 3, 
    internal.red = data_reader.readU8(); // Values: 0, 16, 20, 30, 32, 33, 34, 35, 40, 44, 50, 54, 64, 80, 84, 100, 128,
    internal.green = data_reader.readU8(); // Values: 16, 20, 28, 30, 32, 34, 40, 50, 62, 64, 75, 80, 82, 92, 96, 100, 120, 128,
    internal.blue = data_reader.readU8(); // Values: 0, 8, 16, 20, 21, 22, 24, 25, 30, 32, 34, 40, 50, 64, 80, 100, 128,
    internal.zero = data_reader.readU16( endian ); // Always 0

    return true;
}

Data::Mission::ACT::DCSQuad::DCSQuad() {}

Data::Mission::ACT::DCSQuad::DCSQuad( const ACTResource& obj ) : ACTResource( obj ) {}

Data::Mission::ACT::DCSQuad::DCSQuad( const DCSQuad& obj ) : ACTResource( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::DCSQuad::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::DCSQuad::getTypeIDName() const {
    return "DCSQuad";
}

size_t Data::Mission::ACT::DCSQuad::getSize() const {
    return 20; // bytes
}

bool Data::Mission::ACT::DCSQuad::checkRSL() const {
    if( rsl_data.size() != 4 )
        return false;

    if(rsl_data[0].type != RSL_NULL_TAG || rsl_data[0].resource_id)
        return false;

    if(rsl_data[1].type != RSL_NULL_TAG || rsl_data[1].resource_id)
        return false;

    if(rsl_data[2].type != RSL_NULL_TAG || rsl_data[2].resource_id)
        return false;

    if(rsl_data[3].type != RSL_NULL_TAG || rsl_data[3].resource_id)
        return false;

    return true;
}

Data::Mission::Resource* Data::Mission::ACT::DCSQuad::duplicate() const {
    return new Data::Mission::ACT::DCSQuad( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::DCSQuad::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const DCSQuad*>( &original );

    if( copy_r != nullptr )
        return new Data::Mission::ACT::DCSQuad( *copy_r );
    else
        return new Data::Mission::ACT::DCSQuad( original );
}

Data::Mission::ACT::DCSQuad::Internal Data::Mission::ACT::DCSQuad::getInternal() const {
    return internal;
}

glm::quat Data::Mission::ACT::DCSQuad::getRotationQuaternion() const {
    glm::quat axis = glm::quat( glm::vec3(0, 0.5 * glm::pi<float>(), 0) );

    axis = glm::rotate(axis, getRotation(internal.y_rotation), glm::vec3( 0, 1, 0));
    axis = glm::rotate(axis, getRotation(internal.z_rotation), glm::vec3( 0, 0, 1));
    axis = glm::rotate(axis, getRotation(internal.x_rotation), glm::vec3(-1, 0, 0));

    return axis;
}
