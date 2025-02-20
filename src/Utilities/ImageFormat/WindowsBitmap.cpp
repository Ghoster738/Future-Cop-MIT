#include "WindowsBitmap.h"

namespace {

const size_t   INFO_STRUCT_SIZE =  0xE;
const size_t HEADER_STRUCT_SIZE = 0x28;
const size_t        OFFSET_SIZE = INFO_STRUCT_SIZE + HEADER_STRUCT_SIZE;

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
    Buffer::Reader reader = buffer.getReader();

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

int WindowsBitmap::write( const ImageBase2D<Grid2DPlacementNormal>& image_data, Buffer& buffer ) {
    const size_t SIZE = getSpace(image_data);
    const size_t BIT_AMOUNT = 8 * image_data.getPixelFormat()->byteSize();
    const size_t ROW_SIZE = 4 * ((BIT_AMOUNT * image_data.getWidth() + 31) / 32);
    const size_t ROW_PAD_SIZE = ROW_SIZE - 4 * ((BIT_AMOUNT * image_data.getWidth()) / 32);

    // getSpace will return zero if the image_data's format is not supported.
    if(SIZE == 0)
        return -1;

    // Write the header
    buffer.addI8( 'B' );
    buffer.addI8( 'M' );
    buffer.addU32(        SIZE, Buffer::Endian::LITTLE );
    buffer.addU16(           0, Buffer::Endian::LITTLE );
    buffer.addU16(           0, Buffer::Endian::LITTLE );
    buffer.addU32( OFFSET_SIZE, Buffer::Endian::LITTLE ); // This is where the pixels start.

    // Write the info struct.
    buffer.addU32(                     40, Buffer::Endian::LITTLE );
    buffer.addI32(  image_data.getWidth(), Buffer::Endian::LITTLE );
    buffer.addI32( image_data.getHeight(), Buffer::Endian::LITTLE );
    buffer.addU16(                      1, Buffer::Endian::LITTLE ); // Only one color plane please.
    buffer.addU16(             BIT_AMOUNT, Buffer::Endian::LITTLE );
    buffer.addU32(                      0, Buffer::Endian::LITTLE ); // Uncompressed
    buffer.addU32(     SIZE - OFFSET_SIZE, Buffer::Endian::LITTLE ); // Raw image size can be optained through this method.
    buffer.addI32(                    512, Buffer::Endian::LITTLE ); // horizontal 512 pixels per metre
    buffer.addI32(                    512, Buffer::Endian::LITTLE ); //   vertical 512 pixels per metre
    buffer.addU32(                      0, Buffer::Endian::LITTLE ); // No color palette
    buffer.addU32(                      0, Buffer::Endian::LITTLE ); // No color palette

    if( dynamic_cast<const Utilities::PixelFormatColor_R5G5B5A1*>( image_data.getPixelFormat() ) != nullptr ) {
        uint16_t color;

        for( size_t y = 0; y < image_data.getHeight(); y++ ) {
            for( size_t x = 0; x < image_data.getWidth(); x++ ) {
                auto generic_color = image_data.readPixel( x, y );

                color = 0;

                color |= static_cast<uint16_t>(std::min( generic_color.blue  * 32.0, 31.)) <<  0;
                color |= static_cast<uint16_t>(std::min( generic_color.green * 32.0, 31.)) <<  5;
                color |= static_cast<uint16_t>(std::min( generic_color.red   * 32.0, 31.)) << 15;

                buffer.addU16(color, Buffer::Endian::LITTLE);
            }

            for( size_t p = 0; p < ROW_PAD_SIZE; p++ )
                buffer.addU8(0); // Add padding.
        }
    }
    else
    if( dynamic_cast<const Utilities::PixelFormatColor_R8G8B8*>( image_data.getPixelFormat() ) != nullptr ) {
        for( size_t y = 0; y < image_data.getHeight(); y++ ) {
            for( size_t x = 0; x < image_data.getWidth(); x++ ) {
                auto generic_color = image_data.readPixel( x, y );

                buffer.addU8(std::min( generic_color.blue  * 256.0, 255.));
                buffer.addU8(std::min( generic_color.green * 256.0, 255.));
                buffer.addU8(std::min( generic_color.red   * 256.0, 255.));
            }

            for( size_t p = 0; p < ROW_PAD_SIZE; p++ )
                buffer.addU8(0); // Add padding.
        }
    }
    else
    if( dynamic_cast<const Utilities::PixelFormatColor_R8G8B8A8*>( image_data.getPixelFormat() ) != nullptr ) {
        for( size_t y = 0; y < image_data.getHeight(); y++ ) {
            for( size_t x = 0; x < image_data.getWidth(); x++ ) {
                auto generic_color = image_data.readPixel( x, y );

                buffer.addU8(std::min( generic_color.blue  * 256.0, 255.));
                buffer.addU8(std::min( generic_color.green * 256.0, 255.));
                buffer.addU8(std::min( generic_color.red   * 256.0, 255.));
                buffer.addU8(std::min( generic_color.alpha * 256.0, 255.));
            }

            for( size_t p = 0; p < ROW_PAD_SIZE; p++ )
                buffer.addU8(0); // Add padding. This might be redundent
        }
    }

    return 1;
}


int WindowsBitmap::read( const Buffer& buffer, ImageColor2D<Grid2DPlacementNormal>& image_data ) {
    Buffer::Reader reader = buffer.getReader();

    // Check the header
    if(reader.totalSize() <= INFO_STRUCT_SIZE)
        return -1;

    if( reader.readI8() != 'B' || reader.readI8() != 'M' )
        return -2;

    const auto BMP_SIZE = reader.readU32( Utilities::Buffer::Endian::LITTLE );

    if(BMP_SIZE != reader.totalSize())
        return -3;

    reader.readU16(Buffer::Endian::LITTLE);
    reader.readU16(Buffer::Endian::LITTLE);

    const auto BMP_IMAGE_DATA_OFFSET = reader.readU32( Utilities::Buffer::Endian::LITTLE );

    // if(HEADER_STRUCT_SIZE)
}

}

}
