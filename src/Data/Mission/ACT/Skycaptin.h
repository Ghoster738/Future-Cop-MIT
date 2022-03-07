#ifndef MISSION_ACT_TYPE_SKYCAPTIN_HEADER
#define MISSION_ACT_TYPE_SKYCAPTIN_HEADER

#include "../ACTManager.h"

namespace Data {

namespace Mission {

namespace ACT {

class Skycaptin : public ACTResource {
public:
    static uint_fast16_t TYPE_ID; // = 37;

    struct Internal {
        uint32_t  uint32_0;
        // 4 zero bytes?
        uint32_t  uint32_1[ 2 ];
        uint16_t  uint16_0;
        // 2 zero bytes?
        uint8_t    bytes_0[ 6 ];
        // 2 zero bytes?
        uint16_t  uint16_1[ 2 ];
        uint8_t    bytes_1[ 2 ];
        uint16_t  uint16_2;
        uint32_t  uint32_2; // ? Can be two uint16
        uint16_t  uint16_3[ 2 ];
        uint8_t    bytes_2[ 2 ];
        uint16_t  uint16_4;
        uint8_t    bytes_3[ 2 ];
        uint16_t  uint16_5[ 7 ];
        uint32_t  uint32_3; // ? Can be two uint16
        uint16_t  uint16_6[ 2 ];
        // 4 zero bytes?
        uint8_t    bytes_4;
        // 3 zero bytes?
        uint16_t  uint16_7[ 2 ];
        uint8_t    bytes_5[ 2 ];
        uint16_t  uint16_8;
        // 4 zero bytes?
        uint16_t  uint16_9[ 4 ];
        uint8_t    bytes_6[ 2 ];
        uint16_t uint16_10;
        uint32_t  uint32_4; // ? Can be two uint16
        uint16_t uint16_11[ 2 ];
    } internal;

protected:
    virtual Json::Value makeJson() const;

    virtual bool readACTType( uint_fast8_t act_type, const uint8_t *const chunk_data_r, size_t chunk_size, bool swap_endian );
public:
    Skycaptin();

    virtual uint_fast8_t getTypeID() const;

    virtual size_t getSize() const;

    virtual bool checkRSL() const;

    virtual Resource * duplicate() const;

    Internal getInternal() const;

    static std::vector<Skycaptin*> getVector( Data::Mission::ACTManager& act_manager );
    static const std::vector<Skycaptin*> getVector( const Data::Mission::ACTManager& act_manager );
};

}

}

}

#endif
