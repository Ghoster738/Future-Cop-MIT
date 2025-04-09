#ifndef DATA_MISSION_ACTOR_BASE_ENTITY_HEADER
#define DATA_MISSION_ACTOR_BASE_ENTITY_HEADER

#include "../ACTResource.h"
#include <json/json.h>

namespace Data::Mission::ACT {

class BaseEntity : public ACTResource {
public:
    static uint_fast8_t TYPE_ID;

    enum MapIconType {
        NONE = 0,
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
        FLASHING_GRAY = 31,
        BLACK_TRI = 32,
        RED_TRI = 33,
        BLUE_TRI = 34,
        GREEN_TRI = 35,
        CYAN_TRI = 36,
        YELLOW_TRI = 37,
        MAGENTA_TRI = 38,
        WHITE_TRI = 39,
        GOLD_TRI = 40,
        DARK_RED_TRI = 41,
        DARK_BLUE_TRI = 42,
        DARK_GREEN_TRI = 43,
        DARK_CYAN_TRI = 44,
        DARK_YELLOW_TRI = 45,
        DARK_MAGENTA_TRI = 46,
        GRAY_TRI = 47,
        ORANGE_TRI = 48,
        RED_PULSE_TRI = 49,
        BLUE_WHITE_PULSE_TRI = 50,
        GREEN_PULSE_TRI = 51,
        PINK_TRI = 52,
        SILVER_TRI = 53,
        SAGE_TRI = 54,
        FLASHING_RED_TRI = 55,
        FLASHING_BLUE_TRI = 56,
        FLASHING_GREEN_TRI = 57,
        FLASHING_CYAN_TRI = 58,
        FLASHING_YELLOW_TRI = 59,
        FLASHING_MAGENTA_TRI = 60,
        FLASHING_WHITE_TRI = 61,
        FLASHING_ORANGE_TRI = 62,
        FLASHING_GRAY_TRI = 63,
        BLACK_DIAMOND = 64,
        RED_DIAMOND = 65,
        BLUE_DIAMOND = 66,
        GREEN_DIAMOND = 67,
        CYAN_DIAMOND = 68,
        YELLOW_DIAMOND = 69,
        MAGENTA_DIAMOND = 70,
        WHITE_DIAMOND = 71,
        GOLD_DIAMOND = 72,
        DARK_RED_DIAMOND = 73,
        DARK_BLUE_DIAMOND = 74,
        DARK_GREEN_DIAMOND = 75,
        DARK_CYAN_DIAMOND = 76,
        DARK_YELLOW_DIAMOND = 77,
        DARK_MAGENTA_DIAMOND = 78,
        GRAY_DIAMOND = 79,
        ORANGE_DIAMOND = 80,
        RED_PULSE_DIAMOND = 81,
        BLUE_WHITE_PULSE_DIAMOND = 82,
        GREEN_PULSE_DIAMOND = 83,
        PINK_DIAMOND = 84,
        SILVER_DIAMOND = 85,
        SAGE_DIAMOND = 86,
        FLASHING_RED_DIAMOND = 87,
        FLASHING_BLUE_DIAMOND = 88,
        FLASHING_GREEN_DIAMOND = 89,
        FLASHING_CYAN_DIAMOND = 90,
        FLASHING_YELLOW_DIAMOND = 91,
        FLASHING_MAGENTA_DIAMOND = 92,
        FLASHING_WHITE_DIAMOND = 93,
        FLASHING_ORANGE_DIAMOND = 94,
        FLASHING_GRAY_DIAMOND = 95,
        CYAN_DIAMOND_CLONE = 204
    };

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

    MapIconType getMapIconType() const;

    glm::vec2 getTextureOffset() const;
};

}

#endif // DATA_MISSION_ACTOR_ID_1_HEADER
