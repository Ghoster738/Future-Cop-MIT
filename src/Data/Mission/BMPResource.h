#ifndef MISSION_RESOURCE_BMP_HEADER
#define MISSION_RESOURCE_BMP_HEADER

#include "Resource.h"
#include "../../Utilities/ImageData.h"

namespace Data {

namespace Mission {

class BMPResource : public Resource {
public:
	static const std::string FILE_EXTENSION;
	static const uint32_t IDENTIFIER_TAG;
private:
    // The Windows and Mac versions are strange. Both of the desktop versions still used a color pallete despite using 16 bit color.
    // Note: PS1 Versions do not have lookUpData
    uint8_t lookUpData[ 0x400 ];
    // uint16_t lookUpDataOffsets[ 0x8000 ]; // This is used for actual 15 bit color conversions.

    Utilities::ImageData image_raw; // Holds the image data as is.

    // This holds the color pallete of the texture. It is required for PlayStation 1 version of Future Cop: LAPD
    Utilities::ImageData palette;

    // This holds color data from Windows and Mac. However, this will be empty for PlayStation 1 the textures only seems to use 8-bit data.
    Utilities::ImageData image_from_16_colors;
    // This should hold color data from the palette data for all the versions. However only the PlayStation 1 can write it currently
    Utilities::ImageData image_from_palette;
public:
    BMPResource();
    BMPResource( const BMPResource &obj );

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    virtual bool parse( const Utilities::Buffer &header, const Utilities::Buffer &data, const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    virtual int write( const char *const file_path, const std::vector<std::string> & arguments ) const;

    Utilities::ImageData *const getImage() const;
    Utilities::ImageData *const getRGBImage() const;
    Utilities::ImageData *const getTranslatedImage() const;

    static std::vector<BMPResource*> getVector( IFF &mission_file );
    static const std::vector<BMPResource*> getVector( const IFF &mission_file );
};

}

}

#endif // MISSION_RESOURCE_BMP_HEADER
