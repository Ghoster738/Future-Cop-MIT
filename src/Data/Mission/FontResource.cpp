#include "FontResource.h"

#include "../../Utilities/DataHandler.h"
#include "../../Utilities/ImageFormat/Chooser.h"
#include "../../Utilities/ImagePalette2D.h"
#include <string.h>
#include <fstream>
#include <cassert>
#include <stdexcept>

namespace {
    const size_t HEADER_SIZE = 0x20;
    // For some reason the Windows version stores this as a big endian 32 bit number. Mac uses the little endian.
    // I think this format was made for Windows programs, but the Future Cop developers used it for the Mac version.
    const uint32_t FNTP_LITTLE_TAG = 0x50544E46; // which is { 0x50, 0x54, 0x4E, 0x46 } or { 'P', 'T', 'N', 'F' } or "PTNF"

    const size_t GLYPH_SIZE = 0xB; // Contains the character code with parameters.

    const size_t IMAGE_HEADER_SIZE = 0x10;

#include "Embeded/PFNT.h"
}

Data::Mission::FontResource::Glyph::Glyph( Utilities::Buffer::Reader& reader ) {
    this->glyphID   = reader.readU8();
    this->unk_0     = reader.readU8(); // Skip a byte
    this->width     = reader.readU8();
    this->height    = reader.readU8();
    this->left      = reader.readU8();
    this->unk_1     = reader.readU8(); // Skip a byte
    this->top       = reader.readU8();
    this->unk_2     = reader.readU8(); // Skip a byte
    this->x_advance = reader.readU8();
    this->offset.x  = reader.readI8();
    this->offset.y  = reader.readI8();
}

const std::string Data::Mission::FontResource::FILE_EXTENSION = "fnt";
const uint32_t Data::Mission::FontResource::IDENTIFIER_TAG = 0x43666E74; // which is { 0x43, 0x66, 0x6E, 0x74 } or { 'C', 'f', 'n', 't' } or "Cfnt"

Data::Mission::FontResource::FontResource() : image_p( nullptr ) {
    for( int i = 0; i < sizeof( font_glyphs_r ) / sizeof( Glyph* ); i++ ) {
        font_glyphs_r[ i ] = nullptr;
    }
}

Data::Mission::FontResource::FontResource( const FontResource &obj ) : Resource(obj), image_p( nullptr ), glyphs( obj.glyphs ) {
    for( unsigned int i = 0; i < MAX_GLYPHS; i++ ) {
        font_glyphs_r[ i ] = obj.font_glyphs_r[ i ];
    }
    
    if( obj.image_p != nullptr )
        image_p = new Utilities::Image2D( *obj.image_p );
}

Data::Mission::FontResource::~FontResource() {
    if( image_p != nullptr )
        delete image_p;
    image_p = nullptr;
}

std::string Data::Mission::FontResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::FontResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

const Data::Mission::FontResource::Glyph *const Data::Mission::FontResource::getGlyph( uint8_t which ) const {
    return font_glyphs_r[ which ];
}

Data::Mission::FontResource::FilterStatus Data::Mission::FontResource::filterText( const std::string& unfiltered_text, std::string *filtered_text_r ) const {
    const uint8_t MISSING_SYMBOL = 0x7F;
    FilterStatus filter_status = FilterStatus::PERFECT;
    char space_char;

    // Before doing anything serious check if the missing symbol is present.
    if( getGlyph( MISSING_SYMBOL ) == nullptr ) {
        return FilterStatus::INVALID;
    }

    // Clear the filtered text.
    if( filtered_text_r != nullptr ) {
        *filtered_text_r = "";
    }

    // This sets the space character.
    if( getGlyph( ' ' ) != nullptr )
        space_char = ' ';
    else
        space_char = MISSING_SYMBOL;

    for( auto i = unfiltered_text.begin(); i != unfiltered_text.end(); i++ ) {

        if( (*i) == MISSING_SYMBOL ) {
            filter_status = FilterStatus::CULLED;

            if( filtered_text_r != nullptr )
                filtered_text_r->push_back( space_char ); // Push back a humble space character.
        }
        else
        if( getGlyph( (*i) ) == nullptr ) {
            filter_status = FilterStatus::CULLED;

            if( filtered_text_r != nullptr )
                filtered_text_r->push_back( MISSING_SYMBOL ); // This character does not exist.
        }
        else
        if( filtered_text_r != nullptr ) {
            filtered_text_r->push_back( (*i) ); // The character is valid.
        }
    }

    return filter_status;
}

