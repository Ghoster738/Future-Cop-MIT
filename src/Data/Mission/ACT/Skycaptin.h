#ifndef MISSION_ACT_TYPE_SKYCAPTIN_HEADER
#define MISSION_ACT_TYPE_SKYCAPTIN_HEADER

#include "../ACTManager.h"

namespace Data {

namespace Mission {

namespace ACT {

class Skycaptin : public ACTResource {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        // 4 zero bytes?
        uint32_t  pos_y; // position y
        uint32_t  uint32_0;
        uint16_t  uint16_0;
        // 2 zero bytes?
        uint8_t    bytes_0[ 6 ];
        // 2 zero bytes?
        uint16_t  uint16_1[ 2 ];
        uint8_t    bytes_1[ 2 ];
        uint16_t  uint16_2;
        uint32_t  uint32_1; // ? Can be two uint16
        uint16_t  uint16_3[ 2 ];
        uint8_t    bytes_2[ 2 ];
        uint16_t  uint16_4;
        uint8_t    bytes_3[ 2 ];
        uint16_t  uint16_5[ 7 ];
        uint32_t  uint32_2; // ? Can be two uint16
        uint16_t  uint16_6[ 2 ];
        // 4 zero bytes?
        uint8_t    byte_4;
        // 3 zero bytes?
        uint16_t  uint16_7[ 2 ];
        uint8_t    bytes_5[ 2 ];
        uint16_t  uint16_8;
        // 4 zero bytes?
        uint16_t  uint16_9[ 4 ];
        uint8_t    bytes_6[ 2 ];
        uint16_t uint16_10;
        uint32_t  uint32_3; // ? Can be two uint16
        uint16_t uint16_11[ 2 ];
    } internal;

protected:
    virtual Json::Value makeJson() const;

    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );
public:
    Skycaptin();
    Skycaptin( const ACTResource& obj );
    Skycaptin( const Skycaptin& obj );

    virtual uint_fast8_t getTypeID() const;
    virtual std::string getTypeIDName() const;

    virtual size_t getSize() const;

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;
    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;

    static std::vector<Skycaptin*> getVector( Data::Mission::ACTManager& act_manager );
    static const std::vector<Skycaptin*> getVector( const Data::Mission::ACTManager& act_manager );
};

}

}

}

#endif
