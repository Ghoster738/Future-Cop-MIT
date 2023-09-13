#include "Trigger.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::Trigger::TYPE_ID = 95;

Json::Value Data::Mission::ACT::Trigger::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["width"]    = internal.width;
    root["ACT"][NAME]["height"]   = internal.height;
    root["ACT"][NAME]["uint16_2"] = internal.uint16_2; // Always 2048
    root["ACT"][NAME]["uint8_0"]  = internal.uint8_0; // Always 0
    root["ACT"][NAME]["type"]     = internal.type;
    root["ACT"][NAME]["actor_id"] = internal.actor_id;
    root["ACT"][NAME]["uint16_4"] = internal.uint16_4; // Always 0

    return root;
}

bool Data::Mission::ACT::Trigger::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    internal.width    = data_reader.readU16( endian );
    internal.height   = data_reader.readU16( endian );
    internal.uint16_2 = data_reader.readU16( endian ); // Always 2048

    internal.uint8_0  = data_reader.readU8(); // Always 0
    internal.type     = data_reader.readU8();

    internal.actor_id = data_reader.readU16( endian );
    internal.uint16_4 = data_reader.readU16( endian ); // Always 0

    return true;
}

Data::Mission::ACT::Trigger::Trigger() {}

Data::Mission::ACT::Trigger::Trigger( const ACTResource& obj ) : ACTResource( obj ) {}

Data::Mission::ACT::Trigger::Trigger( const Trigger& obj ) : ACTResource( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::Trigger::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::Trigger::getTypeIDName() const {
    return "Trigger";
}

size_t Data::Mission::ACT::Trigger::getSize() const {
    return 12; // bytes
}

bool Data::Mission::ACT::Trigger::checkRSL() const {
    return rsl_data.size() == 1 && rsl_data[0].type == Data::Mission::ACTResource::RSL_NULL_TAG;
}

Data::Mission::Resource* Data::Mission::ACT::Trigger::duplicate() const {
    return new Data::Mission::ACT::Trigger( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::Trigger::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const Trigger*>( &original );

    if( copy_r != nullptr )
        return new Data::Mission::ACT::Trigger( *copy_r );
    else
        return new Data::Mission::ACT::Trigger( original );
}

Data::Mission::ACT::Trigger::Internal Data::Mission::ACT::Trigger::getInternal() const {
    return internal;
}

std::vector<Data::Mission::ACT::Trigger*> Data::Mission::ACT::Trigger::getVector( Data::Mission::ACTManager& act_manager ) {
    std::vector<ACTResource*> to_copy = act_manager.getACTs( Data::Mission::ACT::Trigger::TYPE_ID );

    std::vector<Trigger*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<Trigger*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::ACT::Trigger*> Data::Mission::ACT::Trigger::getVector( const Data::Mission::ACTManager& act_manager ) {
    return Data::Mission::ACT::Trigger::getVector( const_cast< Data::Mission::ACTManager& >( act_manager ) );
}
