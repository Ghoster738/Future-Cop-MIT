#ifndef DATA_MISSION_ACTOR_ID_96_HEADER
#define DATA_MISSION_ACTOR_ID_96_HEADER

#include "../ACTResource.h"
#include <json/json.h>

namespace Data {

namespace Mission {

namespace ACT {

class Prop : public ACTResource {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint16_t      rotation_y;
        uint16_t      rotation_z;
        uint16_t      rotation_x;
        uint16_t   height_offset;
        uint8_t ground_cast_type;
        uint8_t          uint8_1; // Unknown.
        uint8_t  animation_speed;
        uint8_t          scale_y;
        uint8_t          scale_z;
        uint8_t          scale_x;
        uint8_t       spin_speed;
        uint8_t       spin_angle;
    } internal;

protected:
    virtual Json::Value makeJson() const;
    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    Prop();
    Prop( const ACTResource& obj );
    Prop( const Prop& obj );

    virtual uint_fast8_t getTypeID() const;
    virtual std::string getTypeIDName() const;

    virtual size_t getSize() const;

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;

    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;

    uint32_t getObjResourceID() const { return rsl_data[0].resource_id; }

    float getRotation() const;

    glm::quat getRotationQuaternion() const;

};
}

}

}

#endif // DATA_MISSION_ACTOR_ID_96_HEADER
