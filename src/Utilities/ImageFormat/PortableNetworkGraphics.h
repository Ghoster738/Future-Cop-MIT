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
    const static std::string FILE_EXTENSION;
    
public:
    PortableNetworkGraphics();
    ~PortableNetworkGraphics();
    
    virtual ImageFormat* duplicate() const;
    virtual bool isFormat( const Buffer& buffer ) const;
    virtual bool canRead() const;
    virtual bool canWrite() const;
    virtual size_t getSpace( const ImageData& image_data ) const;
    
    virtual bool supports( ImageData::Type type, unsigned int bytes_per_channel ) const;
    
    virtual std::string getExtension() const;
    
    int write( const ImageData& image_data, Buffer& buffer );
};

};

};

#endif // PORTABLE_NETWORK_GRAPHICS_HEADER

