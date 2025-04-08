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
        int16_t rotation_y;
        int16_t rotation_x;
        int16_t height_offset;
        uint8_t tile_effect; // From See TilResource for details on tile_effect
        uint8_t uint8_0;
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
