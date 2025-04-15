#ifndef DATA_MISSION_ACTOR_BASE_ENTITY_HEADER
#define DATA_MISSION_ACTOR_BASE_ENTITY_HEADER

#include "../ACTResource.h"
#include <json/json.h>

namespace Data::Mission::ACT {

class BaseEntity : public ACTResource {
public:
    static uint_fast8_t TYPE_ID;

    enum MapIconColor {
        BLACK = 0,
        RED = 1,
        BLUE = 2,
        GREEN = 3,
        CYAN = 4,
        YELLOW = 5,
        MAGENTA = 6,
        WHITE = 7,
        GOLD = 8,
        DARK_RED = 9,
        DARK_BLUE = 10,
        DARK_GREEN = 11,
        DARK_CYAN = 12,
        DARK_YELLOW = 13,
        DARK_MAGENTA = 14,
        GRAY = 15,
        ORANGE = 16,
        RED_PULSE = 17,
        BLUE_WHITE_PULSE = 18,
        GREEN_PULSE = 19,
        PINK = 20,
        SILVER = 21,
        SAGE = 22,
        FLASHING_RED = 23,
        FLASHING_BLUE = 24,
        FLASHING_GREEN = 25,
        FLASHING_CYAN = 26,
        FLASHING_YELLOW = 27,
        FLASHING_MAGENTA = 28,
        FLASHING_WHITE = 29,
        FLASHING_ORANGE = 30,
        FLASHING_GRAY = 31
    };

    static MapIconColor getMapIconColor( uint8_t map_icon_bitfield );

    enum MapIconShape {
        CIRCLE = 0,
        TRIANGLE = 1,
        DIAMOND = 2
    };

    static MapIconShape getMapIconShape( uint8_t map_icon_bitfield );

    struct Internal {
        uint32_t bitfield;
        uint16_t health;
        uint16_t collision_damage;
        uint8_t team;
        uint8_t group_id;
        uint8_t map_icon_bitfield;
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

    bool isMapIconPresent() const { return entity_internal.map_icon_bitfield != 0; }

    MapIconColor getMapIconColor() const { return getMapIconColor(entity_internal.map_icon_bitfield); }

    MapIconShape getMapIconShape() const { return getMapIconShape(entity_internal.map_icon_bitfield); }

    glm::vec2 getTextureOffset() const;

    bool disableRendering() const;
};

}

#endif // DATA_MISSION_ACTOR_ID_1_HEADER
