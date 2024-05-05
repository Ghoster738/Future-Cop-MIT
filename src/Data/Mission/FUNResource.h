#ifndef MISSION_RESOURCE_FUN_HEADER
#define MISSION_RESOURCE_FUN_HEADER

#include "Resource.h"

namespace Data {

namespace Mission {

class FUNResource : public Resource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;
    
    struct Function {
        int32_t faction; //    Wild Guess
        int32_t identifier; // Wild Guess
        int32_t zero; // Always zero.
        uint32_t start_parameter_offset;
        uint32_t start_code_offset;
    };

    static uint32_t getNumber(uint8_t *bytes_r, size_t &position);
    
private:
    std::vector<Function> functions;
    std::vector<uint8_t> ext_bytes;

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
};

}

}

#endif // MISSION_RESOURCE_UNKNOWN_HEADER
