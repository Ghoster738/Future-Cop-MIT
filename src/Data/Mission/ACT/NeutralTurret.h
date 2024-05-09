#ifndef DATA_MISSION_ACTOR_ID_36_HEADER
#define DATA_MISSION_ACTOR_ID_36_HEADER

#include "../ACTManager.h"
#include <json/json.h>

namespace Data {

namespace Mission {

namespace ACT {

class NeutralTurret : public ACTResource {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint32_t uint32_0;
        uint16_t uint16_0;
        uint16_t uint16_1;
        uint8_t uint8_0;
        uint8_t uint8_1;
        uint8_t uint8_2;
        uint8_t uint8_3;
        uint8_t uint8_4;
        uint16_t uint16_2;
        uint8_t uint8_5;
        uint16_t uint16_3;
        uint16_t uint16_4;
        uint8_t uint8_6;
        uint8_t uint8_7;
        uint16_t uint16_5;
        uint32_t uint32_1;
        uint16_t uint16_6;
        uint16_t uint16_7;
        uint8_t uint8_8;
        uint8_t uint8_9;
        uint16_t uint16_8;
        uint16_t gun_rotation;
        uint16_t uint16_10;
        uint16_t uint16_11;
        uint16_t uint16_12;
        uint8_t uint8_10;
        uint32_t uint32_2;
        uint8_t uint8_11;
        uint16_t base_rotation;
        uint8_t uint8_12;
        uint8_t uint8_13;
        uint8_t uint8_14;
        uint8_t uint8_15;
        uint8_t value_2;
        uint8_t value_3;
        uint16_t value_4;
    } internal;

protected:
    virtual Json::Value makeJson() const;
    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    NeutralTurret();
    NeutralTurret( const ACTResource& obj );
    NeutralTurret( const NeutralTurret& obj );

    virtual uint_fast8_t getTypeID() const;
    virtual std::string getTypeIDName() const;

    virtual size_t getSize() const;

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;

    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;

    bool getHasAliveGunID() const { return rsl_data[0].type != RSL_NULL_TAG; }
    uint32_t getAliveGunID() const { return rsl_data[0].resource_id; }

    bool getHasDestroyedGunID() const { return rsl_data[1].type != RSL_NULL_TAG; }
    uint32_t getDestroyedGunID() const { return rsl_data[1].resource_id; }

    bool getHasAliveBaseID() const { return rsl_data[2].type != RSL_NULL_TAG; }
    uint32_t getAliveBaseID() const { return rsl_data[2].resource_id; }

    bool getHasDestroyedBaseID() const { return rsl_data[3].type != RSL_NULL_TAG; }
    uint32_t getDestroyedBaseID() const { return rsl_data[3].resource_id; }

    float getGunRotation() const;
    glm::quat getGunRotationQuaternion() const;

    float getBaseRotation() const;
    glm::quat getBaseRotationQuaternion() const;

    static std::vector<NeutralTurret*> getVector( Data::Mission::ACTManager& act_manager );

    static const std::vector<NeutralTurret*> getVector( const Data::Mission::ACTManager& act_manager );

};
}

}

}

#endif // DATA_MISSION_ACTOR_ID_36_HEADER
