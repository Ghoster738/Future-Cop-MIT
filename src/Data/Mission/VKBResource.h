#ifndef MISSION_RESOURCE_VKB_HEADER
#define MISSION_RESOURCE_VKB_HEADER

#include "WAVResource.h"

namespace Data {

namespace Mission {

class VKBResource : public WAVResource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

public:
    VKBResource();
    VKBResource( const VKBResource &obj );

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    virtual int write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;
};

}

}

#endif // MISSION_RESOURCE_VKB_HEADER
