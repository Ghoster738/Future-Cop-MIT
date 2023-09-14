#ifndef DATA_MISSION_ACTOR_ID_1_HEADER
#define DATA_MISSION_ACTOR_ID_1_HEADER

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
        uint32_t uint32_0;
        uint16_t uint16_0;
        uint16_t uint16_1;
        uint8_t uint8_0;
        uint8_t uint8_1;
        uint8_t uint8_2;
        uint8_t uint8_3;
        uint8_t uint8_4;
        uint8_t zero_0;
        uint8_t uint8_6;
        uint8_t zero_1;
        uint16_t uint16_2;
        uint8_t uint8_8;
        uint8_t uint8_9;
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

#endif // DATA_MISSION_ACTOR_ID_1_HEADER
