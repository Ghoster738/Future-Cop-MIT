#ifndef MISSION_RESOURCE_RPNS_HEADER
#define MISSION_RESOURCE_RPNS_HEADER

#include "Resource.h"

namespace Data {

namespace Mission {

class RPNSResource : public Resource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;
    
    struct Bitfield {
        uint8_t a : 2;
        uint8_t b : 6;
    };
    
private:
    std::vector<Bitfield> bitfields;

public:
    RPNSResource();
    RPNSResource( const RPNSResource &obj );

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    // virtual bool parse(); See Resource for documentation.
    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual int write( const std::string& file_path, const std::vector<std::string> & arguments ) const;

    virtual Resource * duplicate() const;
};

}

}

#endif // MISSION_RESOURCE_RPNS_HEADER
