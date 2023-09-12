#include "SkyCaptain.h"

#include "../ObjResource.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::SkyCaptain::TYPE_ID = 37;

Json::Value Data::Mission::ACT::SkyCaptain::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["uint16_0"]      = internal.uint16_0;
    root["ACT"][NAME]["bytes_0"][0]    = internal.bytes_0[0];
    root["ACT"][NAME]["bytes_0"][1]    = internal.bytes_0[1];
    root["ACT"][NAME]["bytes_0"][2]    = internal.bytes_0[2];
    root["ACT"][NAME]["bytes_0"][3]    = internal.bytes_0[3];
    root["ACT"][NAME]["bytes_0"][4]    = internal.bytes_0[4];
    root["ACT"][NAME]["bytes_0"][5]    = internal.bytes_0[5];
    root["ACT"][NAME]["uint16_1"][0]   = internal.uint16_1[0];
    root["ACT"][NAME]["uint16_1"][1]   = internal.uint16_1[1];
    root["ACT"][NAME]["bytes_1"][0]    = internal.bytes_1[0];
    root["ACT"][NAME]["bytes_1"][1]    = internal.bytes_1[1];
    root["ACT"][NAME]["uint16_2"]      = internal.uint16_2;
    root["ACT"][NAME]["uint32_1"]      = internal.uint32_1;
    root["ACT"][NAME]["uint16_3"][0]   = internal.uint16_3[0];
    root["ACT"][NAME]["uint16_3"][1]   = internal.uint16_3[1];
    root["ACT"][NAME]["bytes_2"][0]    = internal.bytes_2[0];
    root["ACT"][NAME]["bytes_2"][1]    = internal.bytes_2[1];
    root["ACT"][NAME]["uint16_4"]      = internal.uint16_4;
    root["ACT"][NAME]["bytes_3"][0]    = internal.bytes_3[0];
    root["ACT"][NAME]["bytes_3"][1]    = internal.bytes_3[1];
    root["ACT"][NAME]["uint16_5"][0]   = internal.uint16_5[0];
    root["ACT"][NAME]["uint16_5"][1]   = internal.uint16_5[1];
    root["ACT"][NAME]["uint16_5"][2]   = internal.uint16_5[2];
    root["ACT"][NAME]["uint16_5"][3]   = internal.uint16_5[3];
    root["ACT"][NAME]["uint16_5"][4]   = internal.uint16_5[4];
    root["ACT"][NAME]["uint16_5"][5]   = internal.uint16_5[5];
    root["ACT"][NAME]["uint16_5"][6]   = internal.uint16_5[6];
    root["ACT"][NAME]["uint32_2"]      = internal.uint32_2;
    root["ACT"][NAME]["uint16_6"][0]   = internal.uint16_6[0];
    root["ACT"][NAME]["uint16_6"][1]   = internal.uint16_6[1];
    root["ACT"][NAME]["byte_4"]        = internal.byte_4;
    root["ACT"][NAME]["uint16_7"][0]   = internal.uint16_7[0];
    root["ACT"][NAME]["uint16_7"][1]   = internal.uint16_7[1];
    root["ACT"][NAME]["bytes_5"][0]    = internal.bytes_5[0];
    root["ACT"][NAME]["bytes_5"][1]    = internal.bytes_5[1];
    root["ACT"][NAME]["uint16_8"]      = internal.uint16_8;
    root["ACT"][NAME]["uint16_9"][0]   = internal.uint16_9[0];
    root["ACT"][NAME]["uint16_9"][1]   = internal.uint16_9[1];
    root["ACT"][NAME]["uint16_9"][2]   = internal.uint16_9[2];
    root["ACT"][NAME]["uint16_9"][3]   = internal.uint16_9[3];
    root["ACT"][NAME]["bytes_6"][0]    = internal.bytes_6[0];
    root["ACT"][NAME]["bytes_6"][1]    = internal.bytes_6[1];
    root["ACT"][NAME]["uint16_10"]     = internal.uint16_10;
    root["ACT"][NAME]["uint32_3"]      = internal.uint32_3;
    root["ACT"][NAME]["uint16_11"][0]  = internal.uint16_11[0];
    root["ACT"][NAME]["uint16_11"][1]  = internal.uint16_11[1];
    

    return root;
}

