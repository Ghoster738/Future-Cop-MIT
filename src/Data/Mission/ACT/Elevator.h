#ifndef DATA_MISSION_ACTOR_ID_10_HEADER
#define DATA_MISSION_ACTOR_ID_10_HEADER

#include "BaseEntity.h"

namespace Data {

namespace Mission {

namespace ACT {

class Elevator : public BaseEntity {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint8_t num_of_stops;
        uint8_t starting_position; // Starts at one not zero.
        int16_t height_offset[3];
        uint16_t stop_time[3];
        uint16_t up_speed;
        uint16_t down_speed;
        int16_t rotation;
        uint8_t trigger_type;
        uint8_t tile_effect;
        uint8_t end_sound;
        uint8_t uint8_0;
    } internal;

protected:
    virtual Json::Value makeJson() const;
    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    Elevator();
    Elevator( const ACTResource& obj );
    Elevator( const Elevator& obj );

    virtual uint_fast8_t getTypeID() const;
    virtual std::string getTypeIDName() const;

    virtual size_t getSize() const;

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;

    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;

    bool getHasElevatorID() const { return rsl_data[0].type != RSL_NULL_TAG; }
    uint32_t getElevatorID() const { return rsl_data[0].resource_id; }

    float getHeightOffset( unsigned index ) const;

    glm::quat getRotationQuaternion() const;
};
}

}

}

#endif // DATA_MISSION_ACTOR_ID_10_HEADER
