#ifndef IMAGE_FORMAT_HEADER
#define IMAGE_FORMAT_HEADER

#include <vector>
#include <stdint.h>

#include "../Buffer.h"
#include "../ImageData.h"

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
    
    static bool isSupported() { return false; }
    
    virtual int write( const ImageData& image_data, Buffer& buffer );
    virtual int read( const Buffer& buffer, ImageData& image_data );
};

};

};

#endif // IMAGE_FORMAT_HEADER

