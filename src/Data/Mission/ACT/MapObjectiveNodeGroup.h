#ifndef DATA_MISSION_ACTOR_ID_35_HEADER
#define DATA_MISSION_ACTOR_ID_35_HEADER

#include "../ACTResource.h"
#include <json/json.h>

namespace Data {

namespace Mission {

namespace ACT {

class MapObjectiveNodeGroup : public ACTResource {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint32_t uint32_0;
        uint16_t uint16_0;
        uint32_t uint32_1;
        uint32_t uint32_2;
        uint16_t uint16_1;
        uint8_t uint8_0;
        uint16_t uint16_2;
        uint8_t uint8_1;

        uint32_t uint32_3;

        uint8_t uint8_2;
        uint8_t uint8_3;
        uint8_t uint8_4;
        uint16_t uint16_3;
        uint8_t uint8_5;
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
        uint16_t uint16_14;
        uint16_t uint16_15;
        uint16_t uint16_16;
        uint16_t uint16_17;
        uint16_t uint16_18;
        uint16_t uint16_19;
        uint16_t uint16_20;
    } internal;

protected:
    virtual Json::Value makeJson() const;
    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    MapObjectiveNodeGroup();
    MapObjectiveNodeGroup( const ACTResource& obj );
    MapObjectiveNodeGroup( const MapObjectiveNodeGroup& obj );

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

#endif // DATA_MISSION_ACTOR_ID_35_HEADER
