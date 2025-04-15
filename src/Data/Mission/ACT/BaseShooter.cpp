#include "BaseShooter.h"

namespace Data::Mission::ACT {

Json::Value BaseShooter::makeJson() const {
    Json::Value root = BaseEntity::makeJson();
    const std::string NAME = "BaseShooter";

    root["ACT"][NAME]["weapon_id"]        = shooter_entity_internal.weapon_id;
    root["ACT"][NAME]["bitfield"]         = shooter_entity_internal.bitfield;
    root["ACT"][NAME]["uint8_0"]          = shooter_entity_internal.uint8_0;
    root["ACT"][NAME]["target_type"]      = shooter_entity_internal.target_type;
    root["ACT"][NAME]["targeting"]       = shooter_entity_internal.targeting;
    root["ACT"][NAME]["fov"]              = shooter_entity_internal.fov;
    root["ACT"][NAME]["uint16_0"]         = shooter_entity_internal.uint16_0;
    root["ACT"][NAME]["engage_range"]     = shooter_entity_internal.engage_range;
    root["ACT"][NAME]["targeting_delay"] = shooter_entity_internal.targeting_delay;

    return root;
}

bool BaseShooter::readBase( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {

    BaseEntity::readBase(data_reader, endian);

    shooter_entity_internal.weapon_id       = data_reader.readU16( endian ); // Values: 1, 3, 4, 5, 6, 9, 11, 12, 14,
    shooter_entity_internal.bitfield        = data_reader.readU16( endian ); // Values: 0, 2, 6, 38, 64, 70, 514, 544,
    shooter_entity_internal.uint8_0         = data_reader.readU8(); // Values: 1, 2, 3,
    shooter_entity_internal.target_type     = data_reader.readU8(); // Values: 1, 2, 3, 4,
    shooter_entity_internal.targeting       = data_reader.readU16( endian ); // Values: 1, 2, 32, 441,
    shooter_entity_internal.fov             = data_reader.readU16( endian ); // Always 0x1000
    shooter_entity_internal.uint16_0        = data_reader.readU16( endian ); // Always 0x1000
    shooter_entity_internal.engage_range    = data_reader.readU16( endian ); // Values: 6144, 8192,
    shooter_entity_internal.targeting_delay = data_reader.readU16( endian ); // Values: 8, 16, 32, 48, 80,

    return true;
}

BaseShooter::BaseShooter() {}

BaseShooter::BaseShooter( const ACTResource& obj ) : BaseEntity( obj ) {}

BaseShooter::BaseShooter( const BaseEntity& obj ) : BaseEntity( obj ) {}

BaseShooter::BaseShooter( const BaseShooter& obj ) : BaseEntity( obj ), shooter_entity_internal( obj.shooter_entity_internal ) {}

BaseShooter::Internal BaseShooter::getShooterInternal() const {
    return shooter_entity_internal;
}

BaseShooter::Bitfield BaseShooter::getShooterBitfield() const {
    Bitfield bitfield;

    // Skipped!
    bitfield.prevent_back_shooting  = (0x00000002 & entity_internal.bitfield) != 0;
    bitfield.shoot_when_facing      = (0x00000004 & entity_internal.bitfield) != 0;
    // Skipped!
    // Skipped!
    bitfield.fire_alternations      = (0x00000020 & entity_internal.bitfield) != 0;
    bitfield.target_priority        = (0x00000040 & entity_internal.bitfield) != 0;
    // Skipped!


    // Skipped!
    bitfield.weapon_actor_collision = (0x00000200 & entity_internal.bitfield) != 0;
    bitfield.attackable_weapon      = (0x00000400 & entity_internal.bitfield) != 0;
    // Skipped!
    // Skipped!
    // Skipped!
    bitfield.allow_switch_target    = (0x00004000 & entity_internal.bitfield) != 0;
    // Skipped!

    return bitfield;
}

}
