#ifndef DATA_MISSION_ACTOR_ID_95_HEADER
#define DATA_MISSION_ACTOR_ID_95_HEADER

#include "../ACTManager.h"
#include <json/json.h>

namespace Data {

namespace Mission {

namespace ACT {

class Trigger : public ACTResource {
public:
    static uint_fast8_t TYPE_ID;

    struct Internal {
        uint16_t width;
        uint16_t height;
        uint16_t uint16_2;
        uint8_t uint8_0;
        uint8_t type;
        uint16_t test_id;
        uint16_t zero;
    } internal;

protected:
    virtual Json::Value makeJson() const;
    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );

public:
    Trigger();
    Trigger( const ACTResource& obj );
    Trigger( const Trigger& obj );

    virtual uint_fast8_t getTypeID() const;
    virtual std::string getTypeIDName() const;

    virtual size_t getSize() const;

    virtual bool checkRSL() const;

    virtual Resource* duplicate() const;

    virtual ACTResource* duplicate( const ACTResource &original ) const;

    Internal getInternal() const;

    static std::vector<Trigger*> getVector( Data::Mission::ACTManager& act_manager );

    static const std::vector<Trigger*> getVector( const Data::Mission::ACTManager& act_manager );

};
}

}

}

#endif // DATA_MISSION_ACTOR_ID_95_HEADER
