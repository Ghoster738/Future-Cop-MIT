#ifndef MISSION_RESOURCE_SHD_HEADER
#define MISSION_RESOURCE_SHD_HEADER

#include "Resource.h"

#include <string>
#include <vector>

namespace Data {

namespace Mission {

class SHDResource : public Resource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

    enum Type {
        PS1_GLOBAL = 0,
        GLOBAL     = 1,
        MISSION    = 2,
        UNKNOWN    = 3,
        ALL        = 4
    };

    struct Entry {
        uint16_t group_id; // Used for random!
        uint16_t sound_id; // Resource ID

        uint8_t unk_0;
        uint8_t unk_1;

        uint8_t loop;
        uint8_t unk_2;

        uint8_t script_id; // ID used in script or assembly code.
        uint8_t zero_0;

        uint8_t zero_1;
        uint8_t zero_2;

        std::string getString() const;
    };

private:
    uint16_t unk_0; // Used to determine Type of SHD Resource.
    uint16_t unk_1; // Used to determine Type of SHD Resource.
    uint16_t entry_count; // This might not be a counter for the entries. Maybe this is to indicate what is loaded in by default?

    std::vector<uint16_t> unknowns; // GlblData has that. I do not know what they do.
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

    Type getType() const;

    static std::string typeToString(Type type);
};

}

}

#endif // MISSION_RESOURCE_SHD_HEADER
