#ifndef IMAGE_FORMAT_HEADER
#define IMAGE_FORMAT_HEADER

#include <vector>
#include <string>
#include <stdint.h>

#include "../Buffer.h"
#include "../Image2D.h"

namespace Utilities {

namespace ImageFormat {

/**
 * This class is an interface class for the ImageFormat.
 *
 * This class is used to switch between image formats.
 */
class ImageFormat {
public:
    ImageFormat();
    virtual ~ImageFormat();
    
    bool isSupported() const;
    virtual ImageFormat* duplicate() const = 0;
    virtual bool isFormat( const Buffer& buffer ) const = 0;
    virtual bool canRead() const = 0;
    virtual bool canWrite() const = 0;
    virtual size_t getSpace( const ImageBase2D<Grid2DPlacementNormal>& image_data ) const = 0;
    virtual bool supports( const PixelFormatColor& pixel_format ) const = 0;
    
    virtual std::string getExtension() const = 0;
    
    virtual std::string appendExtension( const std::string &name ) const;
    
    virtual int write( const ImageBase2D<Grid2DPlacementNormal>& image_data, Buffer& buffer );
    virtual int read( const Buffer& buffer, ImageColor2D<Grid2DPlacementNormal>& image_data );
};

};

};

#endif // IMAGE_FORMAT_HEADER

