#include "FontResource.h"

#include "../../Utilities/ImageFormat/Chooser.h"
#include "../../Utilities/ImagePalette2D.h"
#include <string.h>
#include <fstream>
#include <stdexcept>

namespace {
const size_t HEADER_SIZE = 0x20;
// For some reason the Windows version stores this as a big endian 32 bit number. Mac uses the little endian.
// I think this format was made for Windows programs, but the Future Cop developers used it for the Mac version.
const uint32_t FNTP_LITTLE_TAG = 0x50544E46; // which is { 0x50, 0x54, 0x4E, 0x46 } or { 'P', 'T', 'N', 'F' } or "PTNF"

const size_t GLYPH_SIZE = 0xB; // Contains the character code with parameters.

const size_t IMAGE_HEADER_SIZE = 0x10;

const uint8_t MISSING_SYMBOL_PRIORITY[ Data::Mission::FontResource::MAX_GLYPHS ] = { 0xca, 0xd0, 0xf0, 0xe5, 0xdc, 0xd4, 0xf2, 0xe0, 0xc9, 0xe2, 0xfb, 0xbf, 0xc5, 0xcd, 0xd1, 0xe7, 0xc6, 0xeb, 0xf7, 0xcc, 0xee, 0xc7, 0xd5, 0xcb, 0xfe, 0xbe, 0xcf, 0xed, 0xe8, 0xd6, 0xef, 0xdd, 0xd7, 0xb9, 0x99, 0x3, 0x9a, 0x5, 0x8, 0x4c, 0x34, 0x9f, 0x4, 0x1f, 0xba, 0x33, 0x74, 0x2b, 0xb4, 0x77, 0xab, 0xb, 0xa5, 0x4b, 0x72, 0x9b, 0xb0, 0x51, 0x32, 0x4e, 0x42, 0xbc, 0x54, 0x1, 0x2, 0x76, 0xa6, 0x25, 0x8c, 0x13, 0xb8, 0x78, 0x1b, 0x28, 0x65, 0x29, 0x7a, 0x11, 0xa3, 0x12, 0x81, 0x49, 0x8b, 0x50, 0x17, 0x3b, 0x4a, 0x7b, 0x40, 0xaa, 0x68, 0x83, 0x2d, 0x10, 0x7, 0x47, 0x46, 0xb2, 0x14, 0x3a, 0x59, 0x26, 0xd, 0x9d, 0x6a, 0xb7, 0x1a, 0x18, 0x57, 0x84, 0x7c, 0x95, 0x8a, 0x62, 0x82, 0xa9, 0x15, 0x92, 0x70, 0x79, 0x6e, 0xad, 0x98, 0x6c, 0x48, 0x52, 0x6, 0x0, 0xf6, 0xf9, 0xc1, 0xf3, 0xe3, 0xfd, 0xd8, 0xe1, 0xf8, 0xd9, 0xf1, 0xc3, 0xd2, 0xe9, 0xe4, 0xce, 0xda, 0xff, 0xf5, 0xfa, 0xd3, 0xdf, 0xc2, 0xc4, 0xf4, 0xe6, 0xbd, 0xc0, 0xde, 0xc8, 0xea, 0xec, 0xfc, 0x71, 0x9, 0x87, 0xae, 0x80, 0x90, 0x88, 0xac, 0x97, 0x7d, 0x9e, 0x55, 0xdb, 0x23, 0x66, 0x69, 0x53, 0x1d, 0x1e, 0xa8, 0xb6, 0x4d, 0xb3, 0x37, 0x7e, 0x5e, 0xa1, 0x3f, 0x2e, 0x96, 0x64, 0x5f, 0x30, 0x16, 0x60, 0x35, 0x2a, 0x5c, 0x22, 0xaf, 0x38, 0x36, 0x20, 0x89, 0x31, 0xb5, 0xf, 0x94, 0xbb, 0x6f, 0x1c, 0x5d, 0xa4, 0x44, 0x6d, 0x8e, 0x21, 0x73, 0x56, 0x43, 0x3c, 0xa7, 0x6b, 0x5b, 0xc, 0x67, 0xa0, 0x5a, 0x3e, 0xa2, 0x27, 0xe, 0x8f, 0x93, 0x86, 0x2f, 0x85, 0x2c, 0x7f, 0x9c, 0x8d, 0x75, 0x61, 0x41, 0x3d, 0x45, 0xa, 0x19, 0x24, 0x39, 0x4f, 0x63, 0xb1, 0x91, 0x58, };

#include "Embedded/PFNT.h"
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

const std::filesystem::path Data::Mission::FontResource::FILE_EXTENSION = "fnt";
const uint32_t              Data::Mission::FontResource::IDENTIFIER_TAG = 0x43666E74; // which is { 0x43, 0x66, 0x6E, 0x74 } or { 'C', 'f', 'n', 't' } or "Cfnt"

Data::Mission::FontResource::FontResource() : image_p( nullptr ), height(0), missing_char_symbol(0x7F) {
    for( unsigned int i = 0; i < MAX_GLYPHS; i++ ) {
        font_glyphs_r[ i ] = nullptr;
    }
}

Data::Mission::FontResource::FontResource( const FontResource &obj ) : Resource(obj), image_p( nullptr ), height( obj.height ), missing_char_symbol( obj.missing_char_symbol ), glyphs( obj.glyphs ) {
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

std::filesystem::path Data::Mission::FontResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::FontResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

const Data::Mission::FontResource::Glyph *const Data::Mission::FontResource::getGlyph( uint8_t which ) const {
    return font_glyphs_r[ which ];
}

Data::Mission::FontResource::FilterStatus Data::Mission::FontResource::filterText( const std::string& unfiltered_text, std::string *filtered_text_r ) const {
    FilterStatus filter_status = FilterStatus::PERFECT;

    // Clear the filtered text.
    if( filtered_text_r != nullptr ) {
        *filtered_text_r = "";
    }

    for( auto single_character : unfiltered_text ) {

        if( getGlyph( single_character ) == nullptr ) {
            filter_status = FilterStatus::CULLED;

            if( filtered_text_r != nullptr )
                filtered_text_r->push_back( missing_char_symbol ); // This character does not exist.
        }
        else
        if( filtered_text_r != nullptr ) {
            filtered_text_r->push_back( single_character ); // The character is valid.
        }
    }

    return filter_status;
}

uint32_t Data::Mission::FontResource::getLineLength( const std::string &filtered_text ) const {
    uint32_t line_length = 0;

    for( auto single_character : filtered_text ) {
        auto glyph_r = getGlyph( single_character );

        if( glyph_r != nullptr ) {
            line_length += glyph_r->x_advance;
        }
    }

    return line_length;
}

bool Data::Mission::FontResource::parse( const ParseSettings &settings ) {
    auto debug_log = settings.logger_r->getLog( Utilities::Logger::DEBUG );
    debug_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";
    auto warning_log = settings.logger_r->getLog( Utilities::Logger::WARNING );
    warning_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

    if( this->data != nullptr )
    {
        auto reader = this->getDataReader();

        bool file_is_not_valid;

        if( reader.totalSize() > HEADER_SIZE )
        {
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

            debug_log.output << "  Header = 0x" << std::hex << header << std::dec << "\n"
                << "  Tag Size = 0x" << std::hex << tag_size << std::dec << "\n"
                << "  u16_100 = " << u16_100 << "\n"
                << "  number_of_glyphs = " << number_of_glyphs << "\n"
                << "  platform = " << platform << "\n"
                << "  unk_u8 height? = " << static_cast<uint32_t>( unk_u8 ) << "\n"
                << "  offset_to_glyphs = 0x" << std::hex << offset_to_glyphs << std::dec << "\n"
                << "  offset_to_image_header = 0x" << std::hex << offset_to_image_header << std::dec << "\n";

            // assert( platform == 9 ); // This statement will not crash on Playstation 1 files.
            // assert( platform == 8 ); // This statement will not crash on Mac or Windows files.

            // Check to see if the data will work.
            file_is_not_valid = false;
            file_is_not_valid |= ( header != FNTP_LITTLE_TAG );
            file_is_not_valid |= ( (number_of_glyphs * GLYPH_SIZE + offset_to_glyphs) > reader.totalSize() );
            file_is_not_valid |= ( (IMAGE_HEADER_SIZE + offset_to_image_header) > reader.totalSize() );

            if( !file_is_not_valid )
            {
                debug_log.output << "FNT Header is valid!" << "\n";

                reader.setPosition( offset_to_glyphs, Utilities::Buffer::BEGIN );

                auto readerGlyphs = reader.getReader( number_of_glyphs * GLYPH_SIZE );

                this->glyphs.reserve( number_of_glyphs );

                uint16_t priority = MAX_GLYPHS;
                this->missing_char_symbol = 0x7F;

                // This is used to gather the height of the font.
                int32_t lower_font_y  =  std::numeric_limits<int32_t>::max();
                int32_t higher_font_y = -std::numeric_limits<int32_t>::max();

                for( unsigned int i = 0; i < number_of_glyphs; i++ )
                {
                    auto readerGlyph = readerGlyphs.getReader( GLYPH_SIZE );
                    auto glyph = Glyph( readerGlyph );

                    this->glyphs.push_back( glyph );

                    // Choose the missing_char_symbol.
                    if( priority > MISSING_SYMBOL_PRIORITY[ glyph.glyphID ] ) {
                        priority = MISSING_SYMBOL_PRIORITY[ glyph.glyphID ];
                        this->missing_char_symbol = glyph.glyphID;
                    }

                    // For every Glyph execpt for space.
                    if( glyph.glyphID != 0x20 ) {
                        // Gather the height information.
                        lower_font_y  = std::min( static_cast<int32_t>( glyph.offset.y ), lower_font_y );
                        higher_font_y = std::max( lower_font_y + static_cast<int32_t>( glyph.height ), higher_font_y );
                    }
                }

                // Finally calculate the height.
                this->height = higher_font_y - lower_font_y;

                // The reason why this is in a seperate loop is because the vector glyphs would reallocate.
                for( auto i = this->glyphs.cbegin(); i < this->glyphs.cend(); i++ ) {
                    const uint32_t wrapped_character = (*i).glyphID % MAX_GLYPHS;

                    debug_log.output << "  glyphs[" << (i - this->glyphs.cbegin()) << "] = " << wrapped_character << "\n";

                    font_glyphs_r[ wrapped_character ] = this->glyphs.data() + (i - this->glyphs.cbegin());
                }

                reader.setPosition( offset_to_image_header, Utilities::Buffer::BEGIN );

                auto readerImageHeader = reader.getReader( IMAGE_HEADER_SIZE );

                const auto font_image_identifier = readerImageHeader.readU8();

                if( font_image_identifier != '@' )
                    warning_log.output << "font_image_identifier is not @.\n";

                readerImageHeader.setPosition( 0x4, Utilities::Buffer::BEGIN );
                auto width  = readerImageHeader.readU16( settings.endian );
                auto height = readerImageHeader.readU16( settings.endian );

                debug_log.output << "width = " << width << ", height = " << height << "\n";


                // The pixels are compressed with 4 bit color palettes.
                // Playstation did not support 1 bit, so their best option is 4 bit.
                Utilities::ColorPalette color_palette( Utilities::PixelFormatColor_W8A8::linear );
                
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
                    debug_log.output << "FNT image is valid.\n";

                    auto reader_image = reader.getReader( reader.totalSize() - (IMAGE_HEADER_SIZE + offset_to_image_header) );
                    
                    auto bits = reader_image.getBitfield();
                    
                    image_palette.fromBitfield( bits, 4 );
                    
                    if( image_p != nullptr )
                        delete image_p;
                    
                    auto backup = image_palette.toColorImage();
                    
                    image_p = new Utilities::Image2D( backup );

                    if( image_p == nullptr ) {
                        debug_log.output << "  FNT image failed\n" << std::endl;
                    }


                    // If the image fails to allocate then it failed.
                    return image_p != nullptr;
                }
                else
                {
                    debug_log.output << "  FNT image is invalid\n" << std::endl;
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

int Data::Mission::FontResource::write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    std::ofstream resource;
    Utilities::ImageFormat::Chooser chooser;

    if( iff_options.font.shouldWrite( iff_options.enable_global_dry_default ) ) {
        std::filesystem::path full_file_path = file_path;
        full_file_path += ".";
        full_file_path += this->getFileExtension();

        resource.open( full_file_path, std::ios::out );

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

Data::Mission::FontResource* Data::Mission::FontResource::getWindows( uint32_t resource_id, Utilities::Logger *logger_r ) {
    Data::Mission::FontResource* windows_font_p = new Data::Mission::FontResource;

    windows_font_p->setIndexNumber( 0 );
    windows_font_p->setMisIndexNumber( 0 );
    windows_font_p->setResourceID( resource_id );

    auto loading = Utilities::Buffer::Reader( windows_fnt, windows_fnt_len );

    windows_font_p->read( loading );

    Data::Mission::Resource::ParseSettings parse_settings;
    parse_settings.endian = Utilities::Buffer::LITTLE;
    parse_settings.logger_r = logger_r;

    if( !windows_font_p->parse( parse_settings ) )
        throw std::logic_error( "Internal Error: The internal Windows font has failed to parse!");

    // DEL symbol required for internal fonts!
    if( windows_font_p->getGlyph( 0x7F ) == nullptr )
        throw std::logic_error( "Internal Error: The internal Windows font does not have the DEL key symbol!");

    return windows_font_p;
}

Data::Mission::FontResource* Data::Mission::FontResource::getPlaystation( uint32_t resource_id, Utilities::Logger *logger_r ) {
    Data::Mission::FontResource* playstation_font_p = new Data::Mission::FontResource;

    playstation_font_p->setIndexNumber( 0 );
    playstation_font_p->setMisIndexNumber( 0 );
    playstation_font_p->setResourceID( resource_id );

    auto loading = Utilities::Buffer::Reader( playstation_fnt, playstation_fnt_len );

    playstation_font_p->read( loading );

    Data::Mission::Resource::ParseSettings parse_settings;
    parse_settings.endian = Utilities::Buffer::LITTLE;
    parse_settings.logger_r = logger_r;

    if( !playstation_font_p->parse( parse_settings ) )
        throw std::logic_error( "Internal Error: The internal Playstation font has failed to parse!");

    // DEL symbol required for internal fonts!
    if( playstation_font_p->getGlyph( 0x7F ) == nullptr )
        throw std::logic_error( "Internal Error: The internal Playstation font does not have the DEL key symbol!");

    return playstation_font_p;
}

std::string Data::Mission::FontResource::getValidCharacters() const {
    std::string valid_characters;

    for( uint32_t i = 0; i < MAX_GLYPHS; i++ ) {
        if( this->font_glyphs_r[ i ] != nullptr )
            valid_characters += (char)i;
    }

    return valid_characters;
}

bool Data::Mission::IFFOptions::FontOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {
    return IFFOptions::ResourceOption::readParams( arguments, output_r );
}

std::string Data::Mission::IFFOptions::FontOption::getOptions() const {
    std::string information_text = getBuiltInOptions();

    return information_text;
}
