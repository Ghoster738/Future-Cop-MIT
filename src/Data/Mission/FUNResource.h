#ifndef MISSION_RESOURCE_FUN_HEADER
#define MISSION_RESOURCE_FUN_HEADER

#include "Resource.h"

namespace Data {

namespace Mission {

class FUNResource : public Resource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

public:
    FUNResource();
    FUNResource( const FUNResource &obj );

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    // virtual bool parse(); See Resource for documentation.
    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual int write( const char *const file_path, const std::vector<std::string> & arguments ) const;

    virtual Resource * duplicate() const;
};

}

}

#endif // MISSION_RESOURCE_UNKNOWN_HEADER
