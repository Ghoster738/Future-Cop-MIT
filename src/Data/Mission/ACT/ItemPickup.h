#ifndef DATA_MISSION_ACTOR_ID_16_HEADER
#define DATA_MISSION_ACTOR_ID_16_HEADER

#include "../ACTResource.h"
#include <json/json.h>

namespace Data {

namespace Mission {

namespace ACT {

class ItemPickup : public ACTResource {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint32_t uint32_0;
        uint32_t uint32_1;
        uint16_t uint16_0;
        uint8_t uint8_0;
        uint32_t uint32_2;
        uint16_t uint16_1;
        uint8_t uint8_1;
        uint16_t uint16_2;
        uint16_t uint16_3;
        uint32_t uint32_3;
        uint16_t uint16_4;
        uint32_t uint32_4;
        uint32_t bitfield;
        uint16_t uint16_5;
        uint16_t rotation;
    } internal;

protected:
    virtual Json::Value makeJson() const;
    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    ItemPickup();
    ItemPickup( const ACTResource& obj );
    ItemPickup( const ItemPickup& obj );

    virtual uint_fast8_t getTypeID() const;
    virtual std::string getTypeIDName() const;

    virtual size_t getSize() const;

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;

    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;

    bool getHasItemID() const { return rsl_data[0].type != RSL_NULL_TAG; }
    uint32_t getItemID() const { return rsl_data[0].resource_id; }
};
}

}

}

#endif // DATA_MISSION_ACTOR_ID_16_HEADER
