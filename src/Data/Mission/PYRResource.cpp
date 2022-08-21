#include "PYRResource.h"

#include "../../Utilities/DataHandler.h"
#include "../../Utilities/ImageFormat/Chooser.h"
#include <string.h>
#include <fstream>
#include <cassert>

namespace {
    const uint32_t TAG_PYDT = 0x50594454; // which is { 0x50, 0x59, 0x44, 0x54 } or { 'P', 'Y', 'D', 'T' } or "PYDT"
    const uint32_t TAG_PYPL = 0x5059504C; // which is { 0x50, 0x59, 0x50, 0x4C } or { 'P', 'Y', 'P', 'L' } or "PYPL"
    const uint32_t TAG_PIX8 = 0x50495838; // which is { 0x50, 0x49, 0x58, 0x38 } or { 'P', 'I', 'X', '8' } or "PIX8"
    const uint32_t TAG_PIX4 = 0x50495834; // which is { 0x50, 0x49, 0x58, 0x34 } or { 'P', 'I', 'X', '4' } or "PIX4" Playstation
}

Data::Mission::PYRResource::Particle::Particle( Utilities::Buffer::Reader &reader, Utilities::Buffer::Endian endian ) {

    // Read the opcode
    this->id = reader.readU16( endian );
    this->num_sprites = reader.readU8();
    this->sprite_size = reader.readU8(); // Power of two size

    assert( this->num_sprites != 0 ); // This should not crash at all.
    // This should not crash at all.
    assert( sprite_size == 0x80 | sprite_size == 0x40 | sprite_size == 0x20 | sprite_size == 0x10 );

    this->textures.reserve( this->num_sprites );

    for( unsigned int i = 0; i < this->num_sprites; i++ ) {
        this->textures.push_back( reader );
    }
}

uint16_t Data::Mission::PYRResource::Particle::getID() const {
    return this->id;
}

uint8_t Data::Mission::PYRResource::Particle::getNumSprites() const {
    return this->num_sprites;
}

uint8_t Data::Mission::PYRResource::Particle::getSpriteSize() const {
    return this->sprite_size;
}

const Data::Mission::PYRResource::Particle::Texture *const Data::Mission::PYRResource::Particle::getTexture( uint8_t index ) const {
    return textures.data() + index;
}

Data::Mission::PYRResource::Particle::Texture* Data::Mission::PYRResource::Particle::getTexture( uint8_t index ) {
    return textures.data() + index;
}

Data::Mission::PYRResource::Particle::Texture::Texture( Utilities::Buffer::Reader &reader ) {
    this->location.x = reader.readU8();
    this->location.y = reader.readU8();

    uint8_t u2 = reader.readU8();
    uint8_t u3 = reader.readU8();

    // assert( u2 == 0 | u2 == 1 ); // This will crash on PS1 not PC
    // assert( u3 == 0 | u3 == 1 ); // This will crash on PS1 not PC

    this->size.x = reader.readU8();
    this->size.y = reader.readU8();

    // These two bytes are be zeros in Mac and Windows
    uint8_t u4 = reader.readU8();
    uint8_t u5 = reader.readU8();

    if( u4 == 1 )
        this->location.y = this->location.y | 256;

    //assert( u4 == 0 ); // This will crash on PS1 not PC
    assert( (u4 == 0) | (u4 == 1) ); // This will not crash on any platform.
    assert( u5 == 0 ); // This will crash on PS1 not PC
}

glm::u16vec2 Data::Mission::PYRResource::Particle::Texture::getLocation() const {
    return this->location;
}

glm::u8vec2 Data::Mission::PYRResource::Particle::Texture::getSize() const {
    return this->size;
}

uint8_t* Data::Mission::PYRResource::Particle::Texture::setupPallete( unsigned int number_of_colors ) {
    this->palette.setWidth(  1 );
    this->palette.setHeight( number_of_colors );
    this->palette.setFormat( Utilities::ImageData::RED_GREEN_BLUE, 1 );

    return this->palette.getRawImageData();
}

