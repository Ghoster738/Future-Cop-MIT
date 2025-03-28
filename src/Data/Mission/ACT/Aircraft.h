#ifndef DATA_MISSION_ACTOR_ID_9_HEADER
#define DATA_MISSION_ACTOR_ID_9_HEADER

#include "BaseShooterEntity.h"

namespace Data {

namespace Mission {

namespace ACT {

/**
 * This seems to hold flying units.
 *
 * Jet Copters, and Flying Fortresses have this ACT type.
 * Also, crime war also uses this type. For example drones are found to use this type as well.
 */
class Aircraft : public BaseShooterEntity {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint8_t uint8_0;
        uint8_t uint8_1;
        uint16_t uint16_0;
        uint8_t uint8_2;
        uint8_t uint8_3;
        uint16_t uint16_1;
        uint16_t uint16_2;
        uint16_t uint16_3;
        uint16_t uint16_4;
        uint16_t uint16_5;
        uint16_t uint16_6;
        uint16_t uint16_7;
        uint16_t uint16_8;
        uint16_t uint16_9;
        uint16_t uint16_10;
        uint16_t uint16_11;
        uint16_t uint16_12;
        uint16_t uint16_13;
    } internal;

protected:
    virtual Json::Value makeJson() const;
    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    Aircraft();
    Aircraft( const ACTResource& obj );
    Aircraft( const BaseShooterEntity& obj );
    Aircraft( const Aircraft& obj );

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

#endif // DATA_MISSION_ACTOR_ID_9_HEADER
