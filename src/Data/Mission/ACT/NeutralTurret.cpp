#include "NeutralTurret.h"

#include "../ObjResource.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::NeutralTurret::TYPE_ID = 36;

Json::Value Data::Mission::ACT::NeutralTurret::makeJson() const {
    Json::Value root = Turret::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["uint8_0"]  = turret_internal.uint8_0;
    root["ACT"][NAME]["uint8_1"]  = turret_internal.uint8_1;
    root["ACT"][NAME]["uint8_2"]  = turret_internal.uint8_2;
    root["ACT"][NAME]["uint8_3"]  = turret_internal.uint8_3;
    root["ACT"][NAME]["uint8_4"]  = turret_internal.uint8_4;
    root["ACT"][NAME]["uint8_5"]  = turret_internal.uint8_5;
    root["ACT"][NAME]["uint16_0"] = turret_internal.uint16_0;

    return root;
}

bool Data::Mission::ACT::NeutralTurret::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    Turret::readBase(data_reader, endian);

    turret_internal.uint8_0 = data_reader.readU8(); // Always 1
    turret_internal.uint8_1 = data_reader.readU8(); // Always 2
    turret_internal.uint8_2 = data_reader.readU8(); // Always 1
    turret_internal.uint8_3 = data_reader.readU8(); // Always 2
    turret_internal.uint8_4 = data_reader.readU8(); // Values: 0, 50, 60,
    turret_internal.uint8_5 = data_reader.readU8(); // Values: 0, 33,
    turret_internal.uint16_0 = data_reader.readU16( endian ); // Values: 1024, 1638, 1843,

    return true;
}

Data::Mission::ACT::NeutralTurret::NeutralTurret() {}

Data::Mission::ACT::NeutralTurret::NeutralTurret( const ACTResource& obj ) : Turret( obj ) {}

Data::Mission::ACT::NeutralTurret::NeutralTurret( const NeutralTurret& obj ) : Turret( obj ), turret_internal( obj.turret_internal ) {}

uint_fast8_t Data::Mission::ACT::NeutralTurret::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::NeutralTurret::getTypeIDName() const {
    return "NeutralTurret";
}

size_t Data::Mission::ACT::NeutralTurret::getSize() const {
    return 60; // bytes
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

Data::Mission::ACT::NeutralTurret::Internal Data::Mission::ACT::NeutralTurret::getNeutralInternal() const {
    return turret_internal;
}
