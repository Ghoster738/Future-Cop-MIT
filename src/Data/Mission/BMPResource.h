#ifndef MISSION_RESOURCE_BMP_HEADER
#define MISSION_RESOURCE_BMP_HEADER

#include "Resource.h"
#include "../../Utilities/Image2D.h"
#include "../../Utilities/ImagePalette2D.h"
#include "../../Utilities/ImageFormat/ImageFormat.h"

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

    // This holds the full color version of the texture. It is optional for Macintosh and Windows.
    Utilities::Image2D *image_p;

    // This holds the color palette version of the texture. It is required for PlayStation 1 version of Future Cop: LAPD
    // TODO Deciper how the palette system works on the Windows and Mac versions.
    Utilities::ImagePalette2D *image_palette_p;
    
    bool isPSX; // True if this CBMP file is from PlayStation.
    
    Utilities::ImageFormat::ImageFormat *format_p;
public:
    BMPResource();
    BMPResource( const BMPResource &obj );
    virtual ~BMPResource();

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    virtual int write( const char *const file_path, const std::vector<std::string> & arguments ) const;

    const Utilities::ImageFormat::ImageFormat *const getImageFormat() const;

    Utilities::Image2D *const getImage() const;
    Utilities::ImagePalette2D *const getPaletteImage() const;

    static std::vector<BMPResource*> getVector( IFF &mission_file );
    static const std::vector<BMPResource*> getVector( const IFF &mission_file );
};

}

}

#endif // MISSION_RESOURCE_BMP_HEADER
