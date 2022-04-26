#include "PortableNetworkGraphics.h"

#include <libpng16/png.h>
#include <zlib.h>

const std::string Utilities::ImageFormat::PortableNetworkGraphics::FILE_EXTENSION = "png";

Utilities::ImageFormat::PortableNetworkGraphics::PortableNetworkGraphics() {}

Utilities::ImageFormat::PortableNetworkGraphics::~PortableNetworkGraphics() {}

bool Utilities::ImageFormat::PortableNetworkGraphics::canRead() const {
    return false;
}

bool Utilities::ImageFormat::PortableNetworkGraphics::canWrite() const {
    return true;
}

std::string Utilities::ImageFormat::PortableNetworkGraphics::getExtension() const {
    return FILE_EXTENSION;
}

int Utilities::ImageFormat::PortableNetworkGraphics::write( const ImageData& image_data, Buffer& buffer ) {
    // Thanks for NonStatic
    png_image imageWrite;
    memset( &imageWrite, 0, sizeof(imageWrite) );
    imageWrite.version = PNG_IMAGE_VERSION;
    imageWrite.format = PNG_FORMAT_RGB;
    imageWrite.height = image_data.getHeight();
    imageWrite.width  = image_data.getWidth();
    
    const auto FORBIDDIN_PNG_FORMAT_TYPE = PNG_FORMAT_AG;

    if( image_data.getBytesPerChannel() == 1 ) {
        switch( image_data.getType() ) {
        case ImageData::BLACK_WHITE:
            imageWrite.format = PNG_FORMAT_GRAY;
            break;
        case ImageData::BLACK_WHITE_ALHPA:
            imageWrite.format = PNG_FORMAT_GA;
            break;
        case ImageData::RED_GREEN_BLUE:
            imageWrite.format = PNG_FORMAT_RGB;
            break;
        case ImageData::RED_GREEN_BLUE_ALHPA:
            imageWrite.format = PNG_FORMAT_RGBA;
            break;
        default:
            imageWrite.format = FORBIDDIN_PNG_FORMAT_TYPE;
        }
    }
    else if( image_data.getBytesPerChannel() == 2 ) {
        switch( image_data.getType() ) {
        case ImageData::BLACK_WHITE:
            imageWrite.format = PNG_FORMAT_LINEAR_Y;
            break;
        case ImageData::BLACK_WHITE_ALHPA:
            imageWrite.format = PNG_FORMAT_LINEAR_Y_ALPHA;
            break;
        case ImageData::RED_GREEN_BLUE:
            imageWrite.format = PNG_FORMAT_LINEAR_RGB;
            break;
        case ImageData::RED_GREEN_BLUE_ALHPA:
            imageWrite.format = PNG_FORMAT_LINEAR_RGB_ALPHA;
            break;
        default:
            imageWrite.format = FORBIDDIN_PNG_FORMAT_TYPE;
        }
    }
    else
        imageWrite.format = FORBIDDIN_PNG_FORMAT_TYPE;
    
    if( imageWrite.format == FORBIDDIN_PNG_FORMAT_TYPE )
        return -1; // The format is invalid for writing.
    else {
        png_alloc_size_t length = 0;
        
        if( !png_image_write_to_memory(&imageWrite, nullptr, &length, 0, (void*)image_data.getRawImageData(), 0, nullptr) )
            return -2; // Failed to obtain data needed to write the PNG.
        else if( length == 0 )
            return -3; // There is no length to allocate for the buffer.
        else {
            // Now, attempt to allocate the buffer.
            if( !buffer.allocate( length ) )
                return -4;
            else {
                // The buffer is finally allocated. Now, attempt to load the PNG.
                if( png_image_write_to_memory(&imageWrite, (void*)buffer.dangerousPointer(), &length, 0, (void*)image_data.getRawImageData(), 0, nullptr) )
                    return 1; // The buffer is successfully read.
                else
                    return -5; // The image has failed to be written.
            }
        }
    }
}

