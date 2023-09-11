#ifndef MISSION_ACT_TYPE_X1A_UNIT_HEADER
#define MISSION_ACT_TYPE_X1A_UNIT_HEADER

#include "../ACTManager.h"
#include <json/json.h>

namespace Data {

namespace Mission {

namespace ACT {

/**
 * This holds an X1A unit.
 *
 * This is the vechilce that you pilot in the game.
 */
class X1Alpha : public ACTResource {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint16_t  uint16_0;
        uint16_t  uint16_1;
        uint8_t   byte_0;
        uint8_t   byte_1;
        uint8_t   byte_2;
        uint8_t   byte_3;
        uint8_t   byte_4;
        // Zero Byte
        uint8_t   byte_5;
        // Zero Byte
        uint16_t  uint16_2;
        // Zero Byte
        uint8_t   byte_6;
    } internal;

protected:
    virtual Json::Value makeJson() const;

    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );
public:
    X1Alpha();
    X1Alpha( const ACTResource& obj );
    X1Alpha( const X1Alpha& obj );

    virtual uint_fast8_t getTypeID() const;
    virtual std::string getTypeIDName() const;

    virtual size_t getSize() const;

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;
    
    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;

    static std::vector<X1Alpha*> getVector( Data::Mission::ACTManager& act_manager );
    static const std::vector<X1Alpha*> getVector( const Data::Mission::ACTManager& act_manager );
};

}

}

}

#endif // MISSION_ACT_TYPE_X1A_UNIT_HEADER
