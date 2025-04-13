#ifndef MISSION_RESOURCE_DCS_HEADER
#define MISSION_RESOURCE_DCS_HEADER

#include "Resource.h"

namespace Data {

namespace Mission {

/**
 * This holds images that can be renderered
 */
class DCSResource : public Resource {
public:
    static const std::filesystem::path FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

    struct Image {
        uint8_t x;
        uint8_t y;
        uint8_t width;
        uint8_t height;
        uint8_t cbmp_id;
        // Two bytes of zeros.
        uint8_t id;
    };
private:
    std::vector<Image> images;

public:
    DCSResource();
    DCSResource( const DCSResource &obj );

    virtual std::filesystem::path getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    virtual const Image* getImage(uint8_t id) const;

    // virtual bool parse(); See Resource for documentation.
    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual int write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;

    virtual Resource * duplicate() const;
};

}

}

#endif // MISSION_RESOURCE_UNKNOWN_HEADER
