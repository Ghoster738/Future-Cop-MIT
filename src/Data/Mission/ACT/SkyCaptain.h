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
        uint8_t uint8_12;
        uint16_t zero_3;
        uint8_t zero_4;
        uint16_t uint16_21;
        uint16_t uint16_22;
        uint8_t uint8_14;
        uint8_t uint8_15;
        uint16_t uint16_23;
        uint32_t uint32_3;
        uint16_t uint16_24;
        uint16_t uint16_25;
        uint16_t uint16_26;
        uint16_t uint16_27;
        uint8_t uint8_16;
        uint8_t uint8_17;
        uint16_t uint16_28;
        uint32_t uint32_4;
        uint16_t uint16_29;
        uint16_t uint16_30;
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
