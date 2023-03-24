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

Data::Mission::PYRResource::Particle::Texture::Texture( Utilities::Buffer::Reader &reader ) : location(), size(), palette( Utilities::PixelFormatColor_R5G5B5A1() ) {
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

const Utilities::ColorPalette* Data::Mission::PYRResource::Particle::Texture::getPalette() const {
    return &palette;
}

Utilities::ColorPalette* Data::Mission::PYRResource::Particle::Texture::getPalette() {
    return &palette;
}

const std::string Data::Mission::PYRResource::FILE_EXTENSION = "pyr";
const uint32_t Data::Mission::PYRResource::IDENTIFIER_TAG = 0x43707972; // which is { 0x43, 0x70, 0x79, 0x72 } or { 'C', 'p', 'y', 'r' } or "Cpyr"

Data::Mission::PYRResource::PYRResource() : primary_image_p( nullptr ) {
}

Data::Mission::PYRResource::PYRResource( const PYRResource &obj ) : Resource( obj ), particles(), primary_image_p( nullptr ) {
    
    // TODO This is incomplete!
    
    if( obj.primary_image_p != nullptr )
        primary_image_p = new Utilities::ImagePalette2D( *obj.primary_image_p );
}

Data::Mission::PYRResource::~PYRResource() {
    if( primary_image_p != nullptr )
        delete primary_image_p;
    
    primary_image_p = nullptr;
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
        auto color_profile_w8 = Utilities::PixelFormatColor_W8();

        while( reader.getPosition( Utilities::Buffer::BEGIN ) < reader.totalSize() ) {
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
                PYPL_offset = reader.getPosition( Utilities::Buffer::BEGIN );
                PYPL_tag_size = tag_data_size;
                reader.setPosition( tag_data_size, Utilities::Buffer::CURRENT );
            }
            else
            if( identifier == TAG_PIX8 ) {
                auto readerPIX8 = reader.getReader( tag_data_size );
                
                Utilities::ColorPalette color_palette( color_profile_w8 );
                color_palette.setAmount( PC_PALETTE_SIZE );
                
                for( int i = 0; i < PC_PALETTE_SIZE; i++ ) {
                    color_palette.setIndex( i, Utilities::PixelFormatColor::GenericColor( static_cast<float>(i) / static_cast<float>(PC_PALETTE_SIZE), 1.0f, 1.0f, 1.0f) );
                }
                
                if( primary_image_p != nullptr )
                    delete primary_image_p;
                primary_image_p = new Utilities::ImagePalette2D( 0x100, 0x200, color_palette );
                
                if( primary_image_p != nullptr ) {
                    primary_image_p->fromReader( readerPIX8 );
                }
            }
            else
            if( identifier == TAG_PIX4 ) {
                auto readerPIX4 = reader.getReader( tag_data_size );
                
                Utilities::ColorPalette color_palette( color_profile_w8 );
                color_palette.setAmount( PS1_PALETTE_SIZE );
                
                for( int i = 0; i < PS1_PALETTE_SIZE; i++ ) {
                    color_palette.setIndex( i, Utilities::PixelFormatColor::GenericColor( static_cast<float>(i) / static_cast<float>(PS1_PALETTE_SIZE), 1.0f, 1.0f, 1.0f) );
                }
                
                if( primary_image_p != nullptr )
                    delete primary_image_p;
                primary_image_p = new Utilities::ImagePalette2D( 0x100, 0x200, color_palette );
                
                if( primary_image_p != nullptr ) {
                    auto bitfield = readerPIX4.getBitfield();
                    
                    for( size_t i = 0; i < bitfield.size() / 8; i++ ) {
                        // TODO Replace this with something more offical.
                        bool small_bitfield[8];
                        for( unsigned b = 0; b < 8; b++ )
                            small_bitfield[ b ] = bitfield[ i * 8 + b ];

                        std::swap( small_bitfield[ 0 ], small_bitfield[ 4 ] );
                        std::swap( small_bitfield[ 1 ], small_bitfield[ 5 ] );
                        std::swap( small_bitfield[ 2 ], small_bitfield[ 6 ] );
                        std::swap( small_bitfield[ 3 ], small_bitfield[ 7 ] );

                        for( unsigned b = 0; b < 8; b++ )
                            bitfield[ i * 8 + b ] = small_bitfield[ b ];
                    }
                    
                    primary_image_p->fromBitfield( bitfield, 4 );
                }

                is_PS1 = true;
            }
            else
            {
                if( settings.output_level >= 1 )
                    *settings.output_ref << "PYR Tag Error, 0x" << std::hex << identifier << std::dec << std::endl;
                reader.setPosition( tag_data_size, Utilities::Buffer::CURRENT );
            }
        }

        if( PYPL_tag_size != 0 ) {
            reader.setPosition( PYPL_offset, Utilities::Buffer::BEGIN );
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
                        auto color_palette_r = particles.at( i ).getTexture(0)->getPalette();
                        auto color_format_r = color_palette_r->getColorFormat();
                        color_palette_r->setAmount( PC_PALETTE_SIZE );

                        for( unsigned int d = 0; d < PC_PALETTE_SIZE; d++ ) {
                            color_palette_r->setIndex( d, color_format_r->readPixel( readerPYPL, settings.endian ) );
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
                        auto color_palette_r = particles.at( p ).getTexture( t )->getPalette();
                        auto color_format_r = color_palette_r->getColorFormat();
                        color_palette_r->setAmount( PS1_PALETTE_SIZE );

                        for( unsigned int d = 0; d < PS1_PALETTE_SIZE; d++ ) {
                            color_palette_r->setIndex( d, Utilities::PixelFormatColor_B5G5R5A1().readPixel( readerPYPL, settings.endian ) );
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

int Data::Mission::PYRResource::write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    int return_value = 0;
    Utilities::ImageFormat::Chooser chooser;

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

            Utilities::ImagePalette2D sub_image( texture->getSize().x, texture->getSize().y, *texture->getPalette() );
            
            primary_image_p->subImage(
                texture->getLocation().x, texture->getLocation().y,
                texture->getSize().x,     texture->getSize().y, sub_image );

            Utilities::ImageFormat::ImageFormat* the_choosen_r = chooser.getWriterReference( sub_image );

            if( iff_options.pyr.shouldWrite( iff_options.enable_global_dry_default ) && the_choosen_r != nullptr ) {
                the_choosen_r->write( sub_image, buffer );
                buffer.write( the_choosen_r->appendExtension( file_path_texture ) );
                buffer.set( nullptr, 0 );
            }
        }
        return_value = 1;
    }

    if( iff_options.pyr.export_prime_bw && iff_options.pyr.shouldWrite( iff_options.enable_global_dry_default ) ) {
        Utilities::ImageFormat::ImageFormat* the_choosen_r = chooser.getWriterReference( *primary_image_p );

        if( the_choosen_r != nullptr ) {
            the_choosen_r->write( *primary_image_p, buffer );
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

bool Data::Mission::IFFOptions::PYROption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {
    if( !singleArgument( arguments, "--" + getNameSpace() + "_PRIME_BLACK_WHITE", output_r, export_prime_bw ) )
        return false; // The single argument is not valid.

    return IFFOptions::ResourceOption::readParams( arguments, output_r );
}

std::string Data::Mission::IFFOptions::PYROption::getOptions() const {
    std::string information_text = getBuiltInOptions( 11 );

    information_text += "  --" + getNameSpace() + "_PRIME_BLACK_WHITE Export the index values as a single black and white image. This will look ugly\n";

    return information_text;
}
