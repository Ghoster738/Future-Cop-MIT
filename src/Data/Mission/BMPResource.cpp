#include "BMPResource.h"

#include <cmath>
#include "../../Utilities/DataHandler.h"
#include <fstream>
#include <cassert>

namespace {
// Both versions have this for this is the header for the texture.
const uint32_t CCB_TAG = 0x43434220; // which is { 0x43, 0x43, 0x42, 0x20 } or { 'C', 'C', 'B', ' ' } or "CCB "
// This is probably something for converting 16 bit data to 8 bit pallete data for Windows and Mac.
const uint32_t LKUP_TAG = 0x4C6B5570; // which is { 0x4C, 0x75, 0x55, 0x70 } or { 'L', 'k', 'U', 'p' } or "LkUp"
// Windows and Mac has the full 16 bit colors.
const uint32_t PX16_TAG = 0x50583136; // which is { 0x50, 0x58, 0x31, 0x36 } or { 'P', 'X', '1', '6' } or "PX16"
// Color pallete both used in Playstation 1 and the desktop versions.
const uint32_t PLUT_TAG = 0x504C5554; // which is { 0x50, 0x4C, 0x55, 0x54 } or { 'P', 'L', 'U', 'T' } or "PLUT"
// This is the pixel data for the Playstation 1 version, and probably the computer versions.
const uint32_t PDAT_TAG = 0x50444154; // which is { 0x50, 0x44, 0x41, 0x54 } or { 'P', 'D', 'A', 'T' } or "PDAT"
}

const std::string Data::Mission::BMPResource::FILE_EXTENSION = "cbmp";
const uint32_t Data::Mission::BMPResource::IDENTIFIER_TAG = 0x43626D70; // which is { 0x43, 0x62, 0x6D, 0x70 } or { 'C', 'b', 'm', 'p' } or "Cbmp"

Data::Mission::BMPResource::BMPResource() {

}

Data::Mission::BMPResource::BMPResource( const BMPResource &obj ) : Resource( obj ), palette( obj.palette ), image_from_16_colors( obj.image_from_16_colors ), image_from_palette( obj.image_from_palette ) {
}

