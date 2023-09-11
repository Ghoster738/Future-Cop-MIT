#ifndef MISSION_ACT_TYPE_BASE_TURRET_HEADER
#define MISSION_ACT_TYPE_BASE_TURRET_HEADER

#include "../ACTManager.h"
#include <json/json.h>

namespace Data {

namespace Mission {

namespace ACT {

/**
 * This holds the base turret.
 *
 * This is the turret that always has the same side.
 */
class BaseTurret : public ACTResource {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint16_t  uint16_0; // 28
        uint16_t  uint16_0b; // 30
        uint8_t   byte_0; // 32
        uint8_t   byte_0b; // 33
        uint8_t   byte_1; // 34
        uint8_t   byte_1b; // 35
        uint8_t   byte_2; // 36
        uint8_t   byte_2b; // 37
        uint8_t   byte_3b; // 38
        uint8_t   byte_4b; // 39
        uint16_t  uint16_1; // 40
        uint16_t  uint16_2; // 42
        uint8_t   byte_3; // 44
        uint8_t   byte_4; // 45
        uint16_t  uint16_3; // 46
        uint16_t  uint16_1b; // 48
        uint16_t  uint16_2b; // 50
        uint16_t  uint16_4; // 52
        uint16_t  uint16_3b; // 54
        uint8_t   byte_5b; // 56
        uint8_t   byte_5; // 57
        uint16_t  uint16_5; // 58
        uint16_t  uint16_6; // 60
        uint16_t  uint16_4b; // 62
        uint16_t  uint16_7;
        uint16_t  uint16_8;
        uint8_t   byte_6;
        // 3 zero bytes
        uint8_t   byte_6b;
        // 1 zero byte
        uint16_t  uint16_9;
        
    } internal;

protected:
    virtual Json::Value makeJson() const;

    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );
public:
    BaseTurret();
    BaseTurret( const ACTResource& obj );
    BaseTurret( const BaseTurret& obj );

    virtual uint_fast8_t getTypeID() const;
    virtual std::string getTypeIDName() const;

    virtual size_t getSize() const;

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;
    
    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;

    static std::vector<BaseTurret*> getVector( Data::Mission::ACTManager& act_manager );
    static const std::vector<BaseTurret*> getVector( const Data::Mission::ACTManager& act_manager );
};

}

}

}

#endif // MISSION_ACT_TYPE_X1A_UNIT_HEADER
