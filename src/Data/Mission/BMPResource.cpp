#include "BMPResource.h"

#include <cmath>
#include "../../Utilities/DataHandler.h"
#include <fstream>

namespace {
char LITTLE_CCB [] = {' ','B','C','C'};
char LITTLE_LkUp[] = {'p','U','k','L'}; // This is probably something for converting 16 bit data to 8 bit pallete data.
char LITTLE_PX16[] = {'6','1','X','P'}; // This is the pixel data for the computer versions.
char LITTLE_PLUT[] = {'T','U','L','P'}; // Color pallete both used in Playstation 1 and the desktop versions.
char LITTLE_PDAT[] = {'T','A','D','P'}; // This is the pixel data for the Playstation 1 version, and probably the computer versions.
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

bool Data::Mission::BMPResource::parse( const Utilities::Buffer &header, const Utilities::Buffer &reader_data, const ParseSettings &settings ) {
    auto raw_data = reader_data.getReader().getBytes();

    bool file_is_not_valid = false;
    bool is_playstation_texture = false; // PlayStation textures have 8 bit pixels that are indexes to the palette colors.
    auto data = raw_data.data();

    while( static_cast<unsigned int>( data - raw_data.data() ) < raw_data.size() ) {
        auto identifier = Utilities::DataHandler::read_u32( data, settings.is_opposite_endian );
        auto tag_size   = Utilities::DataHandler::read_u32( data + sizeof( uint32_t ), settings.is_opposite_endian );

        if( identifier == *reinterpret_cast<uint32_t*>(LITTLE_CCB ) ) {
            auto cbb_data = data + sizeof( uint32_t ) * 2;
        }
        else
        if( identifier == *reinterpret_cast<uint32_t*>(LITTLE_LkUp) ) {
            auto lkup_data = data + sizeof( uint32_t ) * 2;

            const size_t LOOKUP_DATA_AMOUNT = sizeof( lookUpData ) / sizeof( lookUpData[0] );

            // This seems to be all 8 bit data in 4 lists each containing 0x100 bytes.
            // In second thought
            // I could be that this is simply a big list of 0x400 unsigned bytes and the 16-bit data is simply going to scale down.
            for( unsigned int i = 0; i < LOOKUP_DATA_AMOUNT; i++ )
            {
                lookUpData[ i ] = *lkup_data;
                lkup_data++;
            }
        }
        else
        if( identifier == *reinterpret_cast<uint32_t*>(LITTLE_PDAT) ) { // For PlayStation
            auto px_raw_data = data + sizeof( uint32_t ) * 2;

            // setup the image
            image_raw.setWidth( 0x100 );
            image_raw.setHeight( 0x100 );
            image_raw.setFormat( Utilities::ImageData::BLACK_WHITE, 1 );

            auto image_data_8_bit = image_raw.getRawImageData();
            for( unsigned int a = 0; a < image_raw.getWidth() * image_raw.getHeight(); a++ ) {

                *reinterpret_cast<uint8_t*>(image_data_8_bit) = Utilities::DataHandler::read_u8( px_raw_data );

                image_data_8_bit += image_raw.getPixelSize();
                px_raw_data += sizeof( uint8_t );
            }
        }
         else
         if( identifier == *reinterpret_cast<uint32_t*>(LITTLE_PX16) ) { // For Windows
            auto px_raw_data = data + sizeof( uint32_t ) * 2;

            // setup the image
            image_raw.setWidth( 0x100 );
            image_raw.setHeight( 0x100 );
            image_raw.setFormat( Utilities::ImageData::BLACK_WHITE, 2 );

            auto image_data_16bit = image_raw.getRawImageData();
            for( unsigned int a = 0; a < image_raw.getWidth() * image_raw.getHeight(); a++ ) {

                *reinterpret_cast<uint16_t*>(image_data_16bit) = Utilities::DataHandler::read_u16( px_raw_data, settings.is_opposite_endian );

                image_data_16bit += image_raw.getPixelSize();
                px_raw_data += sizeof( uint16_t );
            }
        }
        else
        if( identifier == *reinterpret_cast<uint32_t*>(LITTLE_PLUT) ) {
            auto plut_data = data + sizeof( uint32_t ) * 2;

            // The color pallette is located 12 bytes away from the start of the tag.
            auto color_pallete = plut_data + 0xC;

            // Now store the color palette.
            palette.setWidth( 1 );
            palette.setHeight( 0x100 );
            palette.setFormat( Utilities::ImageData::RED_GREEN_BLUE, 1 );

            auto palette_data = palette.getRawImageData();
            uint8_t red, green, blue;

            for( unsigned int d = 0; d < palette.getHeight(); d++ ) {
                Utilities::ImageData::translate_16_to_24( Utilities::DataHandler::read_u16( color_pallete, settings.is_opposite_endian ), blue, green, red );

                palette_data[0] = red;
                palette_data[1] = green;
                palette_data[2] = blue;

                color_pallete += sizeof( uint16_t );

                palette_data += palette.getPixelSize();
            }
        }
        else
        {
            data = raw_data.data() + raw_data.size();
        }

        data += tag_size;
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
            uint8_t red, green, blue;

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
