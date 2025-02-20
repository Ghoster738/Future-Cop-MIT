#include "WindowsBitmap.h"

namespace {

const size_t      INFO_STRUCT_SIZE =  0xE;
const size_t    HEADER_STRUCT_SIZE = 0x28;
const size_t HEADER_32_STRUCT_SIZE = 0x6C;

const uint32_t MASK[4] = {
    0x000000FF,
    0x0000FF00,
    0x00FF0000,
    0xFF000000 };

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
        current_size += INFO_STRUCT_SIZE;

        if( dynamic_cast<const Utilities::PixelFormatColor_R8G8B8A8*>( image_data.getPixelFormat() ) != nullptr )
            current_size += HEADER_STRUCT_SIZE;
        else
            current_size += HEADER_32_STRUCT_SIZE;

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

    size_t header_size;

    if( dynamic_cast<const Utilities::PixelFormatColor_R8G8B8A8*>( image_data.getPixelFormat() ) != nullptr )
        header_size = HEADER_STRUCT_SIZE;
    else
        header_size = HEADER_32_STRUCT_SIZE;

    size_t offset_size = INFO_STRUCT_SIZE + header_size;

    // Write the header
    buffer.addI8( 'B' );
    buffer.addI8( 'M' );
    buffer.addU32(        SIZE, Buffer::Endian::LITTLE );
    buffer.addU16(           0, Buffer::Endian::LITTLE );
    buffer.addU16(           0, Buffer::Endian::LITTLE );
    buffer.addU32( offset_size, Buffer::Endian::LITTLE ); // This is where the pixels start.

    // Write the info struct.
    buffer.addU32(            header_size, Buffer::Endian::LITTLE );
    buffer.addI32(  image_data.getWidth(), Buffer::Endian::LITTLE );
    buffer.addI32( image_data.getHeight(), Buffer::Endian::LITTLE );
    buffer.addU16(                      1, Buffer::Endian::LITTLE ); // Only one color plane please.
    buffer.addU16(             BIT_AMOUNT, Buffer::Endian::LITTLE );
    buffer.addU32(                      0, Buffer::Endian::LITTLE ); // Uncompressed
    buffer.addU32(     SIZE - offset_size, Buffer::Endian::LITTLE ); // Raw image size can be optained through this method.
    buffer.addI32(                    512, Buffer::Endian::LITTLE ); // horizontal 512 pixels per metre
    buffer.addI32(                    512, Buffer::Endian::LITTLE ); //   vertical 512 pixels per metre
    buffer.addU32(                      0, Buffer::Endian::LITTLE ); // No color palette
    buffer.addU32(                      0, Buffer::Endian::LITTLE ); // No color palette

    if(header_size == HEADER_32_STRUCT_SIZE) {
        buffer.addU32( 0x00FF0000, Buffer::Endian::LITTLE ); // red   channel
        buffer.addU32( 0x0000FF00, Buffer::Endian::LITTLE ); // green channel
        buffer.addU32( 0x000000FF, Buffer::Endian::LITTLE ); // blue  channel
        buffer.addU32( 0xFF000000, Buffer::Endian::LITTLE ); // alpha channel
        buffer.addU32( 0x42475273, Buffer::Endian::BIG );

        for(unsigned i = 0; i < 10; i++) {
            buffer.addU32( 0, Buffer::Endian::LITTLE );
        }

        buffer.addU32( 0, Buffer::Endian::LITTLE );
        buffer.addU32( 0, Buffer::Endian::LITTLE );
        buffer.addU32( 0, Buffer::Endian::LITTLE );
    }

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

    // *** Check the header structure ***
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

    // *** Check the info structure ***
    const auto BMP_INFO_STRUCT = reader.readU32( Utilities::Buffer::Endian::LITTLE );

    if(BMP_INFO_STRUCT != HEADER_STRUCT_SIZE && BMP_INFO_STRUCT != HEADER_32_STRUCT_SIZE)
        return -4;

    const auto width       = reader.readI32(Buffer::Endian::LITTLE);
    const auto height      = reader.readI32(Buffer::Endian::LITTLE);
    const auto color_plane = reader.readU16(Buffer::Endian::LITTLE);
    const auto bit_amount  = reader.readU16(Buffer::Endian::LITTLE);
    const auto compression = reader.readU32(Buffer::Endian::LITTLE);
    const auto image_size  = reader.readU32(Buffer::Endian::LITTLE);
    reader.readU32(Buffer::Endian::LITTLE);
    reader.readU32(Buffer::Endian::LITTLE);
    const auto color_palette = reader.readU32(Buffer::Endian::LITTLE);
    const auto important_color_amount = reader.readU32(Buffer::Endian::LITTLE);

