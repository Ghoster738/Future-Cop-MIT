#ifndef DATA_MISSION_ACTOR_BASE_SHOOTER_TURRET_HEADER
#define DATA_MISSION_ACTOR_BASE_SHOOTER_TURRET_HEADER

#include "BaseShooterEntity.h"

namespace Data::Mission::ACT {

class BaseShooterTurret : public BaseShooterEntity {
public:
    struct Internal {
        uint8_t ground_cast;
        uint8_t uint8_0;
        uint16_t uint16_0;
        int16_t  gun_rotation;
        uint16_t height_offset;
        uint16_t turn_speed;
        uint16_t uint16_1;
        uint8_t unk_turn_type;
        uint8_t zero;
    } turret_shooter_internal;

protected:
    virtual Json::Value makeJson() const;

    bool readBase( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    BaseShooterTurret();
    BaseShooterTurret( const ACTResource& obj );
    BaseShooterTurret( const BaseEntity& obj );
    BaseShooterTurret( const BaseShooterEntity& obj );
    BaseShooterTurret( const BaseShooterTurret& obj );

    Internal getShooterTurretInternal() const;

    float getGunRotation() const;
    glm::quat getGunRotationQuaternion() const;
};

}

#endif // DATA_MISSION_ACTOR_BASE_SHOOTER_TURRET_HEADER
