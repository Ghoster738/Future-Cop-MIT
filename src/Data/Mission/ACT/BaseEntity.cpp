#include "BaseEntity.h"

#include <iostream>

namespace Data::Mission::ACT {

Json::Value BaseEntity::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = "BaseEntity";

    root["ACT"][NAME]["bitfield"]         = entity_internal.bitfield;
    root["ACT"][NAME]["health"]           = entity_internal.health;
    root["ACT"][NAME]["collision_damage"] = entity_internal.collision_damage;
    root["ACT"][NAME]["team"]             = entity_internal.team;
    root["ACT"][NAME]["group_id"]         = entity_internal.group_id;
    root["ACT"][NAME]["map_icon_color"]   = entity_internal.map_icon_color;
    root["ACT"][NAME]["target_priority"]  = entity_internal.target_priority;
    root["ACT"][NAME]["explosion"]        = entity_internal.explosion;
    root["ACT"][NAME]["ambient_sound"]    = entity_internal.ambient_sound;
    root["ACT"][NAME]["uv_offset_x"]      = entity_internal.uv_offset_x;
    root["ACT"][NAME]["uv_offset_y"]      = entity_internal.uv_offset_y;

    return root;
}

bool BaseEntity::readBase( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    entity_internal.bitfield = data_reader.readU32( endian );
    entity_internal.health = data_reader.readU16( endian );
    entity_internal.collision_damage = data_reader.readU16( endian );
    entity_internal.team = data_reader.readU8();
    entity_internal.group_id = data_reader.readU8();
    entity_internal.map_icon_color = data_reader.readU8();
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

BaseEntity::MapIconColor BaseEntity::getMapIconColor() const {
}

BaseEntity::MapIconShape BaseEntity::getMapIconShape() const {
}

glm::vec2 BaseEntity::getTextureOffset() const {
    return (1.f / 256.f) * glm::vec2( entity_internal.uv_offset_x, entity_internal.uv_offset_y );
}

}
