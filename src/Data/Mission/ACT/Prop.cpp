#include "Prop.h"

#include "../ObjResource.h"

#include <cassert>
#include <glm/gtc/constants.hpp>

uint_fast8_t Data::Mission::ACT::Prop::TYPE_ID = 96;

Json::Value Data::Mission::ACT::Prop::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["int16_2"] = internal.uint16_2;
    root["ACT"][NAME]["angle?"]  = internal.uint16_3;
    root["ACT"][NAME]["byte 0"]  = internal.byte_0;
    root["ACT"][NAME]["layer?"]  = internal.byte_1;
    root["ACT"][NAME]["byte 2"]  = internal.byte_2;
    root["ACT"][NAME]["spin?"]   = internal.spin;
    root["ACT"][NAME]["byte 3"]  = internal.byte_3;

    return root;
}

bool Data::Mission::ACT::Prop::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    data_reader.readU32(); // Ignore rotation.

    internal.uint16_2 = data_reader.readU16( endian );
    internal.uint16_3 = data_reader.readU16( endian );

    internal.byte_0 = data_reader.readU8();
    internal.byte_1 = data_reader.readU8();
    internal.byte_2 = data_reader.readU8();

    internal.matching_bytes[0] = data_reader.readU8();
    internal.matching_bytes[1] = data_reader.readU8();
    internal.matching_bytes[2] = data_reader.readU8();

    internal.spin = data_reader.readU8();

    internal.byte_3 = data_reader.readU8();

    return true;
}

Data::Mission::ACT::Prop::Prop() {
}

Data::Mission::ACT::Prop::Prop( const ACTResource& obj ) : ACTResource( obj ) {
}

Data::Mission::ACT::Prop::Prop( const Prop& obj ) : ACTResource( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::Prop::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::Prop::getTypeIDName() const {
    return "Prop";
}

size_t Data::Mission::ACT::Prop::getSize() const {
    return 16; // bytes
}

bool Data::Mission::ACT::Prop::checkRSL() const {
    return rsl_data.size() == 1 && rsl_data[0].type == Data::Mission::ObjResource::IDENTIFIER_TAG;
}

Data::Mission::Resource* Data::Mission::ACT::Prop::duplicate() const {
    return new Data::Mission::ACT::Prop( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::Prop::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const Prop*>( &original );
    
    if( copy_r != nullptr)
        return new Data::Mission::ACT::Prop( *copy_r );
    else
        return new Data::Mission::ACT::Prop( original );
}

Data::Mission::ACT::Prop::Internal Data::Mission::ACT::Prop::getInternal() const {
    return internal;
}

std::vector<Data::Mission::ACT::Prop*> Data::Mission::ACT::Prop::getVector( Data::Mission::ACTManager& act_manager ) {
    std::vector<ACTResource*> to_copy = act_manager.getACTs( Data::Mission::ACT::Prop::TYPE_ID );

    std::vector<Prop*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<Prop*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::ACT::Prop*> Data::Mission::ACT::Prop::getVector( const Data::Mission::ACTManager& act_manager ) {
    return Data::Mission::ACT::Prop::getVector( const_cast< Data::Mission::ACTManager& >( act_manager ) );
}