const Utilities::ImageData* Data::Mission::PYRResource::Particle::Texture::getPalette() const {
    return &palette;
}

Utilities::ImageData* Data::Mission::PYRResource::Particle::Texture::getPalette() {
    return &palette;
}

const std::string Data::Mission::PYRResource::FILE_EXTENSION = "pyr";
const uint32_t Data::Mission::PYRResource::IDENTIFIER_TAG = 0x43707972; // which is { 0x43, 0x70, 0x79, 0x72 } or { 'C', 'p', 'y', 'r' } or "Cpyr"

Data::Mission::PYRResource::PYRResource() {
}

Data::Mission::PYRResource::PYRResource( const PYRResource &obj ) : image( obj.image ) {
}

Data::Mission::PYRResource::~PYRResource() {
}

std::string Data::Mission::PYRResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::PYRResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool Data::Mission::PYRResource::parse( const ParseSettings &settings ) {
    if( this->data_p != nullptr )
        {
        auto reader = this->data_p->getReader();
        auto readerPYPL = this->data_p->getReader();

        bool file_is_not_valid = false;
        uint32_t amount_of_tiles = 0;

        size_t PYPL_offset;
        size_t PYPL_tag_size = 0;
        bool is_PS1 = false;

        while( reader.getPosition( Utilities::Buffer::Reader::BEGIN ) < reader.totalSize() ) {
            auto identifier = reader.readU32( settings.endian );
            auto tag_size   = reader.readU32( settings.endian );
            auto tag_data_size = tag_size - 2 * sizeof( uint32_t );

            if( identifier == TAG_PYDT ) {
                auto readerPYDT = reader.getReader( tag_data_size );

                // This number contains the amount of particles.
                amount_of_tiles = readerPYDT.readU32( settings.endian );

                for( unsigned int i = 0; i < amount_of_tiles; i++ )
                    particles.push_back( Particle( readerPYDT, settings.endian ) );
            }
            else
            if( identifier == TAG_PYPL ) {
                PYPL_offset = reader.getPosition( Utilities::Buffer::Reader::BEGIN );
                PYPL_tag_size = tag_data_size;
                reader.setPosition( tag_data_size, Utilities::Buffer::Reader::CURRENT );
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
            if( identifier == TAG_PIX4 ) {
                auto readerPIX4 = reader.getReader( tag_data_size );

                // setup the image
                image.setWidth( 0x100 );
                image.setHeight( 0x200 );
                image.setFormat( Utilities::ImageData::BLACK_WHITE, 1 );

                auto image_data = image.getRawImageData();

                uint8_t pixel;

                for( unsigned int a = 0; a < tag_data_size; a++ ) {
                    pixel = readerPIX4.readU8();

                    image_data[ 0 ] = (pixel >> 0) & 0xF;
                    image_data[ 1 ] = (pixel >> 4) & 0xF;
                    image_data += image.getPixelSize() * 2;
                }

                is_PS1 = true;
            }
            else
            {
                if( settings.output_level >= 1 )
                    *settings.output_ref << "PYR Tag Error, 0x" << std::hex << identifier << std::dec << std::endl;
                reader.setPosition( tag_data_size, Utilities::Buffer::Reader::CURRENT );
            }
        }

        if( PYPL_tag_size != 0 ) {
            reader.setPosition( PYPL_offset, Utilities::Buffer::Reader::BEGIN );
            auto readerPYPL = reader.getReader( PYPL_tag_size );

            if( !is_PS1 ) {
                // I had originally thought that it was the palette for the image, and I was sort of right.
                // This chunk contains the palette for every explosion stored in this file.
                // However by XORing the mac and windows version of the files I had determined that these are 16 bit numbers. I am thankful for the big endian macs systems of the 1990's.

                for( unsigned int i = 0; i < amount_of_tiles; i++ ) {
                    uint16_t first_zero = readerPYPL.readU16( settings.endian );
                    uint16_t id = readerPYPL.readU16( settings.endian );

                    if( settings.output_level >= 2 )
                        *settings.output_ref << "PYPL ID: " << id << std::endl;

                    if( id == particles.at( i ).getID() )
                    {
                        auto palette_data = particles.at( i ).getTexture(0)->setupPallete( PC_PALLETE_SIZE );

                        for( unsigned int d = 0; d < PC_PALLETE_SIZE; d++ ) {
                            uint8_t red, green, blue;

                            Utilities::ImageData::translate_16_to_24( readerPYPL.readU16( settings.endian ), blue, green, red );

                            palette_data[0] = red;
                            palette_data[1] = green;
                            palette_data[2] = blue;

                            palette_data += particles.at( i ).getTexture(0)->getPalette()->getPixelSize();
                        }
                    }
                    else
                    {
                        if( settings.output_level >= 1 )
                            *settings.output_ref << "PYPL Error: id, " << id << ", != " << particles.at( i ).getID() << std::endl;
                        i = amount_of_tiles; // Cancel the reading.
                    }
                }
            }
            else
            {
                for( unsigned int p = 0; p < amount_of_tiles; p++ ) {
                    for( unsigned int t = 0; t < particles.at( p ).getNumSprites(); t++ ) {
                        auto palette_data = particles.at( p ).getTexture( t )->setupPallete( PS1_PALLETE_SIZE );

                        for( unsigned int d = 0; d < PS1_PALLETE_SIZE; d++ ) {
                            uint8_t red, green, blue;

                            Utilities::ImageData::translate_16_to_24( readerPYPL.readU16( settings.endian ), red, green, blue );

                            palette_data[0] = red;
                            palette_data[1] = green;
                            palette_data[2] = blue;

                            palette_data += particles.at( p ).getTexture( t )->getPalette()->getPixelSize();
                        }
                    }
                }
            }
        }

        return !file_is_not_valid;
    }
    else
        return false;
}

Data::Mission::Resource * Data::Mission::PYRResource::duplicate() const {
    return new Data::Mission::PYRResource( *this );
}

int Data::Mission::PYRResource::write( const char *const file_path, const std::vector<std::string> & arguments ) const {
    bool export_prime_bw = false;
    bool enable_export = true;
    int return_value = 0;
    Utilities::ImageFormat::Chooser chooser;

    for( auto arg = arguments.begin(); arg != arguments.end(); arg++ ) {
        if( (*arg).compare("--PYR_Prime_BlackWhite") == 0 )
            export_prime_bw = true;
        else
        if( (*arg).compare("--dry") == 0 )
            enable_export = false;
    }

    Utilities::Buffer buffer;

    for( auto current_particle = particles.begin(); current_particle != particles.end(); current_particle++ ) {

        for( unsigned int index = 0; index != (*current_particle).getNumSprites(); index++ ) {
            std::string file_path_texture = std::string( file_path );
            file_path_texture += " ";
            file_path_texture += std::to_string( static_cast<int>( (*current_particle).getID() ) );

            if( (*current_particle).getNumSprites() != 1 )
            {
                file_path_texture += " f ";
                file_path_texture += std::to_string( index );
            }

            auto texture = (*current_particle).getTexture( index );

            auto sub_image = image.subImage(
                texture->getLocation().x, texture->getLocation().y,
                texture->getSize().x,     texture->getSize().y );

            auto palleted_image = sub_image.applyPalette( (*texture->getPalette()) );

            Utilities::ImageFormat::ImageFormat* the_choosen_r = chooser.getWriterReference( palleted_image );

            if( enable_export && the_choosen_r != nullptr ) {
                the_choosen_r->write( palleted_image, buffer );
                buffer.write( the_choosen_r->appendExtension( file_path_texture ) );
                buffer.set( nullptr, 0 );
            }
        }
        return_value = 1;
    }

    if( export_prime_bw && enable_export ) {
        Utilities::ImageFormat::ImageFormat* the_choosen_r = chooser.getWriterReference( image );

        if( the_choosen_r != nullptr ) {
            the_choosen_r->write( image, buffer );
            buffer.write( the_choosen_r->appendExtension( file_path ) );
        }
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
