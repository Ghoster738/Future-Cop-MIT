#include "BaseTurret.h"

#include "../ObjResource.h"

#include "../../../Utilities/DataHandler.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::BaseTurret::TYPE_ID = 8;

Json::Value Data::Mission::ACT::BaseTurret::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = getTypeIDName();

    /*root["ACT"][NAME]["rotation_value"] = internal.rotation_value;
    root["ACT"][NAME]["uint16_0"] = internal.uint16_0;
    root["ACT"][NAME]["uint16_1"] = internal.uint16_1;
    root["ACT"][NAME]["byte_0"] = internal.byte_0;
    root["ACT"][NAME]["byte_1"] = internal.byte_1;
    root["ACT"][NAME]["byte_2"] = internal.byte_2;
    root["ACT"][NAME]["byte_3"] = internal.byte_3;
    root["ACT"][NAME]["byte_4"] = internal.byte_4;
    root["ACT"][NAME]["byte_5"] = internal.byte_5;
    root["ACT"][NAME]["uint16_2"] = internal.uint16_2;
    root["ACT"][NAME]["byte_6"] = internal.byte_6;*/

    return root;
}

#include <iostream>

bool Data::Mission::ACT::BaseTurret::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());
    // assert(data_reader.totalSize() == this->getSize());
    
    while( !data_reader.ended() ) {
        auto byte = data_reader.readU8();
        std::cout << std::hex << ((byte >> 8) & 0xF) << (byte & 0xF) << std::dec << " ";
    }
    std::cout << std::endl;

    return true;
}

Data::Mission::ACT::BaseTurret::BaseTurret() {
}

Data::Mission::ACT::BaseTurret::BaseTurret( const ACTResource& obj ) : ACTResource( obj ) {
}

Data::Mission::ACT::BaseTurret::BaseTurret( const BaseTurret& obj ) : ACTResource( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::BaseTurret::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::BaseTurret::getTypeIDName() const {
    return "BaseTurret";
}

size_t Data::Mission::ACT::BaseTurret::getSize() const {
    return 20; // bytes
}

bool Data::Mission::ACT::BaseTurret::checkRSL() const {
    return true; //return rsl_data.size() == 1 && rsl_data[0].type == Data::Mission::ObjResource::IDENTIFIER_TAG;
}

Data::Mission::Resource* Data::Mission::ACT::BaseTurret::duplicate() const {
    return new Data::Mission::ACT::BaseTurret( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::BaseTurret::duplicate( const ACTResource &original ) const {
    return new Data::Mission::ACT::BaseTurret( original );
    auto copy_r = dynamic_cast<const BaseTurret*>( &original );
    
    if( copy_r != nullptr)
        return new Data::Mission::ACT::BaseTurret( *copy_r );
    else
        return new Data::Mission::ACT::BaseTurret( original );
}

Data::Mission::ACT::BaseTurret::Internal Data::Mission::ACT::BaseTurret::getInternal() const {
    return internal;
}

std::vector<Data::Mission::ACT::BaseTurret*> Data::Mission::ACT::BaseTurret::getVector( Data::Mission::ACTManager& act_manager ) {
    std::vector<ACTResource*> to_copy = act_manager.getACTs( Data::Mission::ACT::BaseTurret::TYPE_ID );

    std::vector<BaseTurret*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<BaseTurret*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::ACT::BaseTurret*> Data::Mission::ACT::BaseTurret::getVector( const Data::Mission::ACTManager& act_manager ) {
    return Data::Mission::ACT::BaseTurret::getVector( const_cast< Data::Mission::ACTManager& >( act_manager ) );
}
