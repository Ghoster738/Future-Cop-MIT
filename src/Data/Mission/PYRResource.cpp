#include "PYRResource.h"

#include "../../Utilities/DataHandler.h"
#include <string.h>
#include <fstream>

namespace {
    const uint32_t TAG_PYDT = 0x50594454; // which is { 0x50, 0x59, 0x44, 0x54 } or { 'P', 'Y', 'D', 'T' } or "PYDT"
    const uint32_t TAG_PYPL = 0x5059504C; // which is { 0x50, 0x59, 0x50, 0x4C } or { 'P', 'Y', 'P', 'L' } or "PYPL"
    const uint32_t TAG_PIX8 = 0x50495838; // which is { 0x50, 0x49, 0x58, 0x38 } or { 'P', 'I', 'X', '8' } or "PIX8"
}

Data::Mission::PYRIcon::PYRIcon( uint8_t * data ) {

    this->id = Utilities::DataHandler::read_u16_little( data );
    data += sizeof( uint16_t );

    uint8_t u3 = Utilities::DataHandler::read_u8( data );
    data += sizeof( uint8_t );
    uint8_t u4 = Utilities::DataHandler::read_u8( data );
    data += sizeof( uint8_t );

    this->location.x = Utilities::DataHandler::read_u8( data );
    data += sizeof( uint8_t );
    this->location.y = Utilities::DataHandler::read_u8( data );
    data += sizeof( uint8_t );

    uint8_t u7 = Utilities::DataHandler::read_u8( data );
    data += sizeof( uint8_t );
    uint8_t u8 = Utilities::DataHandler::read_u8( data );
    data += sizeof( uint8_t );

    this->size.x = Utilities::DataHandler::read_u8( data );
    data += sizeof( uint8_t );
    this->size.y = Utilities::DataHandler::read_u8( data );
    data += sizeof( uint8_t );

    // These two bytes should be zeros.
    uint8_t uB = Utilities::DataHandler::read_u8( data );
    data += sizeof( uint8_t );
    uint8_t uC = Utilities::DataHandler::read_u8( data );
    data += sizeof( uint8_t );
    /*
    std::cout << "ID: " << id << ": "
        << static_cast<int>( location_x ) << ", " << static_cast<int>( location_y ) << ", "
        << static_cast<int>( size_x ) << ", " << static_cast<int>( size_y ) << std::endl;*/
}

uint16_t Data::Mission::PYRIcon::getID() const {
    return this->id;
}

Utilities::DataTypes::Vec2UByte Data::Mission::PYRIcon::getLocation() const {
    return this->location;
}

Utilities::DataTypes::Vec2UByte Data::Mission::PYRIcon::getSize() const {
    return this->size;
}

const Utilities::ImageData* Data::Mission::PYRIcon::getPalette() const {
    return &palette;
}

Utilities::ImageData* Data::Mission::PYRIcon::getPalette() {
    return &palette;
}

const std::string Data::Mission::PYRResource::FILE_EXTENSION = "pyr";
const uint32_t Data::Mission::PYRResource::IDENTIFIER_TAG = 0x43707972; // which is { 0x43, 0x70, 0x79, 0x72 } or { 'C', 'p', 'y', 'r' } or "Cpyr"

Data::Mission::PYRResource::PYRResource() {

}

Data::Mission::PYRResource::PYRResource( const PYRResource &obj ) : image( obj.image ) {
}

