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
        uint16_t rotation;
        uint16_t uint16_1;
        uint16_t uint16_2;
        uint16_t uint16_3; // Time or angle until loop back?
        uint8_t uint8_0;
        uint8_t uint8_1; // Layer?
        uint8_t uint8_2;
        uint8_t uint8_3;
        uint8_t uint8_4;
        uint8_t uint8_5;
        uint8_t uint8_6; // Spin?
        uint8_t uint8_7;
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