    // Only one color plane.
    if(color_plane != 1)
        return -5;

    // No compression.
    if(compression != 0)
        return -6;

    // Color palettes not supported.
    if(color_palette != 0)
        return -7;

    if(important_color_amount != 0)
        return -8;

    Buffer::Reader pixel_buffer = buffer.getReader(BMP_IMAGE_DATA_OFFSET, image_size);
    const size_t ROW_SIZE = 4 * ((bit_amount * image_data.getWidth() + 31) / 32);
    PixelFormatColor::GenericColor m_color;

    if(BMP_INFO_STRUCT != HEADER_32_STRUCT_SIZE) {
        // *** Small Header Case ***

        m_color.alpha = 1.0;

        if(bit_amount == 16) {
            uint16_t bmp_sample;

            image_data.setDimensions( width, height );

            for( size_t y = 0; y < image_data.getHeight(); y++ ) {
                Buffer::Reader pixel_row_buffer = buffer.getReader(y * ROW_SIZE, ROW_SIZE);

                for( size_t x = 0; x < image_data.getWidth(); x++ ) {
                    bmp_sample = pixel_row_buffer.readU16(Buffer::Endian::LITTLE);

                    m_color.blue  = static_cast<float>( (bmp_sample & 0x001F) >>  0 ) / 32.0;
                    m_color.green = static_cast<float>( (bmp_sample & 0x03E0) >>  5 ) / 32.0;
                    m_color.red   = static_cast<float>( (bmp_sample & 0x7C00) >> 10 ) / 32.0;

                    image_data.writePixel( x, y, m_color );
                }
            }
            return 1;

        } else if(bit_amount == 24) {
            image_data.setDimensions( width, height );

            for( size_t y = 0; y < image_data.getHeight(); y++ ) {
                Buffer::Reader pixel_row_buffer = buffer.getReader(y * ROW_SIZE, ROW_SIZE);

                for( size_t x = 0; x < image_data.getWidth(); x++ ) {

                    m_color.blue  = static_cast<float>( pixel_row_buffer.readU8() ) / 256.0;
                    m_color.green = static_cast<float>( pixel_row_buffer.readU8() ) / 256.0;
                    m_color.red   = static_cast<float>( pixel_row_buffer.readU8() ) / 256.0;

                    image_data.writePixel( x, y, m_color );
                }
            }
            return 1;

        } else
            return -9; // Restrict supported bit depths.
    }
    else {
        // *** Big Header Case ***

        // Restrict supported bit depths.
        if(bit_amount != 32) {
            return -10;
        }

        const unsigned   red_index = 0;
        const unsigned green_index = 1;
        const unsigned  blue_index = 2;
        const unsigned alpha_index = 3;

        uint32_t channels[4];

        channels[  red_index] = reader.readU32(Buffer::Endian::LITTLE);
        channels[green_index] = reader.readU32(Buffer::Endian::LITTLE);
        channels[ blue_index] = reader.readU32(Buffer::Endian::LITTLE);
        channels[alpha_index] = reader.readU32(Buffer::Endian::LITTLE);

        for(size_t i = 0; i < 4; i++) {
            if(MASK[0])
                channels[i] = 0;
            else if(MASK[1])
                channels[i] = 1;
            else if(MASK[2])
                channels[i] = 2;
            else if(MASK[3])
                channels[i] = 3;
            else
                return -11; // Unsupported bitfield
        }

        image_data.setDimensions( width, height );

        uint8_t samples[4];

        for( size_t y = 0; y < image_data.getHeight(); y++ ) {
            Buffer::Reader pixel_row_buffer = buffer.getReader(y * ROW_SIZE, ROW_SIZE);

            for( size_t x = 0; x < image_data.getWidth(); x++ ) {

                samples[0] = pixel_row_buffer.readU8();
                samples[1] = pixel_row_buffer.readU8();
                samples[2] = pixel_row_buffer.readU8();
                samples[3] = pixel_row_buffer.readU8();

                m_color.red   = static_cast<float>( samples[channels[  red_index]] ) / 256.0;
                m_color.green = static_cast<float>( samples[channels[green_index]] ) / 256.0;
                m_color.blue  = static_cast<float>( samples[channels[ blue_index]] ) / 256.0;
                m_color.alpha = static_cast<float>( samples[channels[alpha_index]] ) / 256.0;

                image_data.writePixel( x, y, m_color );
            }
        }
        return 1;
    }
}

}

}
