#ifndef DATA_MISSION_ACTOR_ID_36_HEADER
#define DATA_MISSION_ACTOR_ID_36_HEADER

#include "Turret.h"

namespace Data::Mission::ACT {

class NeutralTurret : public Turret {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint8_t uint8_0;
        uint8_t uint8_1;
        uint8_t uint8_2;
        uint8_t uint8_3;
        uint8_t uint8_4;
        uint8_t uint8_5;
        uint16_t uint16_0;
    } turret_internal;

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

    virtual Resource* duplicate() const;

    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getNeutralInternal() const;
};

}

#endif // DATA_MISSION_ACTOR_ID_36_HEADER
