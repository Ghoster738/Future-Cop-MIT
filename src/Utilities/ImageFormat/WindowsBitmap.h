#ifndef WINDOWS_BITMAP_HEADER
#define WINDOWS_BITMAP_HEADER

#include "ImageFormat.h"

namespace Utilities {

namespace ImageFormat {

/**
 * This class is to both encode and decode a Windows BMP image format.
 */
class WindowsBitmap : public ImageFormat {
public:
    const static std::filesystem::path FILE_EXTENSION;
    
public:
    WindowsBitmap();
    ~WindowsBitmap();
    
    virtual ImageFormat* duplicate() const;
    
    virtual bool isFormat( const Buffer& buffer ) const;
    virtual bool canRead() const;
    virtual bool canWrite() const;
    virtual size_t getSpace( const ImageBase2D<Grid2DPlacementNormal>& image_data ) const;
    
    virtual bool supports( const PixelFormatColor& pixel_format ) const;
    
    virtual std::filesystem::path getExtension() const;
    
    int write( const ImageBase2D<Grid2DPlacementNormal>& image_data, Buffer& buffer );
    int read( const Buffer& buffer, ImageColor2D<Grid2DPlacementNormal>& image_data );
};

};

};

#endif // WINDOWS_BITMAP_HEADER

