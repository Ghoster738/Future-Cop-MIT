#ifndef MISSION_RESOURCE_SHD_HEADER
#define MISSION_RESOURCE_SHD_HEADER

#include "Resource.h"

namespace Data {

namespace Mission {

class SHDResource : public Resource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

    struct Entry {
        uint16_t group_id;
        uint16_t sound_id;

        uint8_t unk_0;
        uint8_t unk_1;

        uint8_t loop;
        uint8_t unk_2;

        uint8_t script_id;
        uint8_t unk_3;

        uint8_t unk_4;
        uint8_t unk_5;
    };

private:
    std::vector<Entry> entries;

public:
    SHDResource();
    SHDResource( const SHDResource &obj );

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    virtual bool noResourceID() const;

    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    virtual int write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;

    const std::vector<Entry>& getEntries() const { return entries; }
};

}

}

#endif // MISSION_RESOURCE_SHD_HEADER
