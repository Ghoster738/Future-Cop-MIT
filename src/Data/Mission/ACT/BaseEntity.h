#ifndef DATA_MISSION_ACTOR_BASE_ENTITY_HEADER
#define DATA_MISSION_ACTOR_BASE_ENTITY_HEADER

#include "../ACTResource.h"
#include <json/json.h>

namespace Data::Mission::ACT {

class BaseEntity : public ACTResource {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint32_t bitfield;
        uint16_t health;
        uint16_t collision_damage;
        uint8_t team;
        uint8_t group_id;
        uint8_t map_icon_color;
        uint8_t target_priority;
        uint8_t explosion;
        uint8_t ambient_sound;
        uint8_t uv_offset_x;
        uint8_t uv_offset_y;
    } entity_internal;

protected:
    virtual Json::Value makeJson() const;

    bool readBase( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    BaseEntity();
    BaseEntity( const ACTResource& obj );
    BaseEntity( const BaseEntity& obj );

    Internal getBaseInternal() const;

    glm::vec2 getTextureOffset() const;
};

}

#endif // DATA_MISSION_ACTOR_ID_1_HEADER
