#ifndef DATA_MISSION_ACTOR_BASE_SHOOTER_HEADER
#define DATA_MISSION_ACTOR_BASE_SHOOTER_HEADER

#include "BaseEntity.h"

namespace Data::Mission::ACT {

class BaseShooter : public BaseEntity {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint16_t weapon_id;
        uint16_t bitfield;
        uint8_t  uint8_0;
        uint8_t  target_type;
        uint16_t targeting; // target team or actor depending on target_type
        uint16_t fov; // normalized
        uint16_t uint16_0;
        uint16_t engage_range;
        uint16_t targeting_delay;
    } shooter_entity_internal;

    struct Bitfield {
        // unknown but always false
        uint16_t     prevent_back_shooting : 1;
        uint16_t         shoot_when_facing : 1;
        // unknown
        // unknown
        uint16_t         fire_alternations : 1;
        uint16_t           target_priority : 1;
        // unknown

        // unknown but always false
        uint16_t    weapon_actor_collision : 1;
        uint16_t         attackable_weapon : 1;
        // unknown
        // unknown
        // unknown
        uint16_t       allow_switch_target : 1;
        // unknown
    };

protected:
    virtual Json::Value makeJson() const;

    bool readBase( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    BaseShooter();
    BaseShooter( const ACTResource& obj );
    BaseShooter( const BaseEntity& obj );
    BaseShooter( const BaseShooter& obj );

    Internal getShooterInternal() const;

    Bitfield getShooterBitfield() const;
};

}

#endif // DATA_MISSION_ACTOR_BASE_SHOOTER_HEADER
