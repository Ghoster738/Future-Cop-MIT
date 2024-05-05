#include "ItemPickup.h"

#include "../ObjResource.h"

#include <iostream>
#include <cassert>

uint_fast8_t Data::Mission::ACT::ItemPickup::TYPE_ID = 16;

Json::Value Data::Mission::ACT::ItemPickup::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["uint32_0"] = internal.uint32_0;
    root["ACT"][NAME]["uint32_1"] = internal.uint32_1;
    root["ACT"][NAME]["uint16_0"] = internal.uint16_0;
    root["ACT"][NAME]["uint8_0"] = internal.uint8_0;
    root["ACT"][NAME]["uint32_2"] = internal.uint32_2;
    root["ACT"][NAME]["uint16_1"] = internal.uint16_1;
    root["ACT"][NAME]["uint8_1"] = internal.uint8_1;
    root["ACT"][NAME]["uint16_2"] = internal.uint16_2;
    root["ACT"][NAME]["uint16_3"] = internal.uint16_3;
    root["ACT"][NAME]["uint32_3"] = internal.uint32_3;
    root["ACT"][NAME]["uint16_4"] = internal.uint16_4;
    root["ACT"][NAME]["uint32_4"] = internal.uint32_4;
    root["ACT"][NAME]["type"] = internal.type;
    root["ACT"][NAME]["uint16_5"] = internal.uint16_5;
    root["ACT"][NAME]["unknown"] = internal.unknown;

    return root;
}

bool Data::Mission::ACT::ItemPickup::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    internal.uint32_0 = data_reader.readU32( endian ); // Always 524417
    internal.uint32_1 = data_reader.readU32( endian ); // Always 0
    internal.uint16_0 = data_reader.readU16( endian ); // Always 0
    internal.uint8_0 = data_reader.readU8(); // Always 5
    internal.uint32_2 = data_reader.readU32( endian ); // Always 0
    internal.uint16_1 = data_reader.readU16( endian ); // Always 0
    internal.uint8_1 = data_reader.readU8(); // Always 1
    internal.uint16_2 = data_reader.readU16( endian ); // Always 1
    internal.uint16_3 = data_reader.readU16( endian ); // Always 81
    internal.uint32_3 = data_reader.readU32( endian ); // Always 53674803
    internal.uint16_4 = data_reader.readU16( endian ); // Always 2048
    internal.uint32_4 = data_reader.readU32( endian ); // Always 6488064
    internal.type = data_reader.readU32( endian ); // Values: 50331649, 50331650, 50331652, 50331664, 50331680, 50331712, 50462720, 50593792,
    internal.uint16_5 = data_reader.readU16( endian ); // Always 65535
    internal.unknown = data_reader.readU16( endian ); // Values: 1228, 60620,

    std::cout << "PlaceItemPickup(" << std::dec << getID() << ")" << std::endl;

    return true;
}

Data::Mission::ACT::ItemPickup::ItemPickup() {}

Data::Mission::ACT::ItemPickup::ItemPickup( const ACTResource& obj ) : ACTResource( obj ) {}

Data::Mission::ACT::ItemPickup::ItemPickup( const ItemPickup& obj ) : ACTResource( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::ItemPickup::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::ItemPickup::getTypeIDName() const {
    return "ItemPickup";
}

size_t Data::Mission::ACT::ItemPickup::getSize() const {
    return 40; // bytes
}

bool Data::Mission::ACT::ItemPickup::checkRSL() const {
    if( rsl_data.size() != 2 )
        return false;
    else
    if( rsl_data[0].type != ObjResource::IDENTIFIER_TAG )
        return false;
    else
    if( rsl_data[1].type != RSL_NULL_TAG )
        return false;
    else
        return true;
}

Data::Mission::Resource* Data::Mission::ACT::ItemPickup::duplicate() const {
    return new Data::Mission::ACT::ItemPickup( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::ItemPickup::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const ItemPickup*>( &original );

    if( copy_r != nullptr )
        return new Data::Mission::ACT::ItemPickup( *copy_r );
    else
        return new Data::Mission::ACT::ItemPickup( original );
}

Data::Mission::ACT::ItemPickup::Internal Data::Mission::ACT::ItemPickup::getInternal() const {
    return internal;
}

std::vector<Data::Mission::ACT::ItemPickup*> Data::Mission::ACT::ItemPickup::getVector( Data::Mission::ACTManager& act_manager ) {
    std::vector<ACTResource*> to_copy = act_manager.getACTs( Data::Mission::ACT::ItemPickup::TYPE_ID );

    std::vector<ItemPickup*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<ItemPickup*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::ACT::ItemPickup*> Data::Mission::ACT::ItemPickup::getVector( const Data::Mission::ACTManager& act_manager ) {
    return Data::Mission::ACT::ItemPickup::getVector( const_cast< Data::Mission::ACTManager& >( act_manager ) );
}
