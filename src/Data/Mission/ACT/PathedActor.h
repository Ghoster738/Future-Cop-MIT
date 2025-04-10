#ifndef DATA_MISSION_ACTOR_ID_5_HEADER
#define DATA_MISSION_ACTOR_ID_5_HEADER

#include "BasePathedEntity.h"

namespace Data {

namespace Mission {

namespace ACT {

class PathedActor : public BasePathedEntity {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint8_t uint8_12;
        uint8_t uint8_13;
        uint8_t uint8_14;
        uint8_t uint8_15;
    } internal;

protected:
    virtual Json::Value makeJson() const;
    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    PathedActor();
    PathedActor( const ACTResource& obj );
    PathedActor( const PathedActor& obj );

    virtual uint_fast8_t getTypeID() const;
    virtual std::string getTypeIDName() const;

    virtual size_t getSize() const;

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;

    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;

    bool getHasAliveID() const { return rsl_data[0].type != RSL_NULL_TAG; }
    uint32_t getAliveID() const { return rsl_data[0].resource_id; }

    bool getHasDestroyedID() const { return rsl_data[1].type != RSL_NULL_TAG; }
    uint32_t getDestroyedID() const { return rsl_data[1].resource_id; }

    bool getHasNetID() const { return rsl_data[2].type != RSL_NULL_TAG; }
    uint32_t getNetID() const { return rsl_data[2].resource_id; }
};
}

}

}

#endif // DATA_MISSION_ACTOR_ID_5_HEADER
