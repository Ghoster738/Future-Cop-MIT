#ifndef MISSION_ACT_TYPE_AIRCRAFT_HEADER
#define MISSION_ACT_TYPE_AIRCRAFT_HEADER

#include "../ACTManager.h"
#include <json/json.h>

namespace Data {

namespace Mission {

namespace ACT {

/**
 * This seems to hold flying units.
 *
 * Jet Copters, and Flying Fortresses have this ACT type.
 * Also, crime war also uses this type. For example drones are found to use this type as well.
 */
class Aircraft : public ACTResource {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint16_t  uint16_20;
        uint16_t  uint16_0;
        uint16_t  uint16_1; // Zero 32
        uint8_t   uint8_0;
        uint8_t   uint8_1;
        uint8_t   uint8_2;
        uint8_t   uint8_3; // Zero 36
        uint8_t   uint8_4;
        uint8_t   uint8_5;// Zero
        uint8_t   uint8_6;
        uint8_t   uint8_7; // 40
        uint16_t  uint16_2;
        uint8_t   uint8_8;
        uint8_t   uint8_9; // 44
        uint8_t   uint8_10;
        uint8_t   uint8_11; // 46
        uint16_t  uint16_3;
        uint16_t  uint16_4; // uint16_4 and uint16_5 could be a uint32_t value.
        uint16_t  uint16_5; // 52
        uint16_t  uint16_6;
        uint16_t  uint16_7; // 56
        uint8_t   uint8_12;
        uint8_t   uint8_13;
        uint16_t  uint16_8; // 60
        uint8_t   uint8_14;
        uint8_t   uint8_15;
        uint16_t  uint16_9;
        uint16_t  uint16_10;
        uint16_t  uint16_11;
        uint16_t  uint16_12;
        uint16_t  uint16_13;
        uint16_t  uint16_14;
        uint16_t  uint16_15;
        uint16_t  uint16_16;
        uint16_t  uint16_17;
        uint16_t  uint16_18;
        uint16_t  uint16_19;
        uint32_t  uint32_0; // Zero
    } internal;

protected:
    virtual Json::Value makeJson() const;

    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );
public:
    Aircraft();
    Aircraft( const ACTResource& obj );
    Aircraft( const Aircraft& obj );

    virtual uint_fast8_t getTypeID() const;
    virtual std::string getTypeIDName() const;

    virtual size_t getSize() const;

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;
    
    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;

    static std::vector<Aircraft*> getVector( Data::Mission::ACTManager& act_manager );
    static const std::vector<Aircraft*> getVector( const Data::Mission::ACTManager& act_manager );
};

}

}

}

#endif // MISSION_ACT_TYPE_AIRCRAFT_HEADER