bool Data::Mission::ACT::SkyCaptain::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if(data_reader.totalSize() != this->getSize())
        return false;

    data_reader.readU16(); // Ignore rotation.
    
    internal.uint16_12 = data_reader.readU16( endian );
    internal.uint16_0 = data_reader.readU16( endian );
    data_reader.readU16(); // 2 zero bytes
    internal.bytes_0[ 0 ] = data_reader.readU8();
    internal.bytes_0[ 1 ] = data_reader.readU8();
    internal.bytes_0[ 2 ] = data_reader.readU8();
    internal.bytes_0[ 3 ] = data_reader.readU8();
    internal.bytes_0[ 4 ] = data_reader.readU8();
    internal.bytes_0[ 5 ] = data_reader.readU8();;
    data_reader.readU16(); // 2 zero bytes
    internal.uint16_1[ 0 ] = data_reader.readU16( endian );
    internal.uint16_1[ 1 ] = data_reader.readU16( endian );
    internal.bytes_1[ 0 ] = data_reader.readU8();
    internal.bytes_1[ 1 ] = data_reader.readU8();
    internal.uint16_2 = data_reader.readU16( endian );
    internal.uint32_1 = data_reader.readU32( endian );
    internal.uint16_3[ 0 ] = data_reader.readU16( endian );
    internal.uint16_3[ 1 ] = data_reader.readU16( endian );
    internal.bytes_2[ 0 ] = data_reader.readU8();
    internal.bytes_2[ 1 ] = data_reader.readU8();
    internal.uint16_4 = data_reader.readU16( endian );
    internal.bytes_3[ 0 ] = data_reader.readU8();
    internal.bytes_3[ 1 ] = data_reader.readU8();
    internal.uint16_5[ 0 ] = data_reader.readU16( endian );
    internal.uint16_5[ 1 ] = data_reader.readU16( endian );
    internal.uint16_5[ 2 ] = data_reader.readU16( endian );
    internal.uint16_5[ 3 ] = data_reader.readU16( endian );
    internal.uint16_5[ 4 ] = data_reader.readU16( endian );
    internal.uint16_5[ 5 ] = data_reader.readU16( endian );
    internal.uint16_5[ 6 ] = data_reader.readU16( endian );
    internal.uint32_2 = data_reader.readU32( endian );
    internal.uint16_6[ 0 ] = data_reader.readU16( endian );
    internal.uint16_6[ 1 ] = data_reader.readU16( endian );
    data_reader.readU32(); // 4 zero bytes
    internal.byte_4 = data_reader.readU8();
    data_reader.readU8();
    data_reader.readU8();
    data_reader.readU8(); // 3 zero bytes
    internal.uint16_7[ 0 ] = data_reader.readU16( endian );
    internal.uint16_7[ 1 ] = data_reader.readU16( endian );
    internal.bytes_5[ 0 ] = data_reader.readU8();
    internal.bytes_5[ 1 ] = data_reader.readU8();
    internal.uint16_8 = data_reader.readU16( endian );
    data_reader.readU32(); // 4 zero bytes
    internal.uint16_9[ 0 ] = data_reader.readU16( endian );
    internal.uint16_9[ 1 ] = data_reader.readU16( endian );
    internal.uint16_9[ 2 ] = data_reader.readU16( endian );
    internal.uint16_9[ 3 ] = data_reader.readU16( endian );
    internal.bytes_6[ 0 ] = data_reader.readU8();
    internal.bytes_6[ 1 ] = data_reader.readU8();
    internal.uint16_10 = data_reader.readU16( endian );
    internal.uint32_3 = data_reader.readU32( endian );
    internal.uint16_11[ 0 ] = data_reader.readU16( endian );
    internal.uint16_11[ 1 ] = data_reader.readU16( endian );

    return true;
}

Data::Mission::ACT::SkyCaptain::SkyCaptain() {
}

Data::Mission::ACT::SkyCaptain::SkyCaptain( const ACTResource& obj ) : ACTResource( obj ) {
}

Data::Mission::ACT::SkyCaptain::SkyCaptain( const SkyCaptain& obj ) : ACTResource( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::SkyCaptain::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::SkyCaptain::getTypeIDName() const {
    return "SkyCaptain";
}

size_t Data::Mission::ACT::SkyCaptain::getSize() const {
    return 100; // bytes
}

bool Data::Mission::ACT::SkyCaptain::checkRSL() const {
    return rsl_data.size() == 2 && rsl_data[0].type == Data::Mission::ObjResource::IDENTIFIER_TAG && rsl_data[1].type == RSL_NULL_TAG;
}

Data::Mission::Resource* Data::Mission::ACT::SkyCaptain::duplicate() const {
    return new Data::Mission::ACT::SkyCaptain( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::SkyCaptain::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const SkyCaptain*>( &original );
    
    if( copy_r != nullptr)
        return new Data::Mission::ACT::SkyCaptain( *copy_r );
    else
        return new Data::Mission::ACT::SkyCaptain( original );
}

Data::Mission::ACT::SkyCaptain::Internal Data::Mission::ACT::SkyCaptain::getInternal() const {
    return internal;
}

std::vector<Data::Mission::ACT::SkyCaptain*> Data::Mission::ACT::SkyCaptain::getVector( Data::Mission::ACTManager& act_manager ) {
    std::vector<ACTResource*> to_copy = act_manager.getACTs( Data::Mission::ACT::SkyCaptain::TYPE_ID );

    std::vector<SkyCaptain*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<SkyCaptain*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::ACT::SkyCaptain*> Data::Mission::ACT::SkyCaptain::getVector( const Data::Mission::ACTManager& act_manager ) {
    return Data::Mission::ACT::SkyCaptain::getVector( const_cast< Data::Mission::ACTManager& >( act_manager ) );
}
