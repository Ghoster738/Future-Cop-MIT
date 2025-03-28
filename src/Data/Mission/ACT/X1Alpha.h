#ifndef DATA_MISSION_ACTOR_ID_1_HEADER
#define DATA_MISSION_ACTOR_ID_1_HEADER

#include "BaseEntity.h"

namespace Data::Mission::ACT {

/**
 * This holds an X1A unit.
 *
 * This is the vehicle that you pilot in the game.
 */
class X1Alpha : public BaseEntity {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint16_t rotation;
        uint8_t uint8_0;
        uint8_t uint8_1;
    } internal;

protected:
    virtual Json::Value makeJson() const;
    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    X1Alpha();
    X1Alpha( const ACTResource& obj );
    X1Alpha( const BaseEntity& obj );
    X1Alpha( const X1Alpha& obj );

    virtual uint_fast8_t getTypeID() const;
    virtual std::string getTypeIDName() const;

    virtual size_t getSize() const;

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;

    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;
};

}

#endif // DATA_MISSION_ACTOR_ID_1_HEADER
