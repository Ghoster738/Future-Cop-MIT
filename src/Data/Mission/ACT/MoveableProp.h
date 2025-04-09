#ifndef DATA_MISSION_ACTOR_ID_25_HEADER
#define DATA_MISSION_ACTOR_ID_25_HEADER

#include "BaseEntity.h"

namespace Data {

namespace Mission {

namespace ACT {

class MoveableProp : public BaseEntity {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint8_t uint8_0;
        uint8_t uint8_1;
        uint8_t uint8_2;
        uint8_t uint8_3;
        uint16_t uint16_0;
        uint16_t uint16_1;
        uint32_t uint32_0;
        uint32_t uint32_1;
    } internal;

protected:
    virtual Json::Value makeJson() const;
    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    MoveableProp();
    MoveableProp( const ACTResource& obj );
    MoveableProp( const MoveableProp& obj );

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

#endif // DATA_MISSION_ACTOR_ID_25_HEADER
