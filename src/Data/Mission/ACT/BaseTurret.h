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

    // Ghoster738 - I could be reading BahKooj's notes wrong.
    struct Internal {
        uint32_t  rotation; // 28
        uint16_t  uint16_0; // 32
        // 2 zero bytes // 34
        uint8_t   byte_0; // hostile_to_it 36
        // zero byte //37
        uint8_t   byte_1; // minimap_color 38
        // zero byte // 39
        uint8_t   byte_2; // 40
        // 3 zero bytes // 41 Note: 43 - texture offsets
        uint16_t  uint16_1; // 43
        uint16_t  uint16_2; // 45
        uint8_t   byte_3; // 46
        uint8_t   byte_4; // 47
        uint16_t  uint16_3; // 48
        // 4 zero bytes // 50 is where the team turret is hostile toward
        uint16_t  uint16_4;
        // 3 zero bytes
        uint8_t   byte_5;
        uint16_t  uint16_5;
        uint16_t  uint16_6;
        // 2 zero bytes
        uint16_t  uint16_7;
        uint16_t  uint16_8;
        uint8_t   byte_6;
        // 5 zero bytes
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
