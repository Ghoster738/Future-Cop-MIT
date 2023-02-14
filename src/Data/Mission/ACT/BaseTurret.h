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
        uint32_t  rotation_value; // Guessed
        uint16_t  uint16_0;
        uint16_t  uint16_1;
        uint8_t   byte_0;
        uint8_t   byte_1;
        uint8_t   byte_2;
        uint8_t   byte_3;
        uint8_t   byte_4;
        // Zero Byte
        uint8_t   byte_5;
        // Zero Byte
        uint16_t  uint16_2;
        // Zero Byte
        uint8_t   byte_6;
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
