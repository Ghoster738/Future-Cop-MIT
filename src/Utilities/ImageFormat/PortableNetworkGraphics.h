#ifndef PORTABLE_NETWORK_GRAPHICS_HEADER
#define PORTABLE_NETWORK_GRAPHICS_HEADER

#include "ImageFormat.h"

namespace Utilities {

namespace ImageFormat {

/**
 * This class only encodes a lossless format.
 *
 * This class uses a PNG library in order to do this task.
 */
class PortableNetworkGraphics : public ImageFormat {
public:
    const static std::filesystem::path FILE_EXTENSION;
    
public:
    PortableNetworkGraphics();
    ~PortableNetworkGraphics();
    
    virtual ImageFormat* duplicate() const;
    virtual bool isFormat( const Buffer& buffer ) const;
    virtual bool canRead() const;
    virtual bool canWrite() const;
    virtual size_t getSpace( const ImageBase2D<Grid2DPlacementNormal>& image_data ) const;
    
    virtual bool supports( const PixelFormatColor& pixel_format ) const;
    
    virtual std::filesystem::path getExtension() const;
    
    int write( const ImageBase2D<Grid2DPlacementNormal>& image_data, Buffer& buffer );
};

};

};

#endif // PORTABLE_NETWORK_GRAPHICS_HEADER

