#ifndef MISSION_RESOURCE_AIFF_HEADER
#define MISSION_RESOURCE_AIFF_HEADER

#include "WAVResource.h"

namespace Data {
namespace Mission {

class AIFFResource : public WAVResource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

public:
    AIFFResource();
    AIFFResource( const AIFFResource &obj );

    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    virtual int write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;

};

}
}

#endif // MISSION_RESOURCE_AIFF_HEADER