std::string Data::Mission::BMPResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::BMPResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool Data::Mission::BMPResource::parse( const ParseSettings &settings ) {
    bool file_is_not_valid = false;
    
    if( this->data_p != nullptr )
    {
        auto reader = this->data_p->getReader();

        while( reader.getPosition() < reader.totalSize() ) {
            auto identifier = reader.readU32( settings.endian );
            auto tag_size   = reader.readU32( settings.endian );

            if( identifier == CCB_TAG ) {
                auto cbb_reader = reader.getReader( tag_size - sizeof( uint32_t ) * 2 );
                // TODO This probably handles the transparency, and maybe other effects.
                
                // Zero data 0x0-0xC.
                // 0x4
                assert( cbb_reader.readU32( settings.endian ) == 0 );
                // 0x8
                assert( cbb_reader.readU32( settings.endian ) == 0 );
                // 0xC = 12
                assert( cbb_reader.readU32( settings.endian ) == 0 );
                
                // Address 0x10 is always 0x01000000
                assert( cbb_reader.readU32( settings.endian ) == 0x01000000 );
                
                assert( cbb_reader.readU32( settings.endian ) == 0 );
                
                assert( cbb_reader.readU32( settings.endian ) == 0x01000000 );
                
                assert( cbb_reader.readU32( settings.endian ) == 0x01000000 );
                
                assert( cbb_reader.readU32( settings.endian ) == 0 );
                
                assert( cbb_reader.readU32( settings.endian ) == 0x01000000 );
                
                assert( cbb_reader.readU32( settings.endian ) == 8 );
                
                assert( cbb_reader.readU16( settings.endian ) == 0 );
                
                cbb_reader.setPosition( 0x2A, Utilities::Buffer::Reader::BEGINING );
                auto bitfield_byte = cbb_reader.readU8();
                
                assert( cbb_reader.readU8() == 0 );
                
                assert( cbb_reader.readU32( settings.endian ) == 0 );
                
                assert( cbb_reader.readU32( settings.endian ) == 0 );
                
                cbb_reader.setPosition( 0x34, Utilities::Buffer::Reader::BEGINING );
                auto first_u32 = cbb_reader.readU32( settings.endian );
                
                uint8_t array[4];
                
                array[ 0 ] = cbb_reader.readU8();
                array[ 1 ] = cbb_reader.readU8();
                array[ 2 ] = cbb_reader.readU8();
                array[ 3 ] = cbb_reader.readU8();
                
                auto second_u32 = cbb_reader.readU32( settings.endian );
                auto third_u32 = cbb_reader.readU32( settings.endian );
            }
            else
            if( identifier == LKUP_TAG ) {
                auto lkup_reader = reader.getReader( tag_size - sizeof( uint32_t ) * 2 );

                const size_t LOOKUP_DATA_AMOUNT = sizeof( lookUpData ) / sizeof( lookUpData[0] );

                // This seems to be all 8 bit data in 4 lists each containing 0x100 bytes.
                // In second thought
                // I could be that this is simply a big list of 0x400 unsigned bytes and the 16-bit data is simply going to scale down.
                for( unsigned int i = 0; i < LOOKUP_DATA_AMOUNT; i++ )
                    lookUpData[ i ] = lkup_reader.readU8();
            }
            else
            if( identifier == PDAT_TAG ) { // For PlayStation
                auto px_reader = reader.getReader( tag_size - sizeof( uint32_t ) * 2 );

                // setup the image
                image_raw.setWidth( 0x100 );
                image_raw.setHeight( 0x100 );
                image_raw.setFormat( Utilities::ImageData::BLACK_WHITE, 1 );

                auto image_data_8_bit = image_raw.getRawImageData();
                for( unsigned int a = 0; a < image_raw.getWidth() * image_raw.getHeight(); a++ ) {

                    *reinterpret_cast<uint8_t*>(image_data_8_bit) = px_reader.readU8();

                    image_data_8_bit += image_raw.getPixelSize();
                }
            }
             else
             if( identifier == PX16_TAG ) { // For Windows
                 auto px16_reader = reader.getReader( tag_size - sizeof( uint32_t ) * 2 );

                // setup the image
                image_raw.setWidth( 0x100 );
                image_raw.setHeight( 0x100 );
                image_raw.setFormat( Utilities::ImageData::BLACK_WHITE, 2 );

                auto image_data_16bit = image_raw.getRawImageData();
                for( unsigned int a = 0; a < image_raw.getWidth() * image_raw.getHeight(); a++ ) {
                    *reinterpret_cast<uint16_t*>(image_data_16bit) = px16_reader.readU16( settings.endian );

                    image_data_16bit += image_raw.getPixelSize();
                }
            }
            else
            if( identifier == PLUT_TAG) {
                auto plut_reader = reader.getReader( tag_size - sizeof( uint32_t ) * 2 );

                // The color pallette is located 12 bytes away from the start of the tag.
                plut_reader.setPosition( 0xC, Utilities::Buffer::Reader::CURRENT );

                // Now store the color palette.
                palette.setWidth( 1 );
                palette.setHeight( 0x100 );
                palette.setFormat( Utilities::ImageData::RED_GREEN_BLUE, 1 );

                auto palette_data = palette.getRawImageData();
                uint8_t red, green, blue;

                for( unsigned int d = 0; d < palette.getHeight(); d++ ) {
                    Utilities::ImageData::translate_16_to_24( plut_reader.readU16( settings.endian ), blue, green, red );

                    palette_data[0] = red;
                    palette_data[1] = green;
                    palette_data[2] = blue;

                    palette_data += palette.getPixelSize();
                }
            }
            else
            {
                reader.setPosition( tag_size - sizeof( uint32_t ) * 2, Utilities::Buffer::Reader::CURRENT );
            }
        }

        if( !file_is_not_valid ) { // If file is valid.

            auto image_raw_data = image_raw.getRawImageData();

            if( image_raw.getPixelSize() == 1 && palette.isValid() ) // (image_raw.getPixelSize() == 1) means that this BMP is a PlayStation image.
            {
                image_from_palette.setWidth(  image_raw.getWidth() );
                image_from_palette.setHeight( image_raw.getHeight() );
                image_from_palette.setFormat( Utilities::ImageData::RED_GREEN_BLUE, 1 );
                auto image_data_24_RGB = image_from_palette.getRawImageData();

                auto palette_data = palette.getRawImageData();

                for( unsigned int d = 0; d < palette.getHeight(); d++ ) {
                    std::swap( palette_data[0], palette_data[2] );

                    palette_data += palette.getPixelSize();
                }
                
                for( unsigned int a = 0; a < image_from_palette.getWidth() * image_from_palette.getHeight(); a++ ) {
                    uint8_t *palette_pixel = reinterpret_cast<uint8_t*>( palette.getRawImageData() + *reinterpret_cast<uint8_t*>(&image_raw_data[0]) * 3 );

                    image_data_24_RGB[0] = palette_pixel[0];
                    image_data_24_RGB[1] = palette_pixel[1];
                    image_data_24_RGB[2] = palette_pixel[2];

                    image_data_24_RGB += image_from_palette.getPixelSize();
                    image_raw_data += image_raw.getPixelSize();
                }
            }
            else // if it is a Windows or Macintosh texture
            {
                image_from_16_colors.setWidth(  image_raw.getWidth() );
                image_from_16_colors.setHeight( image_raw.getHeight() );
                image_from_16_colors.setFormat( Utilities::ImageData::RED_GREEN_BLUE, 1 );
                auto image_data_24_RGB = image_from_16_colors.getRawImageData();

                uint8_t red, green, blue;

                for( unsigned int a = 0; a < image_from_16_colors.getWidth() * image_from_16_colors.getHeight(); a++ ) {
                    Utilities::ImageData::translate_16_to_24( Utilities::DataHandler::read_u16_little( reinterpret_cast<uint8_t*>(&image_raw_data[0]) ), blue, green, red );

                    image_data_24_RGB[0] = red;
                    image_data_24_RGB[1] = green;
                    image_data_24_RGB[2] = blue;

                    image_data_24_RGB += image_from_16_colors.getPixelSize();
                    image_raw_data += image_raw.getPixelSize();
                }
            }
        }

        return !file_is_not_valid;
    }
    else
        return false;
}

