#ifndef DATA_MISSION_ACTOR_ID_6_HEADER
#define DATA_MISSION_ACTOR_ID_6_HEADER

#include "BaseTurret.h"

namespace Data {

namespace Mission {

namespace ACT {

class StationaryActor : public BaseTurret {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint16_t uint16_0;
    } internal;

protected:
    virtual Json::Value makeJson() const;
    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    StationaryActor();
    StationaryActor( const ACTResource& obj );
    StationaryActor( const StationaryActor& obj );

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

#endif // DATA_MISSION_ACTOR_ID_6_HEADER
