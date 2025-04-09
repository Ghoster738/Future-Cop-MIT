#include "PathedActor.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::PathedActor::TYPE_ID = 5;

Json::Value Data::Mission::ACT::PathedActor::makeJson() const {
    Json::Value root = BasePathedEntity::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["uint8_12"] = internal.uint8_12;
    root["ACT"][NAME]["uint8_13"] = internal.uint8_13;
    root["ACT"][NAME]["uint8_14"] = internal.uint8_14;
    root["ACT"][NAME]["uint8_15"] = internal.uint8_15;

    return root;
}

bool Data::Mission::ACT::PathedActor::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    BasePathedEntity::readBase(data_reader, endian);

    internal.uint8_12 = data_reader.readU8(); // Values: 0, 12, 27, 30, 40, 41, 42, 115, 117, 118, 120, 121, 
    internal.uint8_13 = data_reader.readU8(); // Values: 1, 4, 5, 
    internal.uint8_14 = data_reader.readU8(); // Values: 42, 58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 255, 
    internal.uint8_15 = data_reader.readU8(); // Always 0

    return true;
}

Data::Mission::ACT::PathedActor::PathedActor() {}

Data::Mission::ACT::PathedActor::PathedActor( const ACTResource& obj ) : BasePathedEntity( obj ) {}

Data::Mission::ACT::PathedActor::PathedActor( const PathedActor& obj ) : BasePathedEntity( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::PathedActor::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::PathedActor::getTypeIDName() const {
    return "PathedActor";
}

size_t Data::Mission::ACT::PathedActor::getSize() const {
    return 56; // bytes
}

bool Data::Mission::ACT::PathedActor::checkRSL() const { return false; }

Data::Mission::Resource* Data::Mission::ACT::PathedActor::duplicate() const {
    return new Data::Mission::ACT::PathedActor( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::PathedActor::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const PathedActor*>( &original );

    if( copy_r != nullptr )
        return new Data::Mission::ACT::PathedActor( *copy_r );
    else
        return new Data::Mission::ACT::PathedActor( original );
}

Data::Mission::ACT::PathedActor::Internal Data::Mission::ACT::PathedActor::getInternal() const {
    return internal;
}