std::string Data::Mission::PYRResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::PYRResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool Data::Mission::PYRResource::parse( const Utilities::Buffer &header, const Utilities::Buffer &reader_data, const ParseSettings &settings ) {
    auto raw_data = reader_data.getReader().getBytes();

    bool file_is_not_valid = false;
    auto data = raw_data.data();
    uint32_t amount_of_tiles = 0;

    while( static_cast<unsigned int>(data - raw_data.data()) < raw_data.size() ) {
        auto identifier = Utilities::DataHandler::read_u32_little( data );
        auto tag_size   = Utilities::DataHandler::read_u32_little( data + sizeof( uint32_t ) );

        if( identifier == TAG_PYDT ) {
            auto pydt_data = data + sizeof( uint32_t ) * 2;
            // This tag contains the uv cordinates for the particles, there are other bytes that are unknown, but for the most part I understand.
            amount_of_tiles = Utilities::DataHandler::read_u32_little( pydt_data ); // 0x8
            pydt_data += sizeof( uint32_t );

            for( unsigned int i = 0; i < amount_of_tiles; i++ ) {
                icons.push_back( PYRIcon( pydt_data ) );

                pydt_data += sizeof( uint16_t ) + sizeof( uint8_t ) * 10;
            }
        }
        else
        if( identifier == TAG_PYPL ) {
            auto pypl_data = data + sizeof( uint32_t ) * 2;
            // I had originally thought that it was the palette for the image, and I was sort of right.
            // This chunk contains the palette for every explosion stored in this file.
            // However by XORing the mac and windows version of the files I had determined that these are 16 bit numbers. I am thankful for the big endian macs systems of the 1990's.

            for( unsigned int i = 0; i < amount_of_tiles; i++ ) {
                uint16_t first_zero = Utilities::DataHandler::read_u16_little( pypl_data );
                pypl_data += sizeof( uint16_t );
                uint16_t id = Utilities::DataHandler::read_u16_little( pypl_data );
                pypl_data += sizeof( uint16_t );
                uint16_t second_zero = Utilities::DataHandler::read_u16_little( pypl_data );
                pypl_data += sizeof( uint16_t );

                if( settings.output_level >= 2 )
                    *settings.output_ref << "PYPL ID: " << id << std::endl;
                if( id == icons.at( i ).getID() )
                {
                    Utilities::ImageData *palette = icons.at( i ).getPalette();
                    palette->setWidth(  0x001 );
                    palette->setHeight( 0x100 );
                    palette->setFormat( Utilities::ImageData::RED_GREEN_BLUE, 1 );

                    auto palette_data = palette->getRawImageData();

                    for( unsigned int d = 0; d < palette->getHeight(); d++ ) {
                        uint8_t red, green, blue;

                        Utilities::ImageData::translate_16_to_24( Utilities::DataHandler::read_u16_little( pypl_data ), blue, green, red );

                        palette_data[0] = red;
                        palette_data[1] = green;
                        palette_data[2] = blue;

                        pypl_data += sizeof( uint16_t );

                        palette_data += palette->getPixelSize();

                    }
                }
                else
                {
                    if( settings.output_level >= 1 )
                        *settings.output_ref << "PYPL Error: id, " << id << ", != " << icons.at( i ).getID() << std::endl;
                    for( unsigned int d = 0; d < 0xff; d++ ) {
                        pypl_data += sizeof( uint16_t );
                    }
                }
            }
        }
        else
        if( identifier == TAG_PIX8 ) {
            auto pix8_data = data + sizeof( uint32_t ) * 2;

            // setup the image
            image.setWidth( 0x100 );
            image.setHeight( 0x200 );
            image.setFormat( Utilities::ImageData::BLACK_WHITE, 1 );

            auto image_data = image.getRawImageData();
            for( unsigned int a = 0; a < tag_size - 8; a++ ) {
                *image_data = *pix8_data;

                image_data += image.getPixelSize();
                pix8_data++;
            }
        }

        data += tag_size;
    }

    return !file_is_not_valid;
}

Data::Mission::Resource * Data::Mission::PYRResource::duplicate() const {
    return new Data::Mission::PYRResource( *this );
}

int Data::Mission::PYRResource::write( const char *const file_path, const std::vector<std::string> & arguments ) const {
    bool export_prime_bw = false;
    bool enable_export = true;
    int return_value = 0;

    for( auto arg = arguments.begin(); arg != arguments.end(); arg++ ) {
        if( (*arg).compare("--PYR_Prime_BlackWhite") == 0 )
            export_prime_bw = true;
        else
        if( (*arg).compare("--dry") == 0 )
            enable_export = false;
    }

    for( auto current = icons.begin(); current != icons.end(); current++ ) {
        std::string file_path_texture = std::string( file_path );
        file_path_texture += " ";
        file_path_texture += std::to_string( static_cast<int>( (*current).getID() ) );
        file_path_texture += ".png";

        auto sub_image = image.subImage(
            (*current).getLocation().x, (*current).getLocation().y,
            (*current).getSize().x,     (*current).getSize().y );

        if( enable_export )
            sub_image.applyPalette( (*(*current).getPalette()) ).write( file_path_texture.c_str() );

        return_value = 1;
    }

    if( export_prime_bw && enable_export )
    {
        std::string file_path_texture = std::string( file_path ) + ".png";
        return_value = image.write( file_path_texture.c_str() );
    }

    return return_value;
}

std::vector<Data::Mission::PYRResource*> Data::Mission::PYRResource::getVector( IFF &mission_file ) {
    std::vector<Resource*> to_copy = mission_file.getResources( Data::Mission::PYRResource::IDENTIFIER_TAG );

    std::vector<PYRResource*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<PYRResource*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::PYRResource*> Data::Mission::PYRResource::getVector( const IFF &mission_file ) {
    return Data::Mission::PYRResource::getVector( const_cast< IFF& >( mission_file ) );
}
