#ifndef DATA_MISSION_ACTOR_BASE_TURRET_HEADER
#define DATA_MISSION_ACTOR_BASE_TURRET_HEADER

#include "BaseShooter.h"

namespace Data::Mission::ACT {

class BaseTurret : public BaseShooter {
public:
    struct Internal {
        uint8_t ground_cast_type;
        uint8_t uint8_0;
        uint16_t uint16_0;
        int16_t  gun_rotation;
        int16_t height_offset;
        uint16_t turn_speed;
        uint16_t uint16_1;
        uint8_t unk_turn_type;
        uint8_t zero;
    } turret_shooter_internal;

protected:
    virtual Json::Value makeJson() const;

    bool readBase( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    BaseTurret();
    BaseTurret( const ACTResource& obj );
    BaseTurret( const BaseEntity& obj );
    BaseTurret( const BaseShooter& obj );
    BaseTurret( const BaseTurret& obj );

    Internal getShooterTurretInternal() const;

    float getHeightOffset() const;

    float getGunRotation() const;
    glm::quat getGunRotationQuaternion() const;

    bool getHasAliveGunID() const { return rsl_data[0].type != RSL_NULL_TAG; }
    uint32_t getAliveGunID() const { return rsl_data[0].resource_id; }

    bool getHasDestroyedGunID() const { return rsl_data[1].type != RSL_NULL_TAG; }
    uint32_t getDestroyedGunID() const { return rsl_data[1].resource_id; }
};

}

#endif // DATA_MISSION_ACTOR_BASE_TURRET_HEADER