Data::Mission::Resource * Data::Mission::BMPResource::duplicate() const {
    return new Mission::BMPResource( *this );
}
int Data::Mission::BMPResource::write( const char *const file_path, const std::vector<std::string> & arguments ) const {
    std::string file_path_texture = std::string( file_path ) + ".png";
    bool export_enable = true;

    for( auto arg = arguments.begin(); arg != arguments.end(); arg++ ) {
        if( (*arg).compare("--dry") == 0 )
            export_enable = false;
    }

    if( export_enable && getImage() != nullptr)
    {
        unsigned int state = getImage()->write( file_path_texture.c_str() );
        if( image_raw.getPixelSize() == 2 ) {
            std::string file_path_8pal_texture = std::string( file_path ) + "_paletted.png";
            image_from_palette.write( file_path_8pal_texture.c_str() );
        }
        
        return state;
    }
    else
        return 0;
}


Utilities::ImageData *const Data::Mission::BMPResource::getImage() const {
    auto pointer = getRGBImage();

    // If the pure RGB data is not available then we will use the translated pixel data instead.
    if( pointer == nullptr )
        pointer = getTranslatedImage();

    return pointer;
}

Utilities::ImageData *const Data::Mission::BMPResource::getRGBImage() const {
    // If the dimensions of image_from_16_colors does match image_raw's then it actually exists.
    if( image_from_16_colors.getWidth() == image_raw.getWidth() && image_from_16_colors.getHeight() == image_raw.getHeight() )
        return const_cast<Utilities::ImageData *const>( &image_from_16_colors);
    else
        return nullptr;
}

Utilities::ImageData *const Data::Mission::BMPResource::getTranslatedImage() const {
    // If the dimensions of image_from_palette does match image_raw's then it actually exists.
    if( image_from_palette.getWidth() == image_raw.getWidth() && image_from_palette.getHeight() == image_raw.getHeight() )
        return const_cast<Utilities::ImageData *const>( &image_from_palette );
    else
        return nullptr;
}

std::vector<Data::Mission::BMPResource*> Data::Mission::BMPResource::getVector( Data::Mission::IFF &mission_file ) {
    std::vector<Resource*> to_copy = mission_file.getResources( Data::Mission::BMPResource::IDENTIFIER_TAG );

    std::vector<BMPResource*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<BMPResource*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::BMPResource*> Data::Mission::BMPResource::getVector( const Data::Mission::IFF &mission_file ) {
    return Data::Mission::BMPResource::getVector( const_cast< IFF& >( mission_file ) );
}
