#include "Turret.h"

#include "../ObjResource.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::Turret::TYPE_ID = 8;

Json::Value Data::Mission::ACT::Turret::makeJson() const {
    Json::Value root = BaseTurret::makeJson();
    const std::string NAME = Turret::getTypeIDName();

    root["ACT"][NAME]["zero_0"] = internal.zero_0;
    root["ACT"][NAME]["uint8_0"] = internal.uint8_0;
    root["ACT"][NAME]["zero_1"] = internal.zero_1;
    root["ACT"][NAME]["base_rotation"] = internal.base_rotation;

    return root;
}

bool Data::Mission::ACT::Turret::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    readBase(data_reader, endian);

    return true;
}

bool Data::Mission::ACT::Turret::readBase( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    BaseTurret::readBase(data_reader, endian);

    internal.zero_0        = data_reader.readU16( endian ); // Always 0
    internal.uint8_0       = data_reader.readU8();          // Values: 0, 1,
    internal.zero_1        = data_reader.readU8();          // Always 0
    internal.base_rotation = data_reader.readI16( endian ); // Values: 0, 200, 256, 300, 400, 512, 1024, 1100, 1400, 1536, 1800, 2048, 2560, 2900, 3060, 3072, 3200, 3400, 3572, 3584, 3600, 3700,

    return true;
}

Data::Mission::ACT::Turret::Turret() {}

Data::Mission::ACT::Turret::Turret( const ACTResource& obj ) : BaseTurret( obj ) {}

Data::Mission::ACT::Turret::Turret( const Turret& obj ) : BaseTurret( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::Turret::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::Turret::getTypeIDName() const {
    return "Turret";
}

size_t Data::Mission::ACT::Turret::getSize() const {
    return 52; // bytes
}

bool Data::Mission::ACT::Turret::checkRSL() const {
    // rsl_data[0] has alive gun.
    // rsl_data[1] has destroyed gun?
    // rsl_data[2] has alive base.
    // rsl_data[3] has destroyed base.
    return rsl_data.size() == 4 && rsl_data[0].type == ObjResource::IDENTIFIER_TAG && rsl_data[1].type == RSL_NULL_TAG && rsl_data[2].type == ObjResource::IDENTIFIER_TAG && (rsl_data[3].type == RSL_NULL_TAG || rsl_data[3].type == ObjResource::IDENTIFIER_TAG);
}

Data::Mission::Resource* Data::Mission::ACT::Turret::duplicate() const {
    return new Data::Mission::ACT::Turret( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::Turret::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const Turret*>( &original );

    if( copy_r != nullptr )
        return new Data::Mission::ACT::Turret( *copy_r );
    else
        return new Data::Mission::ACT::Turret( original );
}

Data::Mission::ACT::Turret::Internal Data::Mission::ACT::Turret::getInternal() const {
    return internal;
}

float Data::Mission::ACT::Turret::getBaseRotation() const {
    return ACTResource::getRotation( internal.base_rotation - 1024 );
}

glm::quat Data::Mission::ACT::Turret::getBaseRotationQuaternion() const {
    return getRotationQuaternion( this->getBaseRotation() );
}
