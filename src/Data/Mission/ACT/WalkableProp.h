#ifndef DATA_MISSION_ACTOR_ID_12_HEADER
#define DATA_MISSION_ACTOR_ID_12_HEADER

#include "BaseEntity.h"

namespace Data {

namespace Mission {

namespace ACT {

class WalkableProp : public BaseEntity {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint16_t uint16_0;
        uint16_t uint16_1;
        uint16_t uint16_2;
        uint16_t uint16_3;
    } internal;

protected:
    virtual Json::Value makeJson() const;
    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    WalkableProp();
    WalkableProp( const ACTResource& obj );
    WalkableProp( const WalkableProp& obj );

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

#endif // DATA_MISSION_ACTOR_ID_12_HEADER
