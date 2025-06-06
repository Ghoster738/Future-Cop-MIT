#include "ItemPickup.h"

#include "../ObjResource.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::ItemPickup::TYPE_ID = 16;

Json::Value Data::Mission::ACT::ItemPickup::makeJson() const {
    Json::Value root = BaseEntity::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["ground_cast_type"] = internal.ground_cast_type;
    root["ACT"][NAME]["uint8_1"] = internal.uint8_1;
    root["ACT"][NAME]["uint16_0"] = internal.uint16_0;
    root["ACT"][NAME]["uint16_1"] = internal.uint16_1;
    root["ACT"][NAME]["uint32_0"] = internal.uint32_0;
    root["ACT"][NAME]["uint16_2"] = internal.uint16_2;
    root["ACT"][NAME]["uint32_1"] = internal.uint32_1;
    root["ACT"][NAME]["bitfield"] = internal.bitfield;
    root["ACT"][NAME]["uint16_3"] = internal.uint16_3;
    root["ACT"][NAME]["rotational_speed"] = internal.rotational_speed;

    return root;
}

bool Data::Mission::ACT::ItemPickup::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    BaseEntity::readBase(data_reader, endian);

    internal.ground_cast_type = data_reader.readU8();  // Always 0
    internal.uint8_1 = data_reader.readU8();           // Always 1
    internal.uint16_0 = data_reader.readU16( endian ); // Always 1
    internal.uint16_1 = data_reader.readU16( endian ); // Always 81
    internal.uint32_0 = data_reader.readU32( endian ); // Always 53674803
    internal.uint16_2 = data_reader.readU16( endian ); // Always 2048
    internal.uint32_1 = data_reader.readU32( endian ); // Always 6488064
    internal.bitfield = data_reader.readU32( endian ); // Values: 0x03000001, 0x03000002, 0x03000004, 0x03000010, 0x03000020, 0x03000040, 0x03020000, 0x03040000,
    internal.uint16_3 = data_reader.readU16( endian ); // Always 65535
    internal.rotational_speed = data_reader.readI16( endian ); // Values: 1228, 60620,

    return true;
}

Data::Mission::ACT::ItemPickup::ItemPickup() {}

Data::Mission::ACT::ItemPickup::ItemPickup( const ACTResource& obj ) : BaseEntity( obj ) {}

Data::Mission::ACT::ItemPickup::ItemPickup( const BaseEntity& obj ) : BaseEntity( obj ) {}

Data::Mission::ACT::ItemPickup::ItemPickup( const ItemPickup& obj ) : BaseEntity( obj ), internal( obj.internal ) {}

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


bool Data::Mission::ACT::ItemPickup::hasBlink() const {
    Bitfield bitfield = getItemPickupBitfield();

    return (bitfield.power_up_gun | bitfield.power_up_heavy | bitfield.power_up_special) != 0;
}

Data::Mission::ACT::ItemPickup::Bitfield Data::Mission::ACT::ItemPickup::getItemPickupBitfield() const {
    Bitfield bitfield;

    bitfield.reload_gun       = (0x00000001 & internal.bitfield) != 0;
    bitfield.reload_heavy     = (0x00000002 & internal.bitfield) != 0;
    bitfield.reload_special   = (0x00000004 & internal.bitfield) != 0;
    // Skipped!
    bitfield.power_up_gun     = (0x00000010 & internal.bitfield) != 0;
    bitfield.power_up_heavy   = (0x00000020 & internal.bitfield) != 0;
    bitfield.power_up_special = (0x00000040 & internal.bitfield) != 0;
    // Skipped!


    // 8 bits Skipped


    // Skipped!
    bitfield.restore_health  = (0x00020000 & internal.bitfield) != 0;
    bitfield.invisibility    = (0x00040000 & internal.bitfield) != 0;
    bitfield.invincibility   = (0x00080000 & internal.bitfield) != 0;
    // Skipped!
    // Skipped!
    // Skipped!
    // Skipped!


    bitfield.is_set          = (0x01000000 & internal.bitfield) != 0;
    bitfield.pickup_consume  = (0x02000000 & internal.bitfield) != 0;

    // The remaining bits are skipped

    return bitfield;
}
