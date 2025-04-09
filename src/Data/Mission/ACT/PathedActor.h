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
        uint16_t uint16_9;
        uint16_t uint16_10;
        uint16_t uint16_11;
        uint16_t uint16_12;
        uint16_t uint16_13;
        uint16_t uint16_14;
        uint16_t uint16_15;
        uint16_t uint16_16;
        uint16_t uint16_17;
        uint8_t uint8_10;
        uint8_t uint8_11;

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
};
}

}

}

#endif // DATA_MISSION_ACTOR_ID_5_HEADER
