#ifndef DATA_MISSION_ACTOR_ID_11_HEADER
#define DATA_MISSION_ACTOR_ID_11_HEADER

#include "BaseEntity.h"
#include <json/json.h>

namespace Data {

namespace Mission {

namespace ACT {

class DynamicProp : public BaseEntity {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint8_t ground_cast_type;
        uint8_t zero;
        int16_t rotation;
        int32_t height_offset;
    } internal;

protected:
    virtual Json::Value makeJson() const;
    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    DynamicProp();
    DynamicProp( const ACTResource& obj );
    DynamicProp( const DynamicProp& obj );

    virtual uint_fast8_t getTypeID() const;
    virtual std::string getTypeIDName() const;

    virtual size_t getSize() const;

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;

    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;

    float getHeightOffset() const { return (1.f / 8192.f) * internal.height_offset; }

    glm::quat getRotationQuaternion() const;

    bool getHasAliveID() const { return rsl_data[0].type != RSL_NULL_TAG; }
    uint32_t getAliveObjResourceID() const { return rsl_data[0].resource_id; }

    bool getHasDeathID() const { return rsl_data[1].type != RSL_NULL_TAG; }
    uint32_t getDeathObjResourceID() const { return rsl_data[1].resource_id; }
};
}

}

}

#endif // DATA_MISSION_ACTOR_ID_11_HEADER
