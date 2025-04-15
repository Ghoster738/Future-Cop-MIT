#include "BaseEntity.h"

namespace {
    uint32_t BITFIELD_DISABLE_RENDERING = 0x00002000;
}

namespace Data::Mission::ACT {

BaseEntity::MapIconColor BaseEntity::getMapIconColor( uint8_t map_icon_bitfield ) {
    const uint8_t color_index = (map_icon_bitfield & 0b0011111 >> 0);

    switch( color_index ) {
        case  0: return MapIconColor::BLACK;
        case  1: return MapIconColor::RED;
        case  2: return MapIconColor::BLUE;
        case  3: return MapIconColor::GREEN;
        case  4: return MapIconColor::CYAN;
        case  5: return MapIconColor::YELLOW;
        case  6: return MapIconColor::MAGENTA;
        case  7: return MapIconColor::WHITE;
        case  8: return MapIconColor::GOLD;
        case  9: return MapIconColor::DARK_RED;
        case 10: return MapIconColor::DARK_BLUE;
        case 11: return MapIconColor::DARK_GREEN;
        case 12: return MapIconColor::DARK_CYAN;
        case 13: return MapIconColor::DARK_YELLOW;
        case 14: return MapIconColor::DARK_MAGENTA;
        case 15: return MapIconColor::GRAY;
        case 16: return MapIconColor::ORANGE;
        case 17: return MapIconColor::RED_PULSE;
        case 18: return MapIconColor::BLUE_WHITE_PULSE;
        case 19: return MapIconColor::GREEN_PULSE;
        case 20: return MapIconColor::PINK;
        case 21: return MapIconColor::SILVER;
        case 22: return MapIconColor::SAGE;
        case 23: return MapIconColor::FLASHING_RED;
        case 24: return MapIconColor::FLASHING_BLUE;
        case 25: return MapIconColor::FLASHING_GREEN;
        case 26: return MapIconColor::FLASHING_CYAN;
        case 27: return MapIconColor::FLASHING_YELLOW;
        case 28: return MapIconColor::FLASHING_MAGENTA;
        case 29: return MapIconColor::FLASHING_WHITE;
        case 30: return MapIconColor::FLASHING_ORANGE;
    default:
        case 31: return MapIconColor::FLASHING_GRAY;
    }
}

BaseEntity::MapIconShape BaseEntity::getMapIconShape( uint8_t map_icon_bitfield ) {
    const uint8_t shape_index = (map_icon_bitfield & 0b1100000 >> 5);

    switch( shape_index ) {
        case 0: return MapIconShape::CIRCLE;
        case 1: return MapIconShape::TRIANGLE;
    default:
        case 2: return MapIconShape::DIAMOND;
    }
}

Json::Value BaseEntity::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = "BaseEntity";

    root["ACT"][NAME]["bitfield"]          = entity_internal.bitfield;
    root["ACT"][NAME]["health"]            = entity_internal.health;
    root["ACT"][NAME]["collision_damage"]  = entity_internal.collision_damage;
    root["ACT"][NAME]["team"]              = entity_internal.team;
    root["ACT"][NAME]["group_id"]          = entity_internal.group_id;
    root["ACT"][NAME]["map_icon_bitfield"] = entity_internal.map_icon_bitfield;
    root["ACT"][NAME]["target_priority"]   = entity_internal.target_priority;
    root["ACT"][NAME]["explosion"]         = entity_internal.explosion;
    root["ACT"][NAME]["ambient_sound"]     = entity_internal.ambient_sound;
    root["ACT"][NAME]["uv_offset_x"]       = entity_internal.uv_offset_x;
    root["ACT"][NAME]["uv_offset_y"]       = entity_internal.uv_offset_y;

    return root;
}

bool BaseEntity::readBase( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    entity_internal.bitfield = data_reader.readU32( endian );
    entity_internal.health = data_reader.readU16( endian );
    entity_internal.collision_damage = data_reader.readU16( endian );
    entity_internal.team = data_reader.readU8();
    entity_internal.group_id = data_reader.readU8();
    entity_internal.map_icon_bitfield = data_reader.readU8();
    entity_internal.target_priority = data_reader.readU8();
    entity_internal.explosion = data_reader.readU8();
    entity_internal.ambient_sound = data_reader.readU8();
    entity_internal.uv_offset_x = data_reader.readU8();
    entity_internal.uv_offset_y = data_reader.readU8();
    // 16 bytes

    return true;
}

BaseEntity::BaseEntity() {}

BaseEntity::BaseEntity( const ACTResource& obj ) : ACTResource( obj ) {}

BaseEntity::BaseEntity( const BaseEntity& obj ) : ACTResource( obj ), entity_internal( obj.entity_internal ) {}

BaseEntity::Internal BaseEntity::getBaseInternal() const {
    return entity_internal;
}

glm::vec2 BaseEntity::getTextureOffset() const {
    return (1.f / 256.f) * glm::vec2( entity_internal.uv_offset_x, entity_internal.uv_offset_y );
}

bool BaseEntity::disableRendering() const {
    return (BITFIELD_DISABLE_RENDERING & entity_internal.bitfield) != 0;
}

}
