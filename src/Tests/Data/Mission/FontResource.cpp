#include "../../../Data/Mission/FontResource.h"
#include "../../../Utilities/ImageFormat/QuiteOkImage.h"
#include <iostream>
#include "../../Utilities/TestImage2D.h"
#include <set>
#include "Embedded/PFNT.h"
#include "Embedded/PFNTExpected.h"

Data::Mission::FontResource* getMacintosh( uint32_t resource_id, Utilities::Logger *logger_r ) {
    Data::Mission::FontResource* macintosh_font_p = new Data::Mission::FontResource;

    macintosh_font_p->setIndexNumber( 0 );
    macintosh_font_p->setMisIndexNumber( 0 );
    macintosh_font_p->setResourceID( 2 );

    auto loading = Utilities::Buffer::Reader( macintosh_fnt, macintosh_fnt_len );

    macintosh_font_p->read( loading );

    Data::Mission::Resource::ParseSettings parse_settings;
    parse_settings.type = Data::Mission::Resource::ParseSettings::Macintosh;
    parse_settings.endian = Utilities::Buffer::BIG;
    parse_settings.logger_r = logger_r;

    if( !macintosh_font_p->parse( parse_settings ) )
        throw std::logic_error( "Internal Error: The internal Mac font has failed to parse!");

    // DEL symbol required for internal fonts!
    if( macintosh_font_p->getGlyph( 0x7F ) == nullptr )
        throw std::logic_error( "Internal Error: The internal Mac font does not have the DEL key symbol!");

    return macintosh_font_p;
}

int checkImage( const Data::Mission::FontResource &font, size_t qoi_len,
               const uint8_t *const qoi_r, std::string name = "N/A" ) {
    int problem = 0;
    Utilities::ImageFormat::QuiteOkImage qoi_reader;
    
    Utilities::Image2D expected_image( 0, 0, Utilities::PixelFormatColor_W8() );
    
    Utilities::Buffer expected_buffer( qoi_r, qoi_len );
    
    if( qoi_reader.read( expected_buffer, expected_image ) <= 0 ) {
        std::cout << name <<  ": There is something wrong with QOI reading not this module itself!" << std::endl;
        problem = 1;
    }
    
    problem |= compareImage2D<Utilities::Image2D>( *font.getImage(), expected_image, name );
    
    return problem;
}

struct GlyphChecker {
    uint8_t glyphID;
    glm::i8vec2 offset;
    uint8_t width;
    uint8_t height;
    uint8_t left;
    uint8_t top;
    uint8_t x_advance;
    
    GlyphChecker() : glyphID( 0 ), offset(0,0) {}
    GlyphChecker( uint8_t glyphID_param ) : glyphID( glyphID_param ), offset(0,0) {}
    GlyphChecker( uint8_t glyphID_param, uint8_t width_param, uint8_t height_param, uint8_t left_param, uint8_t top_param, uint8_t x_advance_param, int8_t offset_x = 0, int8_t offset_y = 0 ) : glyphID( glyphID_param ), offset(offset_x, offset_y), width( width_param ), height( height_param ),left( left_param ), top( top_param ), x_advance( x_advance_param ) {}
    
    int check( const Data::Mission::FontResource::Glyph &glyph, std::string name ) const {
        int problem = 0;
        
        if( glyph.glyphID != glyphID ) {
            std::cout << name << " has a hashing bug detected with " << (uint16_t)glyphID << " which is '" << glyphID << "'\n";
            std::cout << (uint16_t)glyph.glyphID << " is placed in " << glyphID << std::endl;
            
            problem = 1;
        }
        if( glyph.width != width || glyph.height != height )
        {
            std::cout << name << " has a dimensions problem with " << (uint16_t)glyphID << "\n";
            std::cout << "  width of " << (uint16_t)glyph.width << " is not " << (uint16_t)width << std::endl;
            std::cout << "  height of " << (uint16_t)glyph.height << " is not " << (uint16_t)height << std::endl;
            
            problem = 1;
            
        }
        if( glyph.left != left || glyph.top != top )
        {
            std::cout << name << " has a placement problem with " <<  (uint16_t)glyphID << "\n";
            std::cout << "  left of " << (uint16_t)glyph.left << " is not " << (uint16_t)left << std::endl;
            std::cout << "  top of " << (uint16_t)glyph.top << " is not " << (uint16_t)top << std::endl;
            
            problem = 1;
            
        }
        if( glyph.x_advance != x_advance )
        {
            std::cout << name << " has a x advance problem with " << (uint16_t)glyphID << "\n";
            std::cout << " " << (uint16_t)glyph.x_advance << " is not " << (uint16_t)x_advance << std::endl;
            
            problem = 1;
        }
        if( glyph.offset != offset )
        {
            std::cout << name << " has an offset with " << (uint16_t)glyphID << "\n";
            std::cout << " (" << (int16_t)glyph.offset.x << ", " << (int16_t)glyph.offset.y << ") while it should be (" << (int16_t)offset.x << ", " << (int16_t)offset.y << ")" << std::endl;

            problem = 1;
        }

        if( problem ) {
            std::cout << "  charcode " << (uint16_t)glyphID << " = " << glyphID << std::endl;
        }
        
        return problem;
    }
};

