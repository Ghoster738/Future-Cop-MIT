#include "WalkableProp.h"

#include "../ObjResource.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::WalkableProp::TYPE_ID = 12;

Json::Value Data::Mission::ACT::WalkableProp::makeJson() const {
    Json::Value root = BaseEntity::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["rotation_y"]    = internal.rotation_y;
    root["ACT"][NAME]["rotation_x"]    = internal.rotation_x;
    root["ACT"][NAME]["height_offset"] = internal.height_offset;
    root["ACT"][NAME]["tile_effect"]   = internal.tile_effect;
    root["ACT"][NAME]["uint8_0"]       = internal.uint8_0;

    return root;
}

bool Data::Mission::ACT::WalkableProp::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    BaseEntity::readBase(data_reader, endian);

    internal.rotation_y    = data_reader.readU16( endian ); // Values: 0, 1024, 1536, 3072,
    internal.rotation_x    = data_reader.readU16( endian ); // Always 0
    internal.height_offset = data_reader.readU16( endian ); // Values: 0, 81, 122, 512, 942, 1003, 1024, 1925,
    internal.tile_effect   = data_reader.readU8(); // Always 0
    internal.uint8_0       = data_reader.readU8(); // Always 0

    return true;
}

Data::Mission::ACT::WalkableProp::WalkableProp() {}

Data::Mission::ACT::WalkableProp::WalkableProp( const ACTResource& obj ) : BaseEntity( obj ) {}

Data::Mission::ACT::WalkableProp::WalkableProp( const WalkableProp& obj ) : BaseEntity( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::WalkableProp::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::WalkableProp::getTypeIDName() const {
    return "WalkableProp";
}

size_t Data::Mission::ACT::WalkableProp::getSize() const {
    return 24; // bytes
}

bool Data::Mission::ACT::WalkableProp::checkRSL() const {
    if(rsl_data.size() != 2)
        return false;

    if( rsl_data[0].type != Data::Mission::ObjResource::IDENTIFIER_TAG )
        return false;

    if( rsl_data[1].type != Data::Mission::ObjResource::IDENTIFIER_TAG && rsl_data[1].type != RSL_NULL_TAG )
        return false;

    return true;
}

Data::Mission::Resource* Data::Mission::ACT::WalkableProp::duplicate() const {
    return new Data::Mission::ACT::WalkableProp( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::WalkableProp::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const WalkableProp*>( &original );

    if( copy_r != nullptr )
        return new Data::Mission::ACT::WalkableProp( *copy_r );
    else
        return new Data::Mission::ACT::WalkableProp( original );
}

Data::Mission::ACT::WalkableProp::Internal Data::Mission::ACT::WalkableProp::getInternal() const {
    return internal;
}

glm::quat Data::Mission::ACT::WalkableProp::getRotationQuaternion() const {
    glm::quat axis = glm::quat( glm::vec3(0, 0.5 * glm::pi<float>(), 0) );

    axis = glm::rotate(axis, getRotation(internal.rotation_y), glm::vec3( 0, 1, 0));
    axis = glm::rotate(axis, getRotation(internal.rotation_x), glm::vec3(-1, 0, 0));

    return axis;
}
