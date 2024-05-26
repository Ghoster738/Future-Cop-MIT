#ifndef MISSION_RESOURCE_TOS_HEADER
#define MISSION_RESOURCE_TOS_HEADER

#include "Resource.h"

namespace Data {

namespace Mission {

class TOSResource : public Resource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

public:
    TOSResource();
    TOSResource( const TOSResource &obj );

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    virtual int write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;

    static std::vector<TOSResource*> getVector( IFF &mission_file );
    static const std::vector<TOSResource*> getVector( const IFF &mission_file );
};

}

}

#endif // MISSION_RESOURCE_TOS_HEADER
