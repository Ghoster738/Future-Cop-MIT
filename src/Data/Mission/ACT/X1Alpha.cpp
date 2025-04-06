#include "X1Alpha.h"

#include "../ObjResource.h"
#include <cassert>

uint_fast8_t Data::Mission::ACT::X1Alpha::TYPE_ID = 1;

Json::Value Data::Mission::ACT::X1Alpha::makeJson() const {
    Json::Value root = BaseEntity::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["rotation"] = internal.rotation;
    root["ACT"][NAME]["uint8_0"]  = internal.uint8_0;
    root["ACT"][NAME]["uint8_1"]  = internal.uint8_1;

    return root;
}

bool Data::Mission::ACT::X1Alpha::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    BaseEntity::readBase(data_reader, endian);

    internal.rotation = data_reader.readI16( endian ); // Values: -4096, -3072, -2048, -1024, -1, 0, 1024, 1536, 2048, 3072
    internal.uint8_0  = data_reader.readU8();          // Values: 0, 1,
    internal.uint8_1  = data_reader.readU8();          // Values: 0, 1, 255,

    return true;
}

Data::Mission::ACT::X1Alpha::X1Alpha() {}

Data::Mission::ACT::X1Alpha::X1Alpha( const ACTResource& obj ) : BaseEntity( obj ) {}

Data::Mission::ACT::X1Alpha::X1Alpha( const BaseEntity& obj ) : BaseEntity( obj ) {}

Data::Mission::ACT::X1Alpha::X1Alpha( const X1Alpha& obj ) : BaseEntity( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::X1Alpha::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::X1Alpha::getTypeIDName() const {
    return "X1Alpha";
}

size_t Data::Mission::ACT::X1Alpha::getSize() const {
    return 20; // bytes
}

bool Data::Mission::ACT::X1Alpha::checkRSL() const {
    // rsl_data[0] is legs
    // rsl_data[1] have might be something to do with death? However, it is not used.
    // rsl_data[2] is cockpit
    // rsl_data[3] is gun
    // rsl_data[4] is lights
    // rsl_data[5] is the pilot of X1A only seen when X1A is destoryed
    return rsl_data.size() == 6 && rsl_data[0].type == ObjResource::IDENTIFIER_TAG && rsl_data[1].type == RSL_NULL_TAG && rsl_data[2].type == ObjResource::IDENTIFIER_TAG && rsl_data[3].type == ObjResource::IDENTIFIER_TAG && rsl_data[4].type == ObjResource::IDENTIFIER_TAG && rsl_data[5].type == ObjResource::IDENTIFIER_TAG;
}

Data::Mission::Resource* Data::Mission::ACT::X1Alpha::duplicate() const {
    return new Data::Mission::ACT::X1Alpha( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::X1Alpha::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const X1Alpha*>( &original );

    if( copy_r != nullptr )
        return new Data::Mission::ACT::X1Alpha( *copy_r );
    else
        return new Data::Mission::ACT::X1Alpha( original );
}

Data::Mission::ACT::X1Alpha::Internal Data::Mission::ACT::X1Alpha::getInternal() const {
    return internal;
}

glm::quat Data::Mission::ACT::X1Alpha::getRotationQuaternion() const {
    return glm::quat( glm::vec3(0, glm::tau<float>() / 4096.f * ((std::abs(internal.rotation) + 1024) % 4096), 0) );
}
