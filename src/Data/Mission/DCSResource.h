#ifndef MISSION_RESOURCE_DCS_HEADER
#define MISSION_RESOURCE_DCS_HEADER

#include "Resource.h"

namespace Data {

namespace Mission {

/**
 * I do not know what this does yet.
 * This might have something to do with the Cptr, but I am not sure.
 */
class DCSResource : public Resource {
public:
    static const std::filesystem::path FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

    struct Element {
        uint8_t unk_0;
        uint8_t unk_1;
        uint8_t unk_2;
        uint8_t unk_3;
        uint8_t unk_4;
        // Two bytes of zeros.
        uint8_t unk_5;// Seemed to be one byte for count starting with 1 at the beginning, but not always true.
    };
private:
    std::vector<Element> element;

public:
    DCSResource();
    DCSResource( const DCSResource &obj );

    virtual std::filesystem::path getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    // TODO Add acessor methods.

    // virtual bool parse(); See Resource for documentation.
    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual int write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;

    virtual Resource * duplicate() const;
};

}

}

#endif // MISSION_RESOURCE_UNKNOWN_HEADER
