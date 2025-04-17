#ifndef DATA_MISSION_ACTOR_ID_9_HEADER
#define DATA_MISSION_ACTOR_ID_9_HEADER

#include "BaseShooter.h"

namespace Data {

namespace Mission {

namespace ACT {

/**
 * This seems to hold flying units.
 *
 * Jet Copters, and Flying Fortresses have this ACT type.
 * Also, crime war also uses this type. For example drones are found to use this type as well.
 */
class Aircraft : public BaseShooter {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint8_t uint8_0;
        uint8_t uint8_1;
        uint16_t uint16_0;
        uint8_t uint8_2;
        uint8_t spawn_type;
        uint16_t target_detection_range;
        uint16_t uint16_1;
        uint16_t uint16_2;
        uint16_t height_offset;
        uint16_t time_to_descend;
        uint16_t turn_rate;
        uint16_t move_speed;
        uint16_t orbit_area_x;
        uint16_t orbit_area_y;
        uint16_t uint16_3;
        uint16_t uint16_4;
        uint16_t spawn_pos_x;
        uint16_t spawn_pos_y;
    } internal;

protected:
    virtual Json::Value makeJson() const;
    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

    bool readBase( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    Aircraft();
    Aircraft( const ACTResource& obj );
    Aircraft( const BaseShooter& obj );
    Aircraft( const Aircraft& obj );

    virtual uint_fast8_t getTypeID() const;
    virtual std::string getTypeIDName() const;

    virtual size_t getSize() const;

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;

    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;

    glm::vec2 getSpawnPosition() const;

    float getHeightOffset() const;

    bool hasModelID() const { return rsl_data[0].type != RSL_NULL_TAG; }
    uint32_t getModelID() const { return rsl_data[0].resource_id; }
};
}

}

}

#endif // DATA_MISSION_ACTOR_ID_9_HEADER
