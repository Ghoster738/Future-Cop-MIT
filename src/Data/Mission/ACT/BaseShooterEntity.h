#ifndef DATA_MISSION_ACTOR_BASE_SHOOTER_ENTITY_HEADER
#define DATA_MISSION_ACTOR_BASE_SHOOTER_ENTITY_HEADER

#include "BaseEntity.h"

namespace Data::Mission::ACT {

class BaseShooterEntity : public BaseEntity {
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

protected:
    virtual Json::Value makeJson() const;

    bool readBase( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    BaseShooterEntity();
    BaseShooterEntity( const ACTResource& obj );
    BaseShooterEntity( const BaseEntity& obj );
    BaseShooterEntity( const BaseShooterEntity& obj );

    Internal getShooterInternal() const;
};

}

#endif // DATA_MISSION_ACTOR_BASE_SHOOTER_ENTITY_HEADER
