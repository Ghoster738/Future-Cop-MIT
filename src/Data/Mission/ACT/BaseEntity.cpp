#include "BaseEntity.h"

namespace Data::Mission::ACT {

Json::Value BaseEntity::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = "BaseEntity";

    root[NAME]["bitfield"]         = entity_internal.bitfield;
    root[NAME]["health"]           = entity_internal.health;
    root[NAME]["collision_damage"] = entity_internal.collision_damage;
    root[NAME]["team"]             = entity_internal.team;
    root[NAME]["group_id"]         = entity_internal.group_id;
    root[NAME]["map_icon_color"]   = entity_internal.map_icon_color;
    root[NAME]["target_priority"]  = entity_internal.target_priority;
    root[NAME]["explosion"]        = entity_internal.explosion;
    root[NAME]["ambient_sound"]    = entity_internal.ambient_sound;
    root[NAME]["uv_offset_x"]      = entity_internal.uv_offset_x;
    root[NAME]["uv_offset_y"]      = entity_internal.uv_offset_y;

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

glm::vec2 BaseEntity::getTextureOffset() const {
    return (1.f / 256.f) * glm::vec2( entity_internal.uv_offset_x, entity_internal.uv_offset_y );
}

}
