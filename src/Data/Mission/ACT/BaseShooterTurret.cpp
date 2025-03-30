#include "BaseShooterTurret.h"

namespace Data::Mission::ACT {

Json::Value BaseShooterTurret::makeJson() const {
    Json::Value root = BaseShooterEntity::makeJson();
    const std::string NAME = "BaseShooterTurret";

    root["ACT"][NAME]["ground_cast"]   = turret_shooter_internal.ground_cast;
    root["ACT"][NAME]["uint8_0"]       = turret_shooter_internal.uint8_0;
    root["ACT"][NAME]["uint16_0"]      = turret_shooter_internal.uint16_0;
    root["ACT"][NAME]["gun_rotation"]  = turret_shooter_internal.gun_rotation;
    root["ACT"][NAME]["height_offset"] = turret_shooter_internal.height_offset;
    root["ACT"][NAME]["turn_speed"]    = turret_shooter_internal.turn_speed;
    root["ACT"][NAME]["uint16_1"]      = turret_shooter_internal.uint16_1;
    root["ACT"][NAME]["unk_turn_type"] = turret_shooter_internal.unk_turn_type;
    root["ACT"][NAME]["zero"]          = turret_shooter_internal.zero;

    return root;
}

bool BaseShooterTurret::readBase( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {

    BaseShooterEntity::readBase(data_reader, endian);

    turret_shooter_internal.ground_cast   = data_reader.readU8(); // Values: 0, 1,
    turret_shooter_internal.uint8_0       = data_reader.readU8(); // Values: 0, 1, 4,
    turret_shooter_internal.uint16_0      = data_reader.readU16( endian ); // Values: 0, 1, 2,
    turret_shooter_internal.gun_rotation  = data_reader.readI16( endian ); // Values: 0, 200, 400, 512, 1024, 1100, 1400, 1536, 1800, 2048, 2560, 2900, 3072, 3200, 3400, 3584, 3600, 3700, 65535,
    turret_shooter_internal.height_offset = data_reader.readI16( endian ); // Values: 0, 40, 327, 409, 491, 532, 1925, 2129,
    turret_shooter_internal.turn_speed    = data_reader.readU16( endian ); // Values: 0, 409, 819, 1024, 1228, 1638, 2048, 2457, 3276, 4096,
    turret_shooter_internal.uint16_1      = data_reader.readU16( endian ); // Values: 2048, 6144, 7782, 8192, 10240, 12288,
    turret_shooter_internal.unk_turn_type = data_reader.readU8(); // Values: 0, 1, 3, 4, 5, 7, 35, 69, 71, 128, 132, 133,
    turret_shooter_internal.zero          = data_reader.readU8(); // Always 0

    return true;
}

BaseShooterTurret::BaseShooterTurret() {}

BaseShooterTurret::BaseShooterTurret( const ACTResource& obj ) : BaseShooterEntity( obj ) {}

BaseShooterTurret::BaseShooterTurret( const BaseEntity& obj ) : BaseShooterEntity( obj ) {}

BaseShooterTurret::BaseShooterTurret( const BaseShooterEntity& obj ) : BaseShooterEntity( obj ) {}

BaseShooterTurret::BaseShooterTurret( const BaseShooterTurret& obj ) : BaseShooterEntity( obj ), turret_shooter_internal( obj.turret_shooter_internal ) {}

BaseShooterTurret::Internal BaseShooterTurret::getShooterTurretInternal() const {
    return turret_shooter_internal;
}

float Data::Mission::ACT::BaseShooterTurret::getHeightOffset() const {
    return (1.f / 512.f) * turret_shooter_internal.height_offset;
}

float BaseShooterTurret::getGunRotation() const {
    return ACTResource::getRotation( turret_shooter_internal.gun_rotation - 1024 );
}

glm::quat BaseShooterTurret::getGunRotationQuaternion() const {
    return getRotationQuaternion( this->getGunRotation() );
}

}
