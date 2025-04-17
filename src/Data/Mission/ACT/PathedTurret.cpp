#include "PathedTurret.h"

#include "../NetResource.h"
#include "../ObjResource.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::PathedTurret::TYPE_ID = 20;

Json::Value Data::Mission::ACT::PathedTurret::makeJson() const {
    Json::Value root = BasePathedEntity::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["uint16_18"] = internal.uint16_18;
    root["ACT"][NAME]["uint16_19"] = internal.uint16_19;
    root["ACT"][NAME]["uint8_12"] = internal.uint8_12;
    root["ACT"][NAME]["uint8_13"] = internal.uint8_13;
    root["ACT"][NAME]["uint16_20"] = internal.uint16_20;

    return root;
}

bool Data::Mission::ACT::PathedTurret::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    BasePathedEntity::readBase(data_reader, endian);

    internal.uint16_18 = data_reader.readU16( endian ); // Values: 0, 38, 51, 64, 76, 89, 102, 128, 140, 243, 256, 
    internal.uint16_19 = data_reader.readU16( endian ); // Values: 0, 2, 2048, 4095, 65535, 
    internal.uint8_12 = data_reader.readU8(); // Values: 0, 2, 6, 10, 16, 32, 64, 80, 128, 144, 
    internal.uint8_13 = data_reader.readU8(); // Values: 0, 100, 101, 201, 
    internal.uint16_20 = data_reader.readU16( endian ); // Always 0

    return true;
}

Data::Mission::ACT::PathedTurret::PathedTurret() {}

Data::Mission::ACT::PathedTurret::PathedTurret( const ACTResource& obj ) : BasePathedEntity( obj ) {}

Data::Mission::ACT::PathedTurret::PathedTurret( const PathedTurret& obj ) : BasePathedEntity( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::PathedTurret::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::PathedTurret::getTypeIDName() const {
    return "PathedTurret";
}

size_t Data::Mission::ACT::PathedTurret::getSize() const {
    return 60; // bytes
}

bool Data::Mission::ACT::PathedTurret::checkRSL() const {
    if(rsl_data.size() != 4)
        return false;

    if( rsl_data[0].type != Data::Mission::ObjResource::IDENTIFIER_TAG )
        return false;

    if( rsl_data[1].type != Data::Mission::ObjResource::IDENTIFIER_TAG && rsl_data[1].type != RSL_NULL_TAG )
        return false;

    if( rsl_data[2].type != Data::Mission::NetResource::IDENTIFIER_TAG )
        return false;

    if( rsl_data[3].type != Data::Mission::ObjResource::IDENTIFIER_TAG )
        return false;

    return true;
}

Data::Mission::Resource* Data::Mission::ACT::PathedTurret::duplicate() const {
    return new Data::Mission::ACT::PathedTurret( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::PathedTurret::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const PathedTurret*>( &original );

    if( copy_r != nullptr )
        return new Data::Mission::ACT::PathedTurret( *copy_r );
    else
        return new Data::Mission::ACT::PathedTurret( original );
}

Data::Mission::ACT::PathedTurret::Internal Data::Mission::ACT::PathedTurret::getInternal() const {
    return internal;
}
