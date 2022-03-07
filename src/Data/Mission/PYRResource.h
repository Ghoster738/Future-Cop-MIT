#ifndef MISSION_RESOURCE_PLY_HEADER
#define MISSION_RESOURCE_PLY_HEADER

#include "Resource.h"
#include "../../Utilities/ImageData.h"
#include "../../Utilities/DataTypes.h"

namespace Data {

namespace Mission {

class PYRIcon {
private:
    uint16_t id;
    Utilities::DataTypes::Vec2UByte location;
    Utilities::DataTypes::Vec2UByte size;

    Utilities::ImageData palette;
public:
    PYRIcon( uint8_t * data );

    uint16_t getID() const;
    Utilities::DataTypes::Vec2UByte getLocation() const;
    Utilities::DataTypes::Vec2UByte getSize() const;

    const Utilities::ImageData* getPalette() const;
    Utilities::ImageData* getPalette();
};

class PYRResource : public Resource {
public:
	static const std::string FILE_EXTENSION;
	static const uint32_t IDENTIFIER_TAG;
private:
    std::vector<PYRIcon> icons;

    Utilities::ImageData image;
public:
    PYRResource();
    PYRResource( const PYRResource &obj );

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    virtual bool parse( const Utilities::Buffer &header, const Utilities::Buffer &data, const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    virtual int write( const char *const file_path, const std::vector<std::string> & arguments ) const;

    static std::vector<PYRResource*> getVector( IFF &mission_file );
    static const std::vector<PYRResource*> getVector( const IFF &mission_file );
};

}

}

#endif // MISSION_RESOURCE_PLY_HEADER
