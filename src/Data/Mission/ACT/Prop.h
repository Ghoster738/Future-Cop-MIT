#ifndef MISSION_ACT_TYPE_PROP_HEADER
#define MISSION_ACT_TYPE_PROP_HEADER

#include "../ACTManager.h"
#include <json/json.h>

namespace Data {

namespace Mission {

namespace ACT {

/**
 * This holds the mission's props.
 *
 * They are mearly decorations.
 *
 * Fun Fact: This is the first ACT resource that I added to the system.
 */
class Prop : public ACTResource {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint16_t  uint16_0; // Unknown
        uint16_t  uint16_1; // Unknown
        uint16_t  uint16_2; // Unknown
        uint16_t  uint16_3; // Time or angle until loop back
        uint8_t   byte_0; // Unknown
        uint8_t   byte_1; // wild guess layer???
        uint8_t   byte_2; // Unknown
        uint8_t   matching_bytes[3]; // 0-2 seem to be 0x40 for ConFt. For 0x17 HK
        uint8_t   spin; // Spin value
        uint8_t   byte_3;
    } internal;

protected:
    virtual Json::Value makeJson() const;

    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );
public:
    Prop();
    Prop( const ACTResource& obj );
    Prop( const Prop& obj );

    virtual uint_fast8_t getTypeID() const;
    virtual std::string getTypeIDName() const;

    virtual size_t getSize() const;

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;
    
    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;

    uint32_t getObjResourceID() const { return rsl_data[0].index; }
    float getRotation() const;

    static std::vector<Prop*> getVector( Data::Mission::ACTManager& act_manager );
    static const std::vector<Prop*> getVector( const Data::Mission::ACTManager& act_manager );
};

}

}

}

#endif
