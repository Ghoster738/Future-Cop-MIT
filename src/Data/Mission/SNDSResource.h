#ifndef MISSION_RESOURCE_SNDS_HEADER
#define MISSION_RESOURCE_SNDS_HEADER

#include "WAVResource.h"

namespace Data {

namespace Mission {

class SNDSResource : public Resource {
public:
    static const std::filesystem::path FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

private:
    WAVResource sound;

public:
    SNDSResource();
    SNDSResource( const SNDSResource &obj );

    virtual std::filesystem::path getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    virtual bool noResourceID() const;

    const WAVResource *const soundAccessor() const { return &sound; }

    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual int write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;

    virtual Resource * duplicate() const;
};

}

}

#endif // MISSION_RESOURCE_SNDS_HEADER