bool Data::Mission::FontResource::parse( const ParseSettings &settings ) {
    if( this->data_p != nullptr )
    {
        auto reader = this->data_p->getReader();

        bool file_is_not_valid;

        if( reader.totalSize() > HEADER_SIZE )
        {
            if( getResourceID() == 0 )
                throw std::runtime_error( "Error: Resource ID of zero for FontResource is reserved only for use inside this program. Never in the IFF files." );

            if( settings.output_level >= 1 ) {
                *settings.output_ref << "Loading FNT " << getResourceID() << "!" << std::endl;
            }
            // Get the data first
            auto header   = reader.readU32( settings.endian );
            auto tag_size = reader.readU32( settings.endian );

            auto u16_100 = reader.readU16( settings.endian );
            auto number_of_glyphs = reader.readU16( settings.endian );
            auto platform = reader.readU32( settings.endian ); // Mac and Windows is 8, and Playstation is 9
            auto u16_0 = reader.readU16( settings.endian ); // This could be two 0 bytes
            auto unk_u8 = reader.readU8(); // Offset 0x12
            auto u8_0 = reader.readU8(); // This is always zero.
            auto offset_to_glyphs = reader.readU32( settings.endian );
            auto u32_0 = reader.readU32( settings.endian );
            auto offset_to_image_header = reader.readU32( settings.endian );

            if( settings.output_level >= 2 ) {
                *settings.output_ref << "  Header = 0x" << std::hex << header << std::dec << "\n";
                *settings.output_ref << "  Tag Size = 0x" << std::hex << tag_size << std::dec << "\n";
                *settings.output_ref << "  u16_100 = " << u16_100 << "\n";
                *settings.output_ref << "  number_of_glyphs = " << number_of_glyphs << "\n";
                *settings.output_ref << "  platform = " << platform << "\n";
                *settings.output_ref << "  unk_u8 height? = " << static_cast<uint32_t>( unk_u8 ) << "\n";
                *settings.output_ref << "  offset_to_glyphs = 0x" << std::hex << offset_to_glyphs << std::dec << "\n";
                *settings.output_ref << "  offset_to_image_header = 0x" << std::hex << offset_to_image_header << std::dec << "\n" << std::endl;
            }

            // assert( platform == 9 ); // This statement will not crash on Playstation 1 files.
            // assert( platform == 8 ); // This statement will not crash on Mac or Windows files.

            // Check to see if the data will work.
            file_is_not_valid = false;
            file_is_not_valid |= ( header != FNTP_LITTLE_TAG );
            file_is_not_valid |= ( (number_of_glyphs * GLYPH_SIZE + offset_to_glyphs) > reader.totalSize() );
            file_is_not_valid |= ( (IMAGE_HEADER_SIZE + offset_to_image_header) > reader.totalSize() );

            if( !file_is_not_valid )
            {
                if( settings.output_level >= 2 ) {
                    *settings.output_ref << "  FNT Header is valid!" << std::endl;
                }
                reader.setPosition( offset_to_glyphs, Utilities::Buffer::BEGIN );

                auto readerGlyphs = reader.getReader( number_of_glyphs * GLYPH_SIZE );

                this->glyphs.reserve( number_of_glyphs );

                for( unsigned int i = 0; i < number_of_glyphs; i++ )
                {
                    auto readerGlyph = readerGlyphs.getReader( GLYPH_SIZE );
                    this->glyphs.push_back( Glyph( readerGlyph ) );
                }

                // The reason why this is in a seperate loop is because the vector glyphs would reallocate.
                for( auto i = this->glyphs.cbegin(); i < this->glyphs.cend(); i++ ) {
                    const uint32_t wrapped_character = (*i).glyphID % MAX_GLYPHS;

                    if( settings.output_level >= 3 ) {
                        *settings.output_ref << "  glyphs[" << (i - this->glyphs.cbegin()) << "] = " << wrapped_character << std::endl;
                    }
                    font_glyphs_r[ wrapped_character ] = this->glyphs.data() + (i - this->glyphs.cbegin());
                }

                reader.setPosition( offset_to_image_header, Utilities::Buffer::BEGIN );

                auto readerImageHeader = reader.getReader( IMAGE_HEADER_SIZE );

                auto width  = static_cast<uint16_t>( readerImageHeader.readU8() ) * 4;
                readerImageHeader.setPosition( 0x6, Utilities::Buffer::BEGIN );
                auto height = readerImageHeader.readU16( settings.endian );

                if( settings.output_level >= 2 ) {
                    *settings.output_ref << "  width = " << width << "\n";
                    *settings.output_ref << "  height = " << height << "\n" << std::endl;
                }

                // The pixels are compressed with 4 bit color palettes.
                // Playstation did not support 1 bit, so their best option is 4 bit.
                auto color_format = Utilities::PixelFormatColor_W8();
                Utilities::ColorPalette color_palette( color_format );
                
                color_palette.setAmount( 0x10 );
                
                for( int i = 0; i < 8; i++ ) {
                    color_palette.setIndex( i + 0, Utilities::PixelFormatColor::GenericColor( 0, 0, 0, 0) );
                    color_palette.setIndex( i + 8, Utilities::PixelFormatColor::GenericColor( 1, 1, 1, 1) );
                }
                
                // For some reason they had the width divided by 4.
                Utilities::ImagePalette2D image_palette( width, height, color_palette );

                // Check to see if the image would go beyond the scope of the raw_data.
                file_is_not_valid |= ( (IMAGE_HEADER_SIZE + offset_to_image_header + (image_palette.getWidth() / 2) * image_palette.getHeight()) > reader.totalSize() );

                if( !file_is_not_valid ) {
                    if( settings.output_level >= 1 ) {
                        *settings.output_ref << "  FNT image is valid\n" << std::endl;
                    }

                    auto reader_image = reader.getReader( reader.totalSize() - (IMAGE_HEADER_SIZE + offset_to_image_header) );
                    
                    auto bits = reader_image.getBitfield();
                    
                    image_palette.fromBitfield( bits, 4 );
                    
                    if( image_p != nullptr )
                        delete image_p;
                    
                    auto backup = image_palette.toColorImage();
                    
                    image_p = new Utilities::Image2D( backup );

                    if( image_p == nullptr && settings.output_level >= 1 ) {
                        *settings.output_ref << "  FNT image failed\n" << std::endl;
                    }


                    // If the image fails to allocate then it failed.
                    return image_p != nullptr;
                }
                else
                {
                    if( settings.output_level >= 1 ) {
                        *settings.output_ref << "  FNT image is invalid\n" << std::endl;
                    }
                    return false;
                }
            }
            else
                return false;
        }
        else
            return false;
    }
    else
        return false;
}

