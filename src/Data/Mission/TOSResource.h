#ifndef MISSION_RESOURCE_TOS_HEADER
#define MISSION_RESOURCE_TOS_HEADER

#include "Resource.h"

namespace Data {

namespace Mission {

class TOSResource : public Resource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

private:
    // These probably are offsets or identifiers to SWVR/SNDS sound data and ANM data.
    std::vector<uint32_t> offsets;

public:
    TOSResource();
    TOSResource( const TOSResource &obj );

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    virtual int write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;

    const std::vector<uint32_t>& getOffsets() const { return offsets; }
};

}

}

#endif // MISSION_RESOURCE_TOS_HEADER
