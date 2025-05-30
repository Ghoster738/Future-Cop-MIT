#ifndef DATA_MISSION_ACTOR_ID_1_HEADER
#define DATA_MISSION_ACTOR_ID_1_HEADER

#include "BaseEntity.h"

namespace Data::Mission::ACT {

/**
 * This holds an X1A unit.
 *
 * This is the vehicle that you pilot in the game.
 */
class X1Alpha : public BaseEntity {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        int16_t rotation;
        uint8_t uint8_0;
        uint8_t uint8_1;
    } internal;

protected:
    virtual Json::Value makeJson() const;
    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    X1Alpha();
    X1Alpha( const ACTResource& obj );
    X1Alpha( const BaseEntity& obj );
    X1Alpha( const X1Alpha& obj );

    virtual uint_fast8_t getTypeID() const;
    virtual std::string getTypeIDName() const;

    virtual size_t getSize() const;

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;

    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;

    glm::quat getRotationQuaternion() const;

    bool spawnInHoverMode() const {
        return internal.rotation < 0;
    }

    bool getHasLegID() const { return rsl_data[0].type != RSL_NULL_TAG; }
    uint32_t getLegID() const { return rsl_data[0].resource_id; }

    bool getHasCockpitID() const { return rsl_data[2].type != RSL_NULL_TAG; }
    uint32_t getCockpitID() const { return rsl_data[2].resource_id; }

    bool getHasWeaponID() const { return rsl_data[3].type != RSL_NULL_TAG; }
    uint32_t getWeaponID() const { return rsl_data[3].resource_id; }

    bool getHasBeaconLightsID() const { return rsl_data[4].type != RSL_NULL_TAG; }
    uint32_t getBeaconLightsID() const { return rsl_data[4].resource_id; }

    bool getHasPilotID() const { return rsl_data[5].type != RSL_NULL_TAG; }
    uint32_t getPilotID() const { return rsl_data[5].resource_id; }
};

}

#endif // DATA_MISSION_ACTOR_ID_1_HEADER
