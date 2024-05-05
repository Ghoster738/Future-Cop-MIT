#ifndef MISSION_RESOURCE_FUN_HEADER
#define MISSION_RESOURCE_FUN_HEADER

#include "Resource.h"

namespace Data {

namespace Mission {

class FUNResource : public Resource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

    static const float FUNCTION_TIME_UNITS_TO_SECONDS;
    
    struct Function {
        int32_t how_many_times;
        int32_t time_units;
        int32_t zero; // Always zero.
        uint32_t start_parameter_offset;
        uint32_t start_code_offset;
    };

    static uint32_t getNumber(uint8_t *bytes_r, size_t &position);
    
private:
    std::vector<Function> functions;
    std::vector<uint8_t> ext_bytes;

    // TODO Remove these temporary variables.
    std::vector<uint32_t> spawn_actors_now;
    bool spawn_all_neutral_turrets;
public:
    FUNResource();
    FUNResource( const FUNResource &obj );

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    // virtual bool parse(); See Resource for documentation.
    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual int write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;

    virtual Resource * duplicate() const;
    
    std::vector<uint8_t> getFunctionParameters( unsigned index ) const;
    std::vector<uint8_t> getFunctionCode( unsigned index ) const;

    // TODO Remove these temporary variables.
    const std::vector<uint32_t>& getSpawnActorsNow() const { return spawn_actors_now; }
    bool getSpawnAllNeutralTurrets() const { return spawn_all_neutral_turrets; }
};

}

}

#endif // MISSION_RESOURCE_UNKNOWN_HEADER