inline bool operator<( const GlyphChecker& A, const GlyphChecker& B ) {
    return A.glyphID < B.glyphID;
}

int checkFont( const Data::Mission::FontResource &font, const std::set<GlyphChecker> &expected_glyphs, std::string name ) {
    uint16_t character_amount = 0;
    int character_problem = 0;

    for( uint16_t i = 0; i < 0x100; i++ ) {
        auto glyph_r = font.getGlyph( i );

        if( glyph_r != nullptr ) {
            character_amount++;

            if( character_problem == 0 ) {
                auto accessor = expected_glyphs.find( i );

                if( accessor == expected_glyphs.end() ) {
                    character_problem = 1;

                    std::cout << name << " has a glyph that is not in the test with " << i << " or '" << (char)i << "'" << std::endl;
                }
                else if( (*accessor).check( *glyph_r, name ) == 1 )
                {
                    character_problem = 1;
                }
            }
        }
    }

    if( character_amount != expected_glyphs.size() ) {
        character_problem = 1;

        std::cout << name << " the expected glyphs had expected " << expected_glyphs.size() << " while the actual amount is " << character_amount << std::endl;
    }

    return character_problem;
}

std::set<GlyphChecker> generateComputerFont() {
    std::set<GlyphChecker> glyphs;

    //                           code, width, height, left, top, x advance, of x, of y
    glyphs.insert( GlyphChecker(  '0',     7,      8,    0,   1,         8 ) );
    glyphs.insert( GlyphChecker(  '1',     3,      8,  248,   1,         8,    3 ) );
    glyphs.insert( GlyphChecker(  '2',     7,      8,    8,   1,         8 ) );
    glyphs.insert( GlyphChecker(  '3',     7,      8,   16,   1,         8 ) );
    glyphs.insert( GlyphChecker(  '4',     7,      8,   24,   1,         8 ) );
    glyphs.insert( GlyphChecker(  '5',     7,      8,   32,   1,         8 ) );
    glyphs.insert( GlyphChecker(  '6',     7,      8,   40,   1,         8 ) );
    glyphs.insert( GlyphChecker(  '7',     7,      8,   48,   1,         8 ) );
    glyphs.insert( GlyphChecker(  '8',     7,      8,   56,   1,         8 ) );
    glyphs.insert( GlyphChecker(  '9',     7,      8,   64,   1,         8 ) );

    glyphs.insert( GlyphChecker(  ' ',     1,      1,  254,  23,         8 ) );
    glyphs.insert( GlyphChecker(  '@',     7,      8,   72,   1,         8 ) );
    glyphs.insert( GlyphChecker(  '#',     7,      8,   80,   1,         8 ) );
    glyphs.insert( GlyphChecker(  '$',     7,      8,   88,   1,         8 ) );
    glyphs.insert( GlyphChecker(  '%',     7,      8,   96,   1,         8 ) );
    glyphs.insert( GlyphChecker(  '&',     7,      8,  104,   1,         8 ) );
    glyphs.insert( GlyphChecker(  '*',     5,      5,  100,  19,         8,    1,    1 ) );
    glyphs.insert( GlyphChecker(  '?',     7,      8,  200,   1,         8 ) );
    glyphs.insert( GlyphChecker(  ';',     2,      6,  181,  11,         8,    3,    1 ) );
    glyphs.insert( GlyphChecker(  ':',     1,      5,  143,  19,         8,    3,    1 ) );
    glyphs.insert( GlyphChecker(  ',',     2,      2,   97,  19,         8,    2,    6 ) );
    glyphs.insert( GlyphChecker(  '.',     1,      1,   95,  19,         8,    3,    7 ) );
    glyphs.insert( GlyphChecker(  '!',     1,      8,  242,   1,         8,    3 ) );
    glyphs.insert( GlyphChecker(  '-',     5,      1,  137,  21,         8,    1,    3 ) );
    glyphs.insert( GlyphChecker(  '_',     7,      1,  123,  19,         8,    0,    7 ) );
    glyphs.insert( GlyphChecker(  '+',     5,      5,  131,  19,         8,    1,    1 ) );
    glyphs.insert( GlyphChecker(  '=',     7,      4,  224,  19,         8,    2,    2 ) );
    glyphs.insert( GlyphChecker(  '^',     3,      2,  119,  19,         8,    2 ) );
    glyphs.insert( GlyphChecker(  '`',     3,      3,  177,  10,         8,    2 ) );
    glyphs.insert( GlyphChecker(  '~',     5,      3,  113,  19,         8,    2 ) );

    glyphs.insert( GlyphChecker(  'a',     7,      5,  176,   4,         8,    0,    3 ) );
    glyphs.insert( GlyphChecker(  'b',     7,      8,  112,   1,         8 ) );
    glyphs.insert( GlyphChecker(  'c',     6,      5,  145,  19,         8,    0,    3 ) );
    glyphs.insert( GlyphChecker(  'd',     7,      8,  120,   1,         8 ) );
    glyphs.insert( GlyphChecker(  'e',     7,      5,  152,  19,         8,    0,    3 ) );
    glyphs.insert( GlyphChecker(  'f',     7,      8,  128,   1,         8 ) );
    glyphs.insert( GlyphChecker(  'g',     7,      8,  136,   1,         8,    0,    3 ) );
    glyphs.insert( GlyphChecker(  'h',     7,      8,  144,   1,         8 ) );
    glyphs.insert( GlyphChecker(  'i',     1,      8,  252,   1,         8,    3 ) );
    glyphs.insert( GlyphChecker(  'j',     3,      8,  244,   1,         8,    2,    1 ) );
    glyphs.insert( GlyphChecker(  'k',     7,      8,  152,   1,         8 ) );
    glyphs.insert( GlyphChecker(  'l',     2,      8,  239,   1,         8,    3 ) );
    glyphs.insert( GlyphChecker(  'm',     7,      5,  160,  19,         8,    0,    3 ) );
    glyphs.insert( GlyphChecker(  'n',     7,      5,  168,  19,         8,    0,    3 ) );
    glyphs.insert( GlyphChecker(  'o',     7,      5,  184,   4,         8,    0,    3 ) );
    glyphs.insert( GlyphChecker(  'p',     7,      8,  160,   1,         8,    0,    3 ) );
    glyphs.insert( GlyphChecker(  'q',     7,      8,  168,   1,         8,    0,    3 ) );
    glyphs.insert( GlyphChecker(  'r',     7,      5,  176,  19,         8,    0,    3 ) );
    glyphs.insert( GlyphChecker(  's',     7,      5,  184,  19,         8,    0,    3 ) );
    glyphs.insert( GlyphChecker(  't',     4,      8,  231,   1,         8,    2 ) );
    glyphs.insert( GlyphChecker(  'u',     7,      5,  192,   4,         8,    0,    3 ) );
    glyphs.insert( GlyphChecker(  'v',     7,      5,  192,  19,         8,    0,    3 ) );
    glyphs.insert( GlyphChecker(  'w',     7,      5,  200,  19,         8,    0,    3 ) );
    glyphs.insert( GlyphChecker(  'x',     7,      5,  208,  19,         8,    0,    3 ) );
    glyphs.insert( GlyphChecker(  'y',     7,      8,  161,  10,         8,    0,    3 ) );
    glyphs.insert( GlyphChecker(  'z',     7,      5,  216,  19,         8,    0,    3 ) );

    glyphs.insert( GlyphChecker( '\'',     1,      2,  111,  19,         8,    3 ) );
    glyphs.insert( GlyphChecker(  '"',     3,      2,  106,  19,         8,    2 ) );

    glyphs.insert( GlyphChecker(  'A',     7,      8,  232,  13,         8 ) );
    uint8_t offset = 1;
    for( uint8_t c = 'A' + 1; c < 'I'; c++ ) {
        glyphs.insert( GlyphChecker(  c,   7,      8, offset, 10,         8 ) );
        offset += 8;
    }
    glyphs.insert( GlyphChecker(  'I',     3,      8,  184,  10,         8,    2 ) );
    glyphs.insert( GlyphChecker(  'J',     5,      8,  188,  10,         8,    1 ) );
    glyphs.insert( GlyphChecker(  'K',     7,      8,   57,  10,         8 ) );
    glyphs.insert( GlyphChecker(  'L',     5,      8,  194,  10,         8,    1 ) );
    glyphs.insert( GlyphChecker(  'M',     7,      8,   65,  10,         8 ) );
    glyphs.insert( GlyphChecker(  'N',     7,      8,   73,  10,         8 ) );
    glyphs.insert( GlyphChecker(  'O',     7,      8,  240,  13,         8 ) );
    offset = 81;
    for( uint8_t c = 'P'; c < 'U'; c++ ) {
        glyphs.insert( GlyphChecker(  c,   7,      8, offset, 10,         8 ) );
        offset += 8;
    }
    glyphs.insert( GlyphChecker(  'U',     7,      8,  248,  13,         8 ) );
    offset = 121;
    for( uint8_t c = 'U' + 1; c < 'Z' + 1; c++ ) {
        glyphs.insert( GlyphChecker(  c,   7,      8, offset, 10,         8 ) );
        offset += 8;
    }

    glyphs.insert( GlyphChecker( '\\',     7,      7,  200,  11,         8 ) );
    glyphs.insert( GlyphChecker(  '|',     1,      8,  236,   1,         8,    3 ) );
    glyphs.insert( GlyphChecker(  '/',     7,      7,  224,  11,         8 ) );

    glyphs.insert( GlyphChecker(  '<',     7,      7,  208,  11,         8 ) );
    glyphs.insert( GlyphChecker(  '>',     7,      7,  216,  11,         8 ) );

    glyphs.insert( GlyphChecker(  '(',     3,      8,  223,   1,         8,    2 ) );
    glyphs.insert( GlyphChecker(  ')',     3,      8,  227,   1,         8,    2 ) );

    glyphs.insert( GlyphChecker(  '[',     2,      8,  209,   1,         8,    2 ) );
    glyphs.insert( GlyphChecker(  ']',     2,      8,  212,   1,         8,    2 ) );

    glyphs.insert( GlyphChecker(  '{',     3,      7,  215,   1,         8,    1,    1 ) );
    glyphs.insert( GlyphChecker(  '}',     3,      7,  219,   1,         8,    1,    1 ) );

    // This is the symbol used in this project to indicate character not found.
    glyphs.insert( GlyphChecker(  0x7F,     7,      8,  169,  10,         8 ) );

    // These are ISO-8859-1 codes that go beyond 127. Those characters might not display correctly on the terminal outputs.
    glyphs.insert( GlyphChecker(  0xC4,     7,     11,  232,  10,         8,   0,   -3 ) );
    glyphs.insert( GlyphChecker(  0xE4,     7,      7,  176,   2,         8,   0,    1 ) );
    glyphs.insert( GlyphChecker(  0xD6,     7,     11,  240,  10,         8,   0,   -3 ) );
    glyphs.insert( GlyphChecker(  0xF6,     7,      7,  184,   2,         8,   0,    1 ) );
    glyphs.insert( GlyphChecker(  0xDC,     7,     11,  248,  10,         8,   0,   -3 ) );
    glyphs.insert( GlyphChecker(  0xFC,     7,      7,  192,   2,         8,   0,    1 ) );

    return glyphs;
}

