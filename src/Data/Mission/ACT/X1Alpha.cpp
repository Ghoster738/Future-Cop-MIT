#include "X1Alpha.h"

#include "../ObjResource.h"
#include <cassert>

uint_fast8_t Data::Mission::ACT::X1Alpha::TYPE_ID = 1;

Json::Value Data::Mission::ACT::X1Alpha::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["uint32_0"] = internal.uint32_0;
    root["ACT"][NAME]["uint16_0"] = internal.uint16_0;
    root["ACT"][NAME]["uint16_1"] = internal.uint16_1;
    root["ACT"][NAME]["uint8_0"] = internal.uint8_0;
    root["ACT"][NAME]["uint8_1"] = internal.uint8_1;
    root["ACT"][NAME]["uint8_2"] = internal.uint8_2;
    root["ACT"][NAME]["uint8_3"] = internal.uint8_3;
    root["ACT"][NAME]["uint8_4"] = internal.uint8_4;
    root["ACT"][NAME]["zero_0"] = internal.zero_0;
    root["ACT"][NAME]["uint8_6"] = internal.uint8_6;
    root["ACT"][NAME]["zero_1"] = internal.zero_1;
    root["ACT"][NAME]["uint16_2"] = internal.uint16_2;
    root["ACT"][NAME]["uint8_8"] = internal.uint8_8;
    root["ACT"][NAME]["uint8_9"] = internal.uint8_9;

    return root;
}

bool Data::Mission::ACT::X1Alpha::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    internal.uint32_0 = data_reader.readU32( endian ); // Values: 64, 8256, 524352, 524864, 533056, 
    internal.uint16_0 = data_reader.readU16( endian ); // Values: 1000, 5000, 
    internal.uint16_1 = data_reader.readU16( endian ); // Values: 1, 5, 
    internal.uint8_0 = data_reader.readU8(); // Values: 1, 2, 31, 
    internal.uint8_1 = data_reader.readU8(); // Values: 0, 2, 
    internal.uint8_2 = data_reader.readU8(); // Values: 34, 39, 49, 50, 
    internal.uint8_3 = data_reader.readU8(); // Values: 5, 15, 
    internal.uint8_4 = data_reader.readU8(); // Values: 0, 10, 
    internal.zero_0 = data_reader.readU8(); // Always 0
    internal.uint8_6 = data_reader.readU8(); // Values: 0, 128, 129, 
    internal.zero_1 = data_reader.readU8(); // Always 0
    internal.uint16_2 = data_reader.readU16( endian ); // Values: 0, 1024, 1536, 2048, 3072, 61440, 62464, 63488, 64512, 65535, 
    internal.uint8_8 = data_reader.readU8(); // Values: 0, 1, 
    internal.uint8_9 = data_reader.readU8(); // Values: 0, 1, 255, 

    return true;
}

Data::Mission::ACT::X1Alpha::X1Alpha() {}

Data::Mission::ACT::X1Alpha::X1Alpha( const ACTResource& obj ) : ACTResource( obj ) {}

Data::Mission::ACT::X1Alpha::X1Alpha( const X1Alpha& obj ) : ACTResource( obj ), internal( obj.internal ) {}

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
