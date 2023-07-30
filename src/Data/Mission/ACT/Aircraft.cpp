#include "Aircraft.h"

#include "../ObjResource.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::Aircraft::TYPE_ID = 9;

Json::Value Data::Mission::ACT::Aircraft::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["rotation"] = internal.rotation;
    root["ACT"][NAME]["uint16_0"] = internal.uint16_0;
    root["ACT"][NAME]["uint16_1"] = internal.uint16_1;
    root["ACT"][NAME]["uint8_0"] = internal.uint8_0;
    root["ACT"][NAME]["uint8_1"] = internal.uint8_1;
    root["ACT"][NAME]["uint8_2"] = internal.uint8_2;
    root["ACT"][NAME]["uint8_3"] = internal.uint8_3;
    root["ACT"][NAME]["uint8_4"] = internal.uint8_4;
    root["ACT"][NAME]["uint8_5"] = internal.uint8_5;
    root["ACT"][NAME]["uint8_6"] = internal.uint8_6;
    root["ACT"][NAME]["uint8_7"] = internal.uint8_7;
    root["ACT"][NAME]["uint16_2"] = internal.uint16_2;
    root["ACT"][NAME]["uint8_8"] = internal.uint8_8;
    root["ACT"][NAME]["uint8_9"] = internal.uint8_9;
    root["ACT"][NAME]["uint8_10"] = internal.uint8_10;
    root["ACT"][NAME]["uint8_11"] = internal.uint8_11;
    root["ACT"][NAME]["uint16_3"] = internal.uint16_3;
    root["ACT"][NAME]["uint16_4"] = internal.uint16_4;
    root["ACT"][NAME]["uint16_5"] = internal.uint16_5;
    root["ACT"][NAME]["uint16_6"] = internal.uint16_6;
    root["ACT"][NAME]["uint16_7"] = internal.uint16_7;
    root["ACT"][NAME]["uint8_12"] = internal.uint8_12;
    root["ACT"][NAME]["uint8_13"] = internal.uint8_13;
    root["ACT"][NAME]["uint16_8"] = internal.uint16_8;
    root["ACT"][NAME]["uint8_14"] = internal.uint8_14;
    root["ACT"][NAME]["uint8_15"] = internal.uint8_15;
    root["ACT"][NAME]["uint16_9"] = internal.uint16_9;
    root["ACT"][NAME]["uint16_10"] = internal.uint16_10;
    root["ACT"][NAME]["uint16_11"] = internal.uint16_11;
    root["ACT"][NAME]["uint16_12"] = internal.uint16_12;
    root["ACT"][NAME]["uint16_13"] = internal.uint16_13;
    root["ACT"][NAME]["uint16_14"] = internal.uint16_14;
    root["ACT"][NAME]["uint16_15"] = internal.uint16_15;
    root["ACT"][NAME]["uint16_16"] = internal.uint16_16;
    root["ACT"][NAME]["uint16_17"] = internal.uint16_17;
    root["ACT"][NAME]["uint16_18"] = internal.uint16_18;
    root["ACT"][NAME]["uint16_19"] = internal.uint16_19;
    root["ACT"][NAME]["uint32_0"] = internal.uint32_0;

    return root;
}

bool Data::Mission::ACT::Aircraft::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;
    
    // TODO See if this is acurate information.
    internal.rotation = data_reader.readU32( endian );
    internal.uint16_0 = data_reader.readU16( endian );
    internal.uint16_1 = data_reader.readU16( endian );
    internal.uint8_0 = data_reader.readU8();
    internal.uint8_1 = data_reader.readU8();
    internal.uint8_2 = data_reader.readU8();
    internal.uint8_3 = data_reader.readU8();
    internal.uint8_4 = data_reader.readU8();
    internal.uint8_5 = data_reader.readU8();
    internal.uint8_6 = data_reader.readU8();
    internal.uint8_7 = data_reader.readU8();
    internal.uint16_2 = data_reader.readU16( endian );
    internal.uint8_8 = data_reader.readU8();
    internal.uint8_9 = data_reader.readU8();
    internal.uint8_10 = data_reader.readU8();
    internal.uint8_11 = data_reader.readU8();
    internal.uint16_3 = data_reader.readU16( endian );
    internal.uint16_4 = data_reader.readU16( endian );
    internal.uint16_5 = data_reader.readU16( endian );
    internal.uint16_6 = data_reader.readU16( endian );
    internal.uint16_7 = data_reader.readU16( endian );
    internal.uint8_12 = data_reader.readU8();
    internal.uint8_13 = data_reader.readU8();
    internal.uint16_8 = data_reader.readU16( endian );
    internal.uint8_14 = data_reader.readU8();
    internal.uint8_15 = data_reader.readU8();
    internal.uint16_9 = data_reader.readU16( endian );
    internal.uint16_10 = data_reader.readU16( endian );
    internal.uint16_11 = data_reader.readU16( endian );
    internal.uint16_12 = data_reader.readU16( endian );
    internal.uint16_13 = data_reader.readU16( endian );
    internal.uint16_14 = data_reader.readU16( endian );
    internal.uint16_15 = data_reader.readU16( endian );
    internal.uint16_16 = data_reader.readU16( endian );
    internal.uint16_17 = data_reader.readU16( endian );
    internal.uint16_18 = data_reader.readU16( endian );
    internal.uint16_19 = data_reader.readU16( endian );
    internal.uint32_0 = data_reader.readU32( endian );

    return true;
}

Data::Mission::ACT::Aircraft::Aircraft() {
}

Data::Mission::ACT::Aircraft::Aircraft( const ACTResource& obj ) : ACTResource( obj ) {
}

Data::Mission::ACT::Aircraft::Aircraft( const Aircraft& obj ) : ACTResource( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::Aircraft::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::Aircraft::getTypeIDName() const {
    return "Aircraft";
}

size_t Data::Mission::ACT::Aircraft::getSize() const {
    return 64; // bytes
}

bool Data::Mission::ACT::Aircraft::checkRSL() const {
    // rsl_data[0] is model
    // rsl_data[1] is probably destroyed model?
    return rsl_data.size() == 2 && rsl_data[0].type == ObjResource::IDENTIFIER_TAG && rsl_data[1].type == RSL_NULL_TAG;
}

Data::Mission::Resource* Data::Mission::ACT::Aircraft::duplicate() const {
    return new Data::Mission::ACT::Aircraft( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::Aircraft::duplicate( const ACTResource &original ) const {
    return new Data::Mission::ACT::Aircraft( original );
    auto copy_r = dynamic_cast<const Aircraft*>( &original );
    
    if( copy_r != nullptr)
        return new Data::Mission::ACT::Aircraft( *copy_r );
    else
        return new Data::Mission::ACT::Aircraft( original );
}

Data::Mission::ACT::Aircraft::Internal Data::Mission::ACT::Aircraft::getInternal() const {
    return internal;
}

std::vector<Data::Mission::ACT::Aircraft*> Data::Mission::ACT::Aircraft::getVector( Data::Mission::ACTManager& act_manager ) {
    std::vector<ACTResource*> to_copy = act_manager.getACTs( Data::Mission::ACT::Aircraft::TYPE_ID );

    std::vector<Aircraft*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<Aircraft*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::ACT::Aircraft*> Data::Mission::ACT::Aircraft::getVector( const Data::Mission::ACTManager& act_manager ) {
    return Data::Mission::ACT::Aircraft::getVector( const_cast< Data::Mission::ACTManager& >( act_manager ) );
}
