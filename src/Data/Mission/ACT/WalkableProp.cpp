#include "WalkableProp.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::WalkableProp::TYPE_ID = 12;

Json::Value Data::Mission::ACT::WalkableProp::makeJson() const {
    Json::Value root = BaseEntity::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["uint16_0"] = internal.uint16_0;
    root["ACT"][NAME]["uint16_1"] = internal.uint16_1;
    root["ACT"][NAME]["uint16_2"] = internal.uint16_2;
    root["ACT"][NAME]["uint16_3"] = internal.uint16_3;

    return root;
}

bool Data::Mission::ACT::WalkableProp::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    BaseEntity::readBase(data_reader, endian);

    internal.uint16_0 = data_reader.readU16( endian ); // Values: 0, 1024, 1536, 3072,
    internal.uint16_1 = data_reader.readU16( endian ); // Always 0
    internal.uint16_2 = data_reader.readU16( endian ); // Values: 0, 81, 122, 512, 942, 1003, 1024, 1925,
    internal.uint16_3 = data_reader.readU16( endian ); // Always 0

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

bool Data::Mission::ACT::WalkableProp::checkRSL() const { return false; }

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
