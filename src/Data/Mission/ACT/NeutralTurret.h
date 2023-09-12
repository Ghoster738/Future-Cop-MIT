#ifndef MISSION_ACT_TYPE_NEUTRAL_TURRET_HEADER
#define MISSION_ACT_TYPE_NEUTRAL_TURRET_HEADER

#include "../ACTManager.h"
#include <json/json.h>

namespace Data {

namespace Mission {

namespace ACT {

class NeutralTurret : public ACTResource {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint32_t uint32_0;
        uint16_t uint16_0;    // 0
        uint8_t   uint8_0[10];
        uint16_t uint16_1[2]; // 1
        uint16_t  uint8_1[2];
        uint16_t uint16_2[5]; // 2
        uint16_t  uint8_2[2];
        uint16_t uint16_3[5]; // 3
        uint8_t   uint8_3[6];
        uint16_t uint16_4;    // 4
        uint8_t   uint8_4[6];
        uint16_t uint16_5;    // 5
    } internal;

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

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;
    
    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;

    static std::vector<NeutralTurret*> getVector( Data::Mission::ACTManager& act_manager );
    static const std::vector<NeutralTurret*> getVector( const Data::Mission::ACTManager& act_manager );
};

}

}

}

#endif // MISSION_ACT_TYPE_NEUTRAL_TURRET_HEADER
