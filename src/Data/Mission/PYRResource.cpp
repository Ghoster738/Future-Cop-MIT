#include "PYRResource.h"

#include "../../Utilities/DataHandler.h"
#include <string.h>
#include <fstream>

namespace {
    const uint32_t TAG_PYDT = 0x50594454; // which is { 0x50, 0x59, 0x44, 0x54 } or { 'P', 'Y', 'D', 'T' } or "PYDT"
    const uint32_t TAG_PYPL = 0x5059504C; // which is { 0x50, 0x59, 0x50, 0x4C } or { 'P', 'Y', 'P', 'L' } or "PYPL"
    const uint32_t TAG_PIX8 = 0x50495838; // which is { 0x50, 0x49, 0x58, 0x38 } or { 'P', 'I', 'X', '8' } or "PIX8"
}

Data::Mission::PYRIcon::PYRIcon( Utilities::Buffer::Reader &reader, Utilities::Buffer::Endian endian ) {

    this->id = reader.readU16( endian );

    uint8_t u3 = reader.readU8();
    uint8_t u4 = reader.readU8();

    this->location.x = reader.readU8();
    this->location.y = reader.readU8();

    uint8_t u7 = reader.readU8();
    uint8_t u8 = reader.readU8();

    this->size.x = reader.readU8();
    this->size.y = reader.readU8();

    // These two bytes should be zeros.
    uint8_t uB = reader.readU8();
    uint8_t uC = reader.readU8();
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

bool Data::Mission::PYRResource::parse( const Utilities::Buffer &header, const Utilities::Buffer &buffer, const ParseSettings &settings ) {

    auto reader = buffer.getReader();
    
    bool file_is_not_valid = false;
    uint32_t amount_of_tiles = 0;

    while( reader.getPosition( Utilities::Buffer::Reader::BEGINING ) < reader.totalSize() ) {
        auto identifier = reader.readU32( settings.endian );
        auto tag_size   = reader.readU32( settings.endian );
        auto tag_data_size = tag_size - 2 * sizeof( uint32_t );

        if( identifier == TAG_PYDT ) {
            auto readerPYDT = reader.getReader( tag_data_size );
            // This tag contains the uv cordinates for the particles, there are other bytes that are unknown, but for the most part I understand.
            amount_of_tiles = readerPYDT.readU32( settings.endian ); // 0x8

            for( unsigned int i = 0; i < amount_of_tiles; i++ ) {
                auto readerIcon = readerPYDT.getReader( sizeof( uint16_t ) + sizeof( uint8_t ) * 10 );
                
                icons.push_back( PYRIcon( readerIcon, settings.endian ) );
            }
        }
        else
        if( identifier == TAG_PYPL ) {
            auto readerPYPL = reader.getReader( tag_data_size );
            // I had originally thought that it was the palette for the image, and I was sort of right.
            // This chunk contains the palette for every explosion stored in this file.
            // However by XORing the mac and windows version of the files I had determined that these are 16 bit numbers. I am thankful for the big endian macs systems of the 1990's.

            for( unsigned int i = 0; i < amount_of_tiles; i++ ) {
                uint16_t first_zero = readerPYPL.readU16( settings.endian );
                uint16_t id = readerPYPL.readU16( settings.endian );
                uint16_t second_zero = readerPYPL.readU16( settings.endian );

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

                        Utilities::ImageData::translate_16_to_24( readerPYPL.readU16( settings.endian ), blue, green, red );

                        palette_data[0] = red;
                        palette_data[1] = green;
                        palette_data[2] = blue;

                        palette_data += palette->getPixelSize();
                    }
                }
                else
                {
                    if( settings.output_level >= 1 )
                        *settings.output_ref << "PYPL Error: id, " << id << ", != " << icons.at( i ).getID() << std::endl;
                    
                    // TODO
                }
            }
        }
        else
        if( identifier == TAG_PIX8 ) {
            auto readerPIX8 = reader.getReader( tag_data_size );

            // setup the image
            image.setWidth( 0x100 );
            image.setHeight( 0x200 );
            image.setFormat( Utilities::ImageData::BLACK_WHITE, 1 );

            auto image_data = image.getRawImageData();
            
            for( unsigned int a = 0; a < tag_data_size; a++ ) {
                *image_data = readerPIX8.readU8();

                image_data += image.getPixelSize();
            }
        }
        else
        {
            reader.setPosition( tag_data_size, Utilities::Buffer::Reader::CURRENT );
        }
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
