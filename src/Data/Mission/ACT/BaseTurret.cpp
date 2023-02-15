#include "BaseTurret.h"

#include "../ObjResource.h"

#include "../../../Utilities/DataHandler.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::BaseTurret::TYPE_ID = 8;

Json::Value Data::Mission::ACT::BaseTurret::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["rotation"] = internal.rotation;
    root["ACT"][NAME]["uint16_0"] = internal.uint16_0;
    root["ACT"][NAME]["byte_0"] = internal.byte_0;
    root["ACT"][NAME]["byte_1"] = internal.byte_1;
    root["ACT"][NAME]["byte_2"] = internal.byte_2;
    root["ACT"][NAME]["uint16_1"] = internal.uint16_1;
    root["ACT"][NAME]["uint16_2"] = internal.uint16_2;
    root["ACT"][NAME]["byte_3"] = internal.byte_3;
    root["ACT"][NAME]["byte_4"] = internal.byte_4;
    root["ACT"][NAME]["uint16_3"] = internal.uint16_3;
    root["ACT"][NAME]["uint16_4"] = internal.uint16_4;
    root["ACT"][NAME]["byte_5"] = internal.byte_5;
    root["ACT"][NAME]["uint16_5"] = internal.uint16_5;
    root["ACT"][NAME]["uint16_6"] = internal.uint16_6;
    root["ACT"][NAME]["uint16_7"] = internal.uint16_7;
    root["ACT"][NAME]["uint16_8"] = internal.uint16_8;
    root["ACT"][NAME]["byte_6"] = internal.byte_6;
    root["ACT"][NAME]["uint16_9"] = internal.uint16_9;

    return root;
}

bool Data::Mission::ACT::BaseTurret::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());
    assert(data_reader.totalSize() == this->getSize());
    
    internal.rotation = data_reader.readU32( endian );
    internal.uint16_0 = data_reader.readU16( endian );
    
    data_reader.readU16( endian ); // not zero!
    
    internal.byte_0 = data_reader.readU8();
    
    data_reader.readU8(); // not zero!
    
    internal.byte_1 = data_reader.readU8();
    
    data_reader.readU8(); // not zero!
    
    internal.byte_2 = data_reader.readU8();
    
    data_reader.readU8(); // not zero!
    data_reader.readU16( endian ); // not zero!
    
    internal.uint16_1 = data_reader.readU16( endian );
    internal.uint16_2 = data_reader.readU16( endian );
    internal.byte_3 = data_reader.readU8();
    internal.byte_4 = data_reader.readU8();
    internal.uint16_3 = data_reader.readU16( endian );
    
    data_reader.readU32( endian ); // not zero
    
    internal.uint16_4 = data_reader.readU16( endian );
    
    data_reader.readU16( endian ); // not zero
    data_reader.readU8(); // not zero
    
    internal.byte_5 = data_reader.readU8();
    internal.uint16_5 = data_reader.readU16( endian );
    internal.uint16_6 = data_reader.readU16( endian );
    
    data_reader.readU16( endian ); // not zero
    
    internal.uint16_7 = data_reader.readU16( endian );
    internal.uint16_8 = data_reader.readU16( endian );
    internal.byte_6 = data_reader.readU8();
    
    data_reader.readU8(); // this is in fact a zero byte
    data_reader.readU32( endian ); // not zero
    
    internal.uint16_9 = data_reader.readU16( endian );

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
    return 52; // bytes
}

bool Data::Mission::ACT::BaseTurret::checkRSL() const {
    // rsl_data[0] has alive gun.
    // rsl_data[1] has destroyed gun?
    // rsl_data[2] has alive base.
    // rsl_data[3] has destroyed base.
    return rsl_data.size() == 4 && rsl_data[0].type == ObjResource::IDENTIFIER_TAG && rsl_data[1].type == RSL_NULL_TAG && rsl_data[2].type == ObjResource::IDENTIFIER_TAG && (rsl_data[3].type == RSL_NULL_TAG || rsl_data[3].type == ObjResource::IDENTIFIER_TAG);
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