int main() {
    int problem = 0;
    
    {
        auto windows_font_r = Data::Mission::FontResource::getWindows( 2, &Utilities::logger );
        std::string name = "Windows";
        
        if( windows_font_r == nullptr ) {
            std::cout << name << " font does not exist!" << std::endl;
            return 1;
        }
        if( windows_font_r->getImage() == nullptr ) {
            std::cout << name << " font lacks an image!" << std::endl;
            return 1;
        }
        
        problem |= checkImage( *windows_font_r, computer_qoi_len, computer_qoi, name );

        auto expected_glyphs = generateComputerFont();

        problem |= checkFont(*windows_font_r, expected_glyphs, name);
        
        delete windows_font_r;
    }
    
    {
        auto mac_font_r = getMacintosh( 3, &Utilities::logger );
        std::string name = "Macintosh";
        
        if( mac_font_r == nullptr ) {
            std::cout << name << " font does not exist!" << std::endl;
            return 1;
        }
        if( mac_font_r->getImage() == nullptr ) {
            std::cout << name << " font lacks an image!" << std::endl;
            return 1;
        }
        
        problem |= checkImage( *mac_font_r, computer_qoi_len, computer_qoi, name );

        auto expected_glyphs = generateComputerFont();

        problem |= checkFont(*mac_font_r, expected_glyphs, name);
        
        delete mac_font_r;
    }
    
    {
        auto ps1_font_r = Data::Mission::FontResource::getPlaystation( 1, &Utilities::logger );
        std::string name = "Playstation";
        
        if( ps1_font_r == nullptr ) {
            std::cout << name << " font does not exist!" << std::endl;
            return 1;
        }
        if( ps1_font_r->getImage() == nullptr ) {
            std::cout << name << " font lacks an image!" << std::endl;
            return 1;
        }
        
        problem |= checkImage( *ps1_font_r, playstation_qoi_len, playstation_qoi, name );
        
        std::set<GlyphChecker> expected_glyphs;
        
        const GlyphChecker glyph_default = { 0, 5, 5, 70, 7, 6};
        
        // -- Start of checking PS1 characters to test here.
        {
            GlyphChecker glyph = glyph_default;
            
            glyph.glyphID = ' ';
            glyph.left = 216;
            glyph.top = 7;
            
            expected_glyphs.insert( glyph );
        }
        
        for( uint8_t i = 'A'; i <= 'Z'; i++ )
        {
            GlyphChecker glyph = glyph_default;
            
            glyph.glyphID = i;
            glyph.left = 6 * (i - 'A');
            glyph.top = 1;
            
            expected_glyphs.insert( glyph );
        }
        
        for( uint8_t i = '0'; i <= '9'; i++ )
        {
            GlyphChecker glyph = glyph_default;
            
            glyph.glyphID = i;
            glyph.left = 156 + 6 * (i - '0');
            glyph.top = 1;
            
            expected_glyphs.insert( glyph );
        }
        
        for( uint8_t i = 'a'; i <= 'z'; i++ )
        {
            GlyphChecker glyph = glyph_default;
            
            glyph.glyphID = i;
            glyph.left = 6 * (i - 'a');
            glyph.top = 1;
            
            expected_glyphs.insert( glyph );
        }
        
        {
            GlyphChecker glyph = glyph_default;
            
            glyph.glyphID = '!';
            glyph.left = 216;
            glyph.top = 1;
            
            expected_glyphs.insert( glyph );
        }
        
        {
            GlyphChecker glyph = glyph_default;
            
            glyph.glyphID = '%';
            glyph.left = 234;
            glyph.top = 1;
            
            expected_glyphs.insert( glyph );
        }
        
        {
            GlyphChecker glyph = glyph_default;
            
            glyph.glyphID = '&';
            glyph.left = 246;
            glyph.top = 1;
            
            expected_glyphs.insert( glyph );
        }
        
        {
            GlyphChecker glyph = glyph_default;
            
            glyph.glyphID = '&';
            glyph.left = 246;
            
            expected_glyphs.insert( glyph );
        }
        
        const uint8_t row_7[] = { '-', '_', '+', '=', '{', '}', '[', ']', ':', ';', '\'', '"', '<', '>', ',', '.', '?', '\\', '|', '/'};
        
        for( unsigned i = 0; i < sizeof(row_7) / sizeof(row_7[0]); i++ )
        {
            GlyphChecker glyph = glyph_default;
            
            glyph.glyphID = row_7[ i ];
            glyph.left = 6 * i;
            
            expected_glyphs.insert( glyph );
        }
        
        const uint8_t half_row_7[] = { '`', '(', ')', 0x7F };
        
        for( unsigned i = 0; i < sizeof(half_row_7) / sizeof(half_row_7[0]); i++ )
        {
            GlyphChecker glyph = glyph_default;
            
            glyph.glyphID = half_row_7[ i ];
            glyph.left = 126 + 6 * i;
            
            expected_glyphs.insert( glyph );
        }
        
        const uint8_t german_character_upper[] = { 0xC4, 0xD6, 0xDC };
        
        for( unsigned i = 0; i < sizeof(german_character_upper) / sizeof(german_character_upper[0]); i++ )
        {
            GlyphChecker glyph = glyph_default;
            
            glyph.glyphID = german_character_upper[ i ];
            glyph.left = 198 + 6 * i;
            
            expected_glyphs.insert( glyph );
            
            // Add the lower cases
            glyph.glyphID = german_character_upper[ i ] | 0x20;
            expected_glyphs.insert( glyph );
        }
        
        const uint8_t ps1_symbols[] = { 35, 126, 64, 42 };
        
        for( unsigned i = 0; i < sizeof(ps1_symbols) / sizeof(ps1_symbols[0]); i++ )
        {
            GlyphChecker glyph = glyph_default;
            
            glyph.glyphID = ps1_symbols[ i ];
            glyph.left = 174 + 6 * i;
            
            expected_glyphs.insert( glyph );
        }
        
        {
            GlyphChecker direction = glyph_default;
            
            direction.glyphID = 36;
            direction.width = 11;
            direction.left = 162;
            direction.x_advance = 12;
            expected_glyphs.insert( direction );
            
            direction.glyphID = 94;
            direction.left = 150;
            expected_glyphs.insert( direction );
        }

        problem |= checkFont(*ps1_font_r, expected_glyphs, name);
        
        delete ps1_font_r;
    }
    
    return problem;
}