Data::Mission::Resource * Data::Mission::FontResource::duplicate() const {
    return new Data::Mission::FontResource( *this );
}

Utilities::Image2D *const Data::Mission::FontResource::getImage() const {
    return const_cast<Utilities::Image2D *const>(image_p);
}

int Data::Mission::FontResource::write( const std::string& file_path, const std::vector<std::string> & arguments ) const {
    std::ofstream resource;
    bool export_enable = true;
    Utilities::ImageFormat::Chooser chooser;

    for( auto arg = arguments.begin(); arg != arguments.end(); arg++ ) {
        if( (*arg).compare("--dry") == 0 )
            export_enable = false;
    }

    if( export_enable ) {
        resource.open( std::string(file_path) + "." + getFileExtension(), std::ios::out );

        if( resource.is_open() ) {
            Utilities::ImageFormat::ImageFormat* the_choosen_r = chooser.getWriterReference( *image_p );

            resource << "info face=\"" << getFullName( getResourceID() ) << "\" size=24 bold=0 italic=0 charset=\"ASCII\""
                     << " unicode=0 stretchH=100 smooth=0 aa=1 padding=0,0,0,0 spacing=0,0 outline=1"<< std::endl;

            resource << "common lineHeight=24 base=19 scaleW=" << image_p->getWidth() << " scaleH="
                     << image_p->getHeight() << " pages=1 packed=0 alphaChnl=0 redChnl=0 greenChnl=0 blueChnl=0"
                     << std::endl;

            resource << "page id=0 file=\"" << the_choosen_r->appendExtension( getFullName( getResourceID() ) ) << "\"" << std::endl;

            resource << "chars count=" << glyphs.size() << std::endl;

            for( auto i = this->glyphs.cbegin(); i < this->glyphs.cend(); i++ ) {
                resource << "char id=" << (static_cast<unsigned int>((*i).glyphID ) & 0xFF)
                         << " x=" << static_cast<int>( (*i).left ) << " y=" << static_cast<int>( (*i).top )
                         << " width="  << static_cast<int>( (*i).width )
                         << " height=" << static_cast<int>( (*i).height )
                         << " xoffset=" << static_cast<int>( (*i).offset.x )
                         << " yoffset=" << static_cast<int>( (*i).offset.y )
                         << " xadvance=" << static_cast<int>( (*i).x_advance ) << " page=0 chnl=15"
                         << std::endl;
            }

            resource.close();

            if( the_choosen_r != nullptr ) {
                Utilities::Buffer buffer;
                int state = the_choosen_r->write( *image_p, buffer );

                buffer.write( the_choosen_r->appendExtension( file_path ) );
                return state;
            }
            else
                return -1;
        }
        else
            return -2;
    }
    else
        return 0; // A parameter stopped the exporting.
}

