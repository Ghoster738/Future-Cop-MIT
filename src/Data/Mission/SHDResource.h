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
        PS1_GUI = 0,
        GLOBAL  = 1,
        MISSION = 2,
        UNKNOWN = 3,
        ALL     = 4
    };

    // This is used by the weapons of X1A.
    // The estimated max OptionalEntries used is 4.
    struct OptionalEntry {
        uint16_t id; // Guess
        uint16_t count;
        uint16_t index; // starts out as offsets, but gets converted to indexes.
    };

    struct Entry {
        uint16_t group_id; // Used for random!
        uint16_t sound_id; // Resource ID

        uint8_t unk_metadata; // Either 7 or 9. Purpose unknown.
        uint8_t sound_limit;  // This is an integer between [1, 5]. Values inbetween 1 through 5 were found no gaps.

        uint8_t loop;        // Either 0 or 1.
        uint8_t unk_boolean; // Either 0 or 1. Purpose unknown but if this is off then sound might not play.

        uint8_t script_id; // ID used in script or assembly code.
        uint8_t zero_0;

        uint8_t zero_1;
        uint8_t zero_2;

        std::string getString() const;
    };

private:
    uint16_t id_0; // Might be used to determine Type of SHD Resource.
    uint16_t id_1; // Might be used to determine Type of SHD Resource.
    uint16_t entry_count; // This might not be a counter for the entries. Maybe this is to indicate what is loaded in by default?

    std::vector<OptionalEntry> optional_entires; // GlblData has these.
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
