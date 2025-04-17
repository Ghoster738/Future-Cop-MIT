#ifndef DATA_MISSION_ACTOR_ID_8_HEADER
#define DATA_MISSION_ACTOR_ID_8_HEADER

#include "BaseTurret.h"

namespace Data::Mission::ACT {

/**
 * This holds the base turret.
 *
 * This is the turret that always has the same side.
 */
class Turret : public BaseTurret {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint16_t zero_0;
        uint8_t uint8_0;
        uint8_t zero_1;
        int16_t base_rotation;
    } internal;

protected:
    virtual Json::Value makeJson() const;
    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

    bool readBase( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    Turret();
    Turret( const ACTResource& obj );
    Turret( const Turret& obj );

    virtual uint_fast8_t getTypeID() const;
    virtual std::string getTypeIDName() const;

    virtual size_t getSize() const;

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;

    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;

    bool getHasAliveBaseID() const { return rsl_data[2].type != RSL_NULL_TAG; }
    uint32_t getAliveBaseID() const { return rsl_data[2].resource_id; }

    bool getHasDestroyedBaseID() const { return rsl_data[3].type != RSL_NULL_TAG; }
    uint32_t getDestroyedBaseID() const { return rsl_data[3].resource_id; }

    float getBaseRotation() const;
    glm::quat getBaseRotationQuaternion() const;
};

}

#endif // DATA_MISSION_ACTOR_ID_8_HEADER
