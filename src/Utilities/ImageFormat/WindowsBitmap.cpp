#include "WindowsBitmap.h"

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
    const size_t HEADER_STRUCT_SIZE =  0xE;
    // const size_t   HEADER_STRUCT_SIZE = 0x28; // Not needed

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
    return true; // By default this program can load windows bmp files.
}

bool WindowsBitmap::canWrite() const {
    return true; // By default this program can write windows bmp files.
}

}

}
