#include "Unknown.h"

#include "Internal/Hash.h"

uint_fast16_t Data::Mission::ACT::Unknown::TYPE_ID = 0; // Zero is not used by any ACT types that future cop uses.

bool Data::Mission::ACT::Unknown::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    if( data_reader.totalSize() >= 4 )
    {
        this->act_type = act_type;
        this->act_size = data_reader.totalSize() - 4;
        return true;
    }
    else
        return false;
}

Json::Value Data::Mission::ACT::Unknown::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();

    root["ACT"]["type"] = static_cast<uint32_t>( this->act_type );
    root["ACT"]["size"] = static_cast<uint32_t>( this->act_size );

    return root;
}

Data::Mission::ACT::Unknown::Unknown() : act_type( Data::Mission::ACT::Unknown::TYPE_ID ) {
}

Data::Mission::ACT::Unknown::Unknown( const ACTResource &obj ) : ACTResource( obj ) {
}

Data::Mission::ACT::Unknown::Unknown( const Unknown &obj ) : ACTResource( obj ), act_type( obj.act_type ), act_size( obj.act_size ) {
}

uint_fast8_t Data::Mission::ACT::Unknown::getTypeID() const {
    return this->act_type;
};

std::string Data::Mission::ACT::Unknown::getTypeIDName() const {
    return "Unknown: {T:" + std::to_string( getTypeID() ) + ",S:" + std::to_string( getSize() ) + "}";
};

size_t Data::Mission::ACT::Unknown::getSize() const {
    return this->act_size;
}

bool Data::Mission::ACT::Unknown::checkRSL() const {
    return true;
}

Data::Mission::Resource* Data::Mission::ACT::Unknown::duplicate() const {
    return new Unknown( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::Unknown::duplicate( const ACTResource &original ) const {
    return new Unknown( original );
}
