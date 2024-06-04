#ifndef MISSION_RESOURCE_MSIC_HEADER
#define MISSION_RESOURCE_MSIC_HEADER

#include "WAVResource.h"

namespace Data {

namespace Mission {

class MSICResource : public Resource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

private:
    WAVResource sound;

public:
    MSICResource();
    MSICResource( const MSICResource &obj );

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    const WAVResource *const soundAccessor() const { return &sound; }

    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual int write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;

    virtual Resource * duplicate() const;
};

}

}

#endif // MISSION_RESOURCE_MSIC_HEADER
