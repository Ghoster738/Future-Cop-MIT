#ifndef PORTABLE_NETWORK_GRAPHICS_HEADER
#define PORTABLE_NETWORK_GRAPHICS_HEADER

#include <vector>
#include <stdint.h>

#include "../Buffer.h"
#include "../ImageData.h"

namespace Utilities {

namespace ImageFormat {

/**
 * This class only encodes a lossless format.
 *
 * This class uses a PNG library in order to do this task.
 */
class PortableNetworkGraphics {
public:
    PortableNetworkGraphics() {}
    ~PortableNetworkGraphics() {}
    
    static bool isSupported();
    
    int write( const ImageData& image_data, Buffer& buffer );
};

};

};

#endif // PORTABLE_NETWORK_GRAPHICS_HEADER

