#ifndef MISSION_RESOURCE_SNDS_HEADER
#define MISSION_RESOURCE_SNDS_HEADER

#include "WAVResource.h"

namespace Data {

namespace Mission {

class SNDSResource : public Resource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;
private:
    WAVResource sound;
public:
    SNDSResource();
    SNDSResource( const SNDSResource &obj );

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual int write( const std::string& file_path, const std::vector<std::string> & arguments ) const;

    virtual Resource * duplicate() const;

    static std::vector<SNDSResource*> getVector( IFF &mission_file );
    static const std::vector<SNDSResource*> getVector( const IFF &mission_file );
};

}

}

#endif // MISSION_RESOURCE_SNDS_HEADER
