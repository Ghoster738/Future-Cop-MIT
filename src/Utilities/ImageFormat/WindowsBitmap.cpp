#include "WindowsBitmap.h"

namespace {

const size_t   INFO_STRUCT_SIZE =  0xE;
const size_t HEADER_STRUCT_SIZE = 0x28;

}

namespace Utilities {

namespace ImageFormat {

const std::string WindowsBitmap::FILE_EXTENSION = "bmp";

// 16 bit color for bitmap urrrrrgggggbbbbb
// Low Blue   = 0x0001
// High Blue  = 0x001F
// Low Green  = 0x0020
// High Green = 0x03E0
// Low Red    = 0x0400
// High Red   = 0x7C00

WindowsBitmap::WindowsBitmap() {}

WindowsBitmap::~WindowsBitmap() {}

ImageFormat* WindowsBitmap::duplicate() const {
    return new WindowsBitmap();
}

bool WindowsBitmap::isFormat( const Buffer& buffer ) const {
    auto reader = buffer.getReader();

    // Check the header
    if(reader.totalSize() <= INFO_STRUCT_SIZE)
        return false;

    if( reader.readI8() != 'B' || reader.readI8() != 'M' )
        return false;

    const auto BMP_SIZE = reader.readU32( Utilities::Buffer::Endian::LITTLE );

    if(BMP_SIZE != reader.totalSize())
        return false;

    // WARNING this does not fully check if the BMP is correct for the parser.

    return true;
}

bool WindowsBitmap::canRead() const {
    return true; // By default this program can load Windows BMP files.
}

bool WindowsBitmap::canWrite() const {
    return true; // By default this program can write Windows BMP files.
}

size_t WindowsBitmap::getSpace( const ImageBase2D<Grid2DPlacementNormal>& image_data ) const {
    size_t current_size = 0;

    if( supports( *image_data.getPixelFormat() ) ) {
        current_size += INFO_STRUCT_SIZE + HEADER_STRUCT_SIZE;

        const size_t BIT_AMOUNT = 8 * image_data.getPixelFormat()->byteSize();

        const size_t ROW_SIZE = 4 * ((BIT_AMOUNT * image_data.getWidth() + 31) / 32);

        current_size += ROW_SIZE * image_data.getHeight();
    }

    return current_size;
}

bool WindowsBitmap::supports( const PixelFormatColor& pixel_format ) const {
    if( dynamic_cast<const Utilities::PixelFormatColor_R5G5B5A1*>( &pixel_format ) != nullptr )
        return true;
    else
    if( dynamic_cast<const Utilities::PixelFormatColor_R8G8B8*>( &pixel_format ) != nullptr )
        return true;
    else
    if( dynamic_cast<const Utilities::PixelFormatColor_R8G8B8A8*>( &pixel_format ) != nullptr )
        return true;
    else
        return false;
}

std::string WindowsBitmap::getExtension() const {
    return FILE_EXTENSION;
}

}

}
