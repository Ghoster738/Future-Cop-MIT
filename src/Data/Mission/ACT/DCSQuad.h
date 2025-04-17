#ifndef DATA_MISSION_ACTOR_ID_97_HEADER
#define DATA_MISSION_ACTOR_ID_97_HEADER

#include "../ACTResource.h"
#include <json/json.h>

namespace Data {

namespace Mission {

namespace ACT {

class DCSQuad : public ACTResource {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint8_t bitfield;
        uint8_t dcs_id;
        uint16_t height_offset;
        uint16_t width;
        uint16_t height;
        uint16_t rotation_y;
        uint16_t rotation_z;
        uint16_t rotation_x;
        uint8_t ground_cast_type;
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint16_t zero;
    } internal;

protected:
    virtual Json::Value makeJson() const;
    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    DCSQuad();
    DCSQuad( const ACTResource& obj );
    DCSQuad( const DCSQuad& obj );

    virtual uint_fast8_t getTypeID() const;
    virtual std::string getTypeIDName() const;

    virtual size_t getSize() const;

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;

    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;

    float getHeightOffset() const { return (1.f / 512.f) * internal.height_offset; }

    glm::vec2 getScale() const {
        return (1.f / 1024.f) * glm::vec2(internal.width, internal.height);
    }

    glm::vec4 getColor() const {
        return (1.f / 128.f) * glm::vec4(internal.red, internal.green, internal.blue, 128);
    }

    glm::quat getRotationQuaternion() const;
};
}

}

}

#endif // DATA_MISSION_ACTOR_ID_97_HEADER
