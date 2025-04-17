#include "StationaryActor.h"

#include "../ObjResource.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::StationaryActor::TYPE_ID = 6;

Json::Value Data::Mission::ACT::StationaryActor::makeJson() const {
    Json::Value root = BaseTurret::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["uint16_0"] = internal.uint16_0;

    return root;
}

bool Data::Mission::ACT::StationaryActor::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    BaseTurret::readBase(data_reader, endian);

    internal.uint16_0 = data_reader.readU16( endian ); // Always 0

    return true;
}

Data::Mission::ACT::StationaryActor::StationaryActor() {}

Data::Mission::ACT::StationaryActor::StationaryActor( const ACTResource& obj ) : BaseTurret( obj ) {}

Data::Mission::ACT::StationaryActor::StationaryActor( const StationaryActor& obj ) : BaseTurret( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::StationaryActor::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::StationaryActor::getTypeIDName() const {
    return "StationaryActor";
}

size_t Data::Mission::ACT::StationaryActor::getSize() const {
    return 48; // bytes
}

bool Data::Mission::ACT::StationaryActor::checkRSL() const {
    if( rsl_data.size() != 2 )
        return false;

    if( rsl_data[0].type != ObjResource::IDENTIFIER_TAG )
        return false;

    if( rsl_data[1].type != ObjResource::IDENTIFIER_TAG && rsl_data[1].type != RSL_NULL_TAG )
        return false;

    return true;
}

Data::Mission::Resource* Data::Mission::ACT::StationaryActor::duplicate() const {
    return new Data::Mission::ACT::StationaryActor( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::StationaryActor::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const StationaryActor*>( &original );

    if( copy_r != nullptr )
        return new Data::Mission::ACT::StationaryActor( *copy_r );
    else
        return new Data::Mission::ACT::StationaryActor( original );
}

Data::Mission::ACT::StationaryActor::Internal Data::Mission::ACT::StationaryActor::getInternal() const {
    return internal;
}
