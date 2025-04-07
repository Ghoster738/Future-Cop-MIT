#include "DynamicProp.h"

#include "../ObjResource.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::DynamicProp::TYPE_ID = 11;

Json::Value Data::Mission::ACT::DynamicProp::makeJson() const {
    Json::Value root = BaseEntity::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["ground_cast_type"] = internal.ground_cast_type;
    root["ACT"][NAME]["zero"]             = internal.zero;
    root["ACT"][NAME]["uint16_0"]         = internal.uint16_0;
    root["ACT"][NAME]["uint32_0"]         = internal.uint32_0;

    return root;
}

bool Data::Mission::ACT::DynamicProp::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    BaseEntity::readBase(data_reader, endian);

    internal.ground_cast_type = data_reader.readU8(); // Values: 0, 1, 255,
    internal.zero             = data_reader.readU8(); // Always 0
    internal.uint16_0         = data_reader.readU16( endian ); // Values: 0, 100, 122, 222, 300, 310, 320, 340, 356, 400, 500, 512, 700, 1000, 1024, 1100, 1200, 1234, 1250, 1345, 1500, 1536, 1600, 1700, 1800, 2000, 2034, 2048, 2100, 2134, 2234, 2345, 2500, 2560, 2600, 2765, 2800, 2978, 3000, 3072, 3076, 3200, 3535, 3584, 3600, 3700, 3750, 3900, 4095,
    internal.uint32_0         = data_reader.readU32( endian ); // Values: 0, 655, 1311, 1966, 2621, 2949, 3277, 3932, 4588, 5898, 6554, 7209, 7537, 8192, 8258, 8520, 8847, 9175, 9830, 12124, 12452, 13107, 13763, 14418, 15073, 15729, 16384, 19661, 19988, 22938, 4294944358, 4294960742,

    return true;
}

Data::Mission::ACT::DynamicProp::DynamicProp() {}

Data::Mission::ACT::DynamicProp::DynamicProp( const ACTResource& obj ) : BaseEntity( obj ) {}

Data::Mission::ACT::DynamicProp::DynamicProp( const DynamicProp& obj ) : BaseEntity( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::DynamicProp::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::DynamicProp::getTypeIDName() const {
    return "DynamicProp";
}

size_t Data::Mission::ACT::DynamicProp::getSize() const {
    return 24; // bytes
}

bool Data::Mission::ACT::DynamicProp::checkRSL() const {
    if(rsl_data.size() != 2)
        return false;

    if( rsl_data[0].type != Data::Mission::ObjResource::IDENTIFIER_TAG )
        return false;

    if( rsl_data[1].type != Data::Mission::ObjResource::IDENTIFIER_TAG && rsl_data[1].type != RSL_NULL_TAG )
        return false;

    return true;
}

Data::Mission::Resource* Data::Mission::ACT::DynamicProp::duplicate() const {
    return new Data::Mission::ACT::DynamicProp( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::DynamicProp::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const DynamicProp*>( &original );

    if( copy_r != nullptr )
        return new Data::Mission::ACT::DynamicProp( *copy_r );
    else
        return new Data::Mission::ACT::DynamicProp( original );
}

Data::Mission::ACT::DynamicProp::Internal Data::Mission::ACT::DynamicProp::getInternal() const {
    return internal;
}
