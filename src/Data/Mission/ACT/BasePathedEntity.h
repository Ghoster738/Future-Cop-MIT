#ifndef DATA_MISSION_ACTOR_BASE_PATHED_ENTITY_HEADER
#define DATA_MISSION_ACTOR_BASE_PATHED_ENTITY_HEADER

#include "BaseShooter.h"

namespace Data::Mission::ACT {

class BasePathedEntity : public BaseShooter {
public:
    struct Internal {
        uint16_t uint16_9;
        uint16_t uint16_10;
        uint16_t uint16_11;
        uint16_t uint16_12;
        uint16_t uint16_13;
        uint16_t uint16_14;
        uint16_t uint16_15;
        uint16_t uint16_16;
        uint16_t uint16_17;
        uint8_t uint8_10;
        uint8_t uint8_11;
    } pathed_entity_internal;

protected:
    virtual Json::Value makeJson() const;

    bool readBase( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    BasePathedEntity();
    BasePathedEntity( const ACTResource& obj );
    BasePathedEntity( const BaseEntity& obj );
    BasePathedEntity( const BaseShooter& obj );
    BasePathedEntity( const BasePathedEntity& obj );

    Internal getPathedEntityInternal() const;

    float getHeightOffset() const { return (1.f / 512.f) * pathed_entity_internal.uint16_11; }

};

}

#endif // DATA_MISSION_ACTOR_BASE_PATHED_ENTITY_HEADER
