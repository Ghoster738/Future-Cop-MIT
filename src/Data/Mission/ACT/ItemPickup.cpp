#include "ItemPickup.h"

#include "../ObjResource.h"

#include <cassert>

namespace {
const uint32_t BITFIELD_RELOAD_GUN     = 0x00000001;
const uint32_t BITFIELD_RELOAD_HEAVY   = 0x00000002;
const uint32_t BITFIELD_RELOAD_SPECIAL = 0x00000004;
const uint32_t BITFIELD_POWER_GUN      = 0x00000010;
const uint32_t BITFIELD_POWER_HEAVY    = 0x00000020;
const uint32_t BITFIELD_POWER_SPECIAL  = 0x00000040;
const uint32_t BITFIELD_RESTORE_HEALTH = 0x00020000;
const uint32_t BITFIELD_INVISIBILITY   = 0x00040000;
const uint32_t BITFIELD_INVINCIBILITY  = 0x00080000;
const uint32_t BITFIELD_IS_SET         = 0x01000000; // If not set then it is added.
const uint32_t BITFIELD_PICKUP_CONSUME = 0x02000000;
}

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
    root["ACT"][NAME]["bitfield"] = internal.bitfield;
    root["ACT"][NAME]["uint16_5"] = internal.uint16_5;
    root["ACT"][NAME]["rotation"] = internal.rotation;

    return root;
}

bool Data::Mission::ACT::ItemPickup::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    internal.uint32_0 = data_reader.readU32( endian ); // Always 0x00080081
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
    internal.bitfield = data_reader.readU32( endian ); // Values: 0x03000001, 0x03000002, 0x03000004, 0x03000010, 0x03000020, 0x03000040, 0x03020000, 0x03040000,
    internal.uint16_5 = data_reader.readU16( endian ); // Always 65535
    internal.rotation = data_reader.readU16( endian ); // Values: 1228, 60620,

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
