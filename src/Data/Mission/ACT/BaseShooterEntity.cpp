#include "BaseShooterEntity.h"

namespace Data::Mission::ACT {

Json::Value BaseShooterEntity::makeJson() const {
    Json::Value root = BaseEntity::makeJson();
    const std::string NAME = "BaseShooterEntity";

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

bool BaseShooterEntity::readBase( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {

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

BaseShooterEntity::BaseShooterEntity() {}

BaseShooterEntity::BaseShooterEntity( const ACTResource& obj ) : BaseEntity( obj ) {}

BaseShooterEntity::BaseShooterEntity( const BaseEntity& obj ) : BaseEntity( obj ) {}

BaseShooterEntity::BaseShooterEntity( const BaseShooterEntity& obj ) : BaseEntity( obj ), shooter_entity_internal( obj.shooter_entity_internal ) {}

BaseShooterEntity::Internal BaseShooterEntity::getShooterInternal() const {
    return shooter_entity_internal;
}

}