std::vector<Data::Mission::FontResource*> Data::Mission::FontResource::getVector( Data::Mission::IFF &mission_file ) {
    std::vector<Resource*> to_copy = mission_file.getResources( Data::Mission::FontResource::IDENTIFIER_TAG );

    std::vector<FontResource*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<FontResource*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::FontResource*> Data::Mission::FontResource::getVector( const Data::Mission::IFF &mission_file ) {
    return Data::Mission::FontResource::getVector( const_cast< Data::Mission::IFF& >( mission_file ) );
}

Data::Mission::FontResource* Data::Mission::FontResource::getWindows( std::ostream *stream, int output_level ) {
    Data::Mission::FontResource* windows_font_p = new Data::Mission::FontResource;

    windows_font_p->setIndexNumber( 0 );
    windows_font_p->setMisIndexNumber( 0 );
    windows_font_p->setResourceID( 2 );

    auto loading = Utilities::Buffer::Reader( windows_fnt, windows_fnt_len );

    windows_font_p->read( loading );

    Data::Mission::Resource::ParseSettings parse_settings;
    parse_settings.type = Data::Mission::Resource::ParseSettings::Windows;
    parse_settings.endian = Utilities::Buffer::LITTLE;
    parse_settings.output_level = output_level;
    parse_settings.output_ref = stream;

    if( !windows_font_p->parse( parse_settings ) )
        throw std::logic_error( "Internal Error: The internal Windows font has failed to parse!");

    // DEL symbol required for internal fonts!
    if( windows_font_p->getGlyph( 0x7F ) == nullptr )
        throw std::logic_error( "Internal Error: The internal Windows font does not have the DEL key symbol!");

    return windows_font_p;
}

Data::Mission::FontResource* Data::Mission::FontResource::getPlaystation( std::ostream *stream, int output_level ) {
    Data::Mission::FontResource* playstation_font_p = new Data::Mission::FontResource;

    playstation_font_p->setIndexNumber( 0 );
    playstation_font_p->setMisIndexNumber( 0 );
    playstation_font_p->setResourceID( 1 );

    auto loading = Utilities::Buffer::Reader( playstation_fnt, playstation_fnt_len );

    playstation_font_p->read( loading );

    Data::Mission::Resource::ParseSettings parse_settings;
    parse_settings.type = Data::Mission::Resource::ParseSettings::Playstation;
    parse_settings.endian = Utilities::Buffer::LITTLE;
    parse_settings.output_level = output_level;
    parse_settings.output_ref = stream;

    if( !playstation_font_p->parse( parse_settings ) )
        throw std::logic_error( "Internal Error: The internal Playstation font has failed to parse!");

    // DEL symbol required for internal fonts!
    if( playstation_font_p->getGlyph( 0x7F ) == nullptr )
        throw std::logic_error( "Internal Error: The internal Playstation font does not have the DEL key symbol!");

    return playstation_font_p;
}
