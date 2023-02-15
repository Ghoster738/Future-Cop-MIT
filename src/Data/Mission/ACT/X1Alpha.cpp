#include "X1Alpha.h"

#include "../ObjResource.h"

#include "../../../Utilities/DataHandler.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::X1AUnit::TYPE_ID = 1;

Json::Value Data::Mission::ACT::X1AUnit::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["rotation_value"] = internal.rotation_value;
    root["ACT"][NAME]["uint16_0"] = internal.uint16_0;
    root["ACT"][NAME]["uint16_1"] = internal.uint16_1;
    root["ACT"][NAME]["byte_0"] = internal.byte_0;
    root["ACT"][NAME]["byte_1"] = internal.byte_1;
    root["ACT"][NAME]["byte_2"] = internal.byte_2;
    root["ACT"][NAME]["byte_3"] = internal.byte_3;
    root["ACT"][NAME]["byte_4"] = internal.byte_4;
    root["ACT"][NAME]["byte_5"] = internal.byte_5;
    root["ACT"][NAME]["uint16_2"] = internal.uint16_2;
    root["ACT"][NAME]["byte_6"] = internal.byte_6;

    return root;
}

bool Data::Mission::ACT::X1AUnit::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());
    assert(data_reader.totalSize() == this->getSize());
    
    internal.rotation_value = data_reader.readU32( endian );
    internal.uint16_0 = data_reader.readU16( endian );
    internal.uint16_1 = data_reader.readU16( endian );
    internal.byte_0 = data_reader.readU8();
    internal.byte_1 = data_reader.readU8();
    internal.byte_2 = data_reader.readU8();
    internal.byte_3 = data_reader.readU8();
    internal.byte_4 = data_reader.readU8();
    data_reader.readU8();
    internal.byte_5 = data_reader.readU8();
    data_reader.readU8();
    internal.uint16_2 = data_reader.readU16( endian );
    data_reader.readU8();
    internal.byte_6 = data_reader.readU8();

    return true;
}

Data::Mission::ACT::X1AUnit::X1AUnit() {
}

Data::Mission::ACT::X1AUnit::X1AUnit( const ACTResource& obj ) : ACTResource( obj ) {
}

Data::Mission::ACT::X1AUnit::X1AUnit( const X1AUnit& obj ) : ACTResource( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::X1AUnit::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::X1AUnit::getTypeIDName() const {
    return "X1AUnit";
}

size_t Data::Mission::ACT::X1AUnit::getSize() const {
    return 20; // bytes
}

bool Data::Mission::ACT::X1AUnit::checkRSL() const {
    // rsl_data[0] is legs
    // rsl_data[1] have might be something to do with death? However, it is not used.
    // rsl_data[2] is cockpit
    // rsl_data[3] is gun
    // rsl_data[4] is lights
    // rsl_data[5] is the pilot of X1A only seen when X1A is destoryed
    return rsl_data.size() == 6 && rsl_data[0].type == ObjResource::IDENTIFIER_TAG && rsl_data[1].type == RSL_NULL_TAG && rsl_data[2].type == ObjResource::IDENTIFIER_TAG && rsl_data[3].type == ObjResource::IDENTIFIER_TAG && rsl_data[4].type == ObjResource::IDENTIFIER_TAG && rsl_data[5].type == ObjResource::IDENTIFIER_TAG;
}

Data::Mission::Resource* Data::Mission::ACT::X1AUnit::duplicate() const {
    return new Data::Mission::ACT::X1AUnit( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::X1AUnit::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const X1AUnit*>( &original );
    
    if( copy_r != nullptr)
        return new Data::Mission::ACT::X1AUnit( *copy_r );
    else
        return new Data::Mission::ACT::X1AUnit( original );
}

Data::Mission::ACT::X1AUnit::Internal Data::Mission::ACT::X1AUnit::getInternal() const {
    return internal;
}

std::vector<Data::Mission::ACT::X1AUnit*> Data::Mission::ACT::X1AUnit::getVector( Data::Mission::ACTManager& act_manager ) {
    std::vector<ACTResource*> to_copy = act_manager.getACTs( Data::Mission::ACT::X1AUnit::TYPE_ID );

    std::vector<X1AUnit*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<X1AUnit*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::ACT::X1AUnit*> Data::Mission::ACT::X1AUnit::getVector( const Data::Mission::ACTManager& act_manager ) {
    return Data::Mission::ACT::X1AUnit::getVector( const_cast< Data::Mission::ACTManager& >( act_manager ) );
}
