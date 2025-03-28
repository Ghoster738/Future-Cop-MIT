#include "BaseEntity.h"

namespace Data::Mission::ACT {

Json::Value BaseEntity::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = "BaseEntity";

    root[NAME]["bitfield"]         = base_internal.bitfield;
    root[NAME]["health"]           = base_internal.health;
    root[NAME]["collision_damage"] = base_internal.collision_damage;
    root[NAME]["team"]             = base_internal.team;
    root[NAME]["group_id"]         = base_internal.group_id;
    root[NAME]["map_icon_color"]   = base_internal.map_icon_color;
    root[NAME]["target_priority"]  = base_internal.target_priority;
    root[NAME]["explosion"]        = base_internal.explosion;
    root[NAME]["ambient_sound"]    = base_internal.ambient_sound;
    root[NAME]["uv_offset_x"]      = base_internal.uv_offset_x;
    root[NAME]["uv_offset_y"]      = base_internal.uv_offset_y;

    return root;
}

bool BaseEntity::readBase( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    base_internal.bitfield = data_reader.readU32( endian );
    base_internal.health = data_reader.readU16( endian );
    base_internal.collision_damage = data_reader.readU16( endian );
    base_internal.team = data_reader.readU8();
    base_internal.group_id = data_reader.readU8();
    base_internal.map_icon_color = data_reader.readU8();
    base_internal.target_priority = data_reader.readU8();
    base_internal.explosion = data_reader.readU8();
    base_internal.ambient_sound = data_reader.readU8();
    base_internal.uv_offset_x = data_reader.readU8();
    base_internal.uv_offset_y = data_reader.readU8();
    // 16 bytes

    return true;
}

BaseEntity::BaseEntity() {}

BaseEntity::BaseEntity( const ACTResource& obj ) : ACTResource( obj ) {}

BaseEntity::BaseEntity( const BaseEntity& obj ) : ACTResource( obj ), base_internal( obj.base_internal ) {}

BaseEntity::Internal BaseEntity::getBaseInternal() const {
    return base_internal;
}

}
