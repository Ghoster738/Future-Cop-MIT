#ifndef DATA_MISSION_ACTOR_ID_11_HEADER
#define DATA_MISSION_ACTOR_ID_11_HEADER

#include "BaseEntity.h"
#include <json/json.h>

namespace Data {

namespace Mission {

namespace ACT {

class DynamicProp : public BaseEntity {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint8_t uint8_8;
        uint8_t uint8_9;
        uint16_t uint16_2;
        uint32_t uint32_1;
    } internal;

protected:
    virtual Json::Value makeJson() const;
    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    DynamicProp();
    DynamicProp( const ACTResource& obj );
    DynamicProp( const DynamicProp& obj );

    virtual uint_fast8_t getTypeID() const;
    virtual std::string getTypeIDName() const;

    virtual size_t getSize() const;

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;

    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;

    uint32_t getObjResourceID() const { return rsl_data[0].resource_id; }
};
}

}

}

#endif // DATA_MISSION_ACTOR_ID_11_HEADER
