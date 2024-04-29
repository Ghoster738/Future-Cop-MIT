#ifndef DATA_MISSION_ACTOR_ID_37_HEADER
#define DATA_MISSION_ACTOR_ID_37_HEADER

#include "../ACTManager.h"
#include <json/json.h>

namespace Data {

namespace Mission {

namespace ACT {

class SkyCaptain : public ACTResource {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint32_t uint32_0;
        uint16_t uint16_0;
        uint16_t zero_0;
        uint8_t uint8_0;
        uint8_t zero_1;
        uint8_t uint8_2;
        uint8_t uint8_3;
        uint8_t uint8_4;
        uint8_t uint8_5;
        uint16_t zero_2;
        uint16_t uint16_3;
        uint16_t uint16_4;
        uint8_t uint8_6;
        uint8_t uint8_7;
        uint16_t uint16_5;
        uint32_t uint32_1;
        uint16_t uint16_6;
        uint16_t uint16_7;
        uint8_t uint8_8;
        uint8_t uint8_9;
        uint16_t uint16_8;
        uint8_t uint8_10;
        uint8_t position_bitfield; // 0b01000 Means that Skycaptain would use an alturnative cordinate system.
        uint16_t uint16_9;
        uint16_t uint16_10;
        uint16_t uint16_11;
        uint16_t uint16_12;
        uint16_t uint16_13;
        uint16_t uint16_14;
        uint16_t uint16_15;
        uint32_t uint32_2;
        uint16_t uint16_16;
        uint16_t uint16_17;
        uint16_t alt_position_x;
        uint16_t alt_position_y;
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
    } internal;

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

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;

    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;

    glm::vec2 getSpawnPosition() const;

    static std::vector<SkyCaptain*> getVector( Data::Mission::ACTManager& act_manager );

    static const std::vector<SkyCaptain*> getVector( const Data::Mission::ACTManager& act_manager );
};
}

}

}

#endif // DATA_MISSION_ACTOR_ID_37_HEADER
