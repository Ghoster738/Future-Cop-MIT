#include "PortableNetworkGraphics.h"

#include <cstring>
#include <libpng16/png.h>
#include <zlib.h>

namespace {

png_image setupImage( const Utilities::ImageData& image_data, bool &is_valid ) {
    // Thanks for NonStatic
    png_image image_write;
    memset( &image_write, 0, sizeof(image_write) );
    image_write.version = PNG_IMAGE_VERSION;
    image_write.format = PNG_FORMAT_RGB;
    image_write.height = image_data.getHeight();
    image_write.width  = image_data.getWidth();

    is_valid = true;

    if( image_data.getBytesPerChannel() == 1 ) {
        switch( image_data.getType() ) {
        case Utilities::ImageData::BLACK_WHITE:
            image_write.format = PNG_FORMAT_GRAY;
            break;
        case Utilities::ImageData::BLACK_WHITE_ALHPA:
            image_write.format = PNG_FORMAT_GA;
            break;
        case Utilities::ImageData::RED_GREEN_BLUE:
            image_write.format = PNG_FORMAT_RGB;
            break;
        case Utilities::ImageData::RED_GREEN_BLUE_ALHPA:
            image_write.format = PNG_FORMAT_RGBA;
            break;
        default:
            is_valid = false;
        }
    }
    else if( image_data.getBytesPerChannel() == 2 ) {
        switch( image_data.getType() ) {
        case Utilities::ImageData::BLACK_WHITE:
            image_write.format = PNG_FORMAT_LINEAR_Y;
            break;
        case Utilities::ImageData::BLACK_WHITE_ALHPA:
            image_write.format = PNG_FORMAT_LINEAR_Y_ALPHA;
            break;
        case Utilities::ImageData::RED_GREEN_BLUE:
            image_write.format = PNG_FORMAT_LINEAR_RGB;
            break;
        case Utilities::ImageData::RED_GREEN_BLUE_ALHPA:
            image_write.format = PNG_FORMAT_LINEAR_RGB_ALPHA;
            break;
        default:
            is_valid = false;
        }
    }
    else
        is_valid = false;
    
    return image_write;
}
}

const std::string Utilities::ImageFormat::PortableNetworkGraphics::FILE_EXTENSION = "png";

Utilities::ImageFormat::PortableNetworkGraphics::PortableNetworkGraphics() {}

Utilities::ImageFormat::PortableNetworkGraphics::~PortableNetworkGraphics() {}

Utilities::ImageFormat::ImageFormat* Utilities::ImageFormat::PortableNetworkGraphics::duplicate() const {
    return new PortableNetworkGraphics();
}

bool Utilities::ImageFormat::PortableNetworkGraphics::canRead() const {
    return false;
}

bool Utilities::ImageFormat::PortableNetworkGraphics::canWrite() const {
    return true;
}

bool Utilities::ImageFormat::PortableNetworkGraphics::supports(
     ImageData::Type type,
     unsigned int bytes_per_channel ) const {
    
    if( bytes_per_channel != 1 && bytes_per_channel != 2 )
        return false;
    
    switch( type ) {
        case ImageData::Type::BLACK_WHITE:
        case ImageData::Type::BLACK_WHITE_ALHPA:
        case ImageData::Type::RED_GREEN_BLUE:
        case ImageData::Type::RED_GREEN_BLUE_ALHPA:
            return true;
        default:
            return false;
    }
}

size_t Utilities::ImageFormat::PortableNetworkGraphics::getSpace( const ImageData& image_data ) const {
    bool is_valid;
    auto image_write = setupImage( image_data, is_valid );
    
    if( !is_valid )
        return 0; // The format is invalid for writing.
    else {
        png_alloc_size_t length = 0;
        
        if( !png_image_write_to_memory(&image_write, nullptr, &length, 0, (void*)image_data.getRawImageData(), 0, nullptr) )
            return 0; // Failed to obtain data needed to write the PNG.
        else
            return static_cast<size_t>( length );
    }
}

std::string Utilities::ImageFormat::PortableNetworkGraphics::getExtension() const {
    return FILE_EXTENSION;
}

bool Utilities::ImageFormat::PortableNetworkGraphics::isFormat( const Buffer& buffer ) const {
    auto reader = buffer.getReader();
    uint32_t FIRST = 0x89504E47; // \211 P N G
    uint32_t LAST  = 0x0D0A1A0A; // \r \n \032 \n
    if( reader.totalSize() < 0x8 )
        return false;
    else
        return (reader.readU32( Buffer::Endian::BIG ) == FIRST) &
               (reader.readU32( Buffer::Endian::BIG ) ==  LAST);
}

int Utilities::ImageFormat::PortableNetworkGraphics::write( const ImageData& image_data, Buffer& buffer ) {
    bool is_valid;
    auto image_write = setupImage( image_data, is_valid );
    
    if( !is_valid )
        return -1; // The format is invalid for writing.
    else {
        png_alloc_size_t length = 0;
        
        if( !png_image_write_to_memory(&image_write, nullptr, &length, 0, (void*)image_data.getRawImageData(), 0, nullptr) )
            return -2; // Failed to obtain data needed to write the PNG.
        else if( length == 0 )
            return -3; // There is no length to allocate for the buffer.
        else {
            // Now, attempt to allocate the buffer.
            if( !buffer.allocate( length ) )
                return -4;
            else {
                // The buffer is finally allocated. Now, attempt to load the PNG.
                if( png_image_write_to_memory(&image_write, (void*)buffer.dangerousPointer(), &length, 0, (void*)image_data.getRawImageData(), 0, nullptr) )
                    return 1; // The buffer is successfully read.
                else
                    return -5; // The image has failed to be written.
            }
        }
    }
}

