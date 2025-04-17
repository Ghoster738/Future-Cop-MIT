#ifndef DATA_MISSION_ACTOR_ID_37_HEADER
#define DATA_MISSION_ACTOR_ID_37_HEADER

#include "Aircraft.h"

namespace Data {

namespace Mission {

namespace ACT {

class SkyCaptain : public Aircraft {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint8_t    uint8_0;
        uint16_t    zero_0;
        uint8_t     zero_1;
        uint16_t  uint16_1;
        uint16_t  uint16_2;
        uint8_t    uint8_1;
        uint8_t    uint8_2;
        uint16_t  uint16_3;
        uint32_t  uint32_0;
        uint16_t  uint16_4;
        uint16_t  uint16_5;
        uint16_t  uint16_6;
        uint16_t  uint16_7;
        uint8_t    uint8_3;
        uint8_t    uint8_4;
        uint16_t  uint16_8;
        uint32_t  uint32_1;
        uint16_t  uint16_9;
        uint16_t uint16_10;
    } sky_internal;

protected:
    virtual Json::Value makeJson() const;
    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    SkyCaptain();
    SkyCaptain( const ACTResource& obj );
    SkyCaptain( const SkyCaptain& obj );

    virtual uint_fast8_t getTypeID() const;
    virtual std::string getTypeIDName() const;

    virtual size_t getSize() const;

    virtual Resource* duplicate() const;

    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getSkyInternal() const;
};
}

}

}

#endif // DATA_MISSION_ACTOR_ID_37_HEADER
