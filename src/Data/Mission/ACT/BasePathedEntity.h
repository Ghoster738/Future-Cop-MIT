#ifndef DATA_MISSION_ACTOR_BASE_PATHED_ENTITY_HEADER
#define DATA_MISSION_ACTOR_BASE_PATHED_ENTITY_HEADER

#include "BaseShooter.h"

namespace Data::Mission::ACT {

class BasePathedEntity : public BaseShooter {
public:
    struct Internal {
        uint16_t bitfield;
        uint16_t move_speed;
        uint16_t height_offset;
        uint16_t move_speed_multipiler; // ?
        uint16_t acceleration;
        uint16_t uint16_14;
        uint16_t uint16_15;
        uint16_t uint16_16;
        uint16_t uint16_17;
        uint8_t uint8_10;
        uint8_t uint8_11;
    } pathed_entity_internal;

    struct Bitfield {
        uint16_t enable_backtrack : 1;
        // unknown
        // unknown
        uint16_t disable_path_obstruction : 1;
        // unknown
        // unknown
        // unknown
        // unknown

        // unknown
        // unknown
        uint16_t disable_ease : 1;
        // unknown
        // unknown
        // unknown
        // unknown
        // unknown
    };

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

    float getHeightOffset() const { return (1.f / 512.f) * pathed_entity_internal.height_offset; }

    /**
     * This looks wierd but it works.
     */
    float getMovementSpeed() const {
        if(pathed_entity_internal.move_speed == 0)
            return (1.f / 512.f);

        return (1.f / 512.f) * pathed_entity_internal.move_speed;
    }

    bool  getHasNetID() const { return rsl_data[2].type != RSL_NULL_TAG; }
    uint32_t getNetID() const { return rsl_data[2].resource_id; }

    Bitfield getPathedEntityBitfield() const;
};

}

#endif // DATA_MISSION_ACTOR_BASE_PATHED_ENTITY_HEADER
