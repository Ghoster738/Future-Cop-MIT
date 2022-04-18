#include "FontResource.h"

#include "../../Utilities/DataHandler.h"
#include <string.h>
#include <fstream>
#include <cassert>
#include <iostream>

namespace {
    const size_t HEADER_SIZE = 0x20;
    // For some reason the Windows version stores this as a big endian 32 bit number. Mac uses the little endian.
    // I think this format was made for Windows programs, but the Future Cop developers used it for the Mac version.
    const uint32_t FNTP_LITTLE_TAG = 0x50544E46; // which is { 0x50, 0x54, 0x4E, 0x46 } or { 'P', 'T', 'N', 'F' } or "PTNF"

    const size_t GLYPH_SIZE = 0xB; // Contains the character code with parameters.

    const size_t IMAGE_HEADER_SIZE = 0x10;

    // The image data starts after the end of the image header.
}

Data::Mission::FontGlyph::FontGlyph( Utilities::Buffer::Reader& reader ) {
    this->glyphID   = reader.readU8();
    this->unk_0     = reader.readU8(); // Skip a byte
    this->width     = reader.readU8();
    this->height    = reader.readU8();
    this->left      = reader.readU8();
    this->unk_1     = reader.readU8(); // Skip a byte
    this->top       = reader.readU8();
    this->unk_2     = reader.readU8(); // Skip a byte
    this->x_advance = reader.readU8();
    this->offset.x  = reader.readU8();
    this->offset.y  = reader.readU8();
}

uint8_t Data::Mission::FontGlyph::getGlyph() const {
    return glyphID;
}

uint8_t Data::Mission::FontGlyph::getRight() const {
    return this->left + this->width;
}

uint8_t Data::Mission::FontGlyph::getLeft() const {
    return this->left;
}

uint8_t Data::Mission::FontGlyph::getTop() const {
    return this->top;
}

uint8_t Data::Mission::FontGlyph::getBottom() const {
    // This does not make sense!
    // What was I thinking!
    return this->top + this->height;
}

Utilities::DataTypes::Vec2Byte Data::Mission::FontGlyph::getOffset() const {
    return this->offset;
}

uint8_t Data::Mission::FontGlyph::getXAdvance() const {
    return this->x_advance;
}
uint8_t Data::Mission::FontGlyph::getWidth() const {
    return this->width;
}

uint8_t Data::Mission::FontGlyph::getHeight() const {
    return this->height;
}

const std::string Data::Mission::FontResource::FILE_EXTENSION = "fnt";
const uint32_t Data::Mission::FontResource::IDENTIFIER_TAG = 0x43666E74; // which is { 0x43, 0x66, 0x6E, 0x74 } or { 'C', 'f', 'n', 't' } or "Cfnt"

Data::Mission::FontResource::FontResource() {
    for( int i = 0; i < sizeof( font_glyphs_r ) / sizeof( FontGlyph* ); i++ ) {
        font_glyphs_r[ i ] = nullptr;
    }
}

Data::Mission::FontResource::FontResource( const FontResource &obj ) : Resource(obj), image( obj.image ), glyphs( obj.glyphs ) {
    for( unsigned int i = 0; i < MAX_GLYPHS; i++ ) {
        font_glyphs_r[ i ] = obj.font_glyphs_r[ i ];
    }
    /*
    for( int i = 0; i < sizeof( font_glyphs_r ) / sizeof( FontGlyph* ); i++ ) {
        if( obj.font_glyphs_r[ i ] != nullptr )
            font_glyphs_r[ i ] = glyphs.data() + (obj.font_glyphs_r[ i ] - obj.glyphs.data());
        else
            font_glyphs_r[ i ] = nullptr;
    }
    std::cout << "Mission::FontResource::FontResource copied" << std::endl;
    */
}

std::string Data::Mission::FontResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::FontResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

const Data::Mission::FontGlyph *const Data::Mission::FontResource::getGlyph( uint8_t which ) const {
    return font_glyphs_r[ which ];
}

bool Data::Mission::FontResource::parse( const ParseSettings &settings ) {
    if( this->data_p != nullptr )
    {
        auto reader = this->data_p->getReader();

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

            // assert( platform == 9 ); // This statement will not crash on Playstation 1 files.
            // assert( platform == 8 ); // This statement will not crash on Mac or Windows files.

            // Check to see if the data will work.
            file_is_not_valid = false;
            file_is_not_valid |= ( header != FNTP_LITTLE_TAG );
            file_is_not_valid |= ( (number_of_glyphs * GLYPH_SIZE + offset_to_glyphs) > reader.totalSize() );
            file_is_not_valid |= ( (IMAGE_HEADER_SIZE + offset_to_image_header) > reader.totalSize() );

            if( !file_is_not_valid )
            {
                reader.setPosition( offset_to_glyphs, Utilities::Buffer::Reader::BEGINING );

                auto readerGlyphs = reader.getReader( number_of_glyphs * GLYPH_SIZE );

                this->glyphs.reserve( number_of_glyphs );

                for( unsigned int i = 0; i < number_of_glyphs; i++ )
                {
                    auto readerGlyph = readerGlyphs.getReader( GLYPH_SIZE );
                    this->glyphs.push_back( FontGlyph( readerGlyph ) );
                }

                // The reason why this is in a seperate loop is because the vector glyphs would reallocate.
                for( unsigned int i = 0; i != this->glyphs.size(); i++ )
                    font_glyphs_r[ glyphs[i].getGlyph() % MAX_GLYPHS ] = this->glyphs.data() + i;

                reader.setPosition( offset_to_image_header, Utilities::Buffer::Reader::BEGINING );

                auto readerImageHeader = reader.getReader( IMAGE_HEADER_SIZE );

                auto width  = static_cast<uint16_t>( readerImageHeader.readU8() ) * 4;
                readerImageHeader.setPosition( 0x6, Utilities::Buffer::Reader::BEGINING );
                auto height = readerImageHeader.readU16( settings.endian );

                this->image.setWidth( width ); // For some reason they had this value divided by 4.
                this->image.setHeight( height );

                // Set up the image.
                file_is_not_valid |= !image.setFormat( Utilities::ImageData::BLACK_WHITE, 1 );

                // Check to see if the image would go beyond the scope of the raw_data.
                file_is_not_valid |= ( (IMAGE_HEADER_SIZE + offset_to_image_header + (image.getWidth() / 2) * image.getHeight()) > reader.totalSize() );

                if( !file_is_not_valid ) {
                    auto readerImage = reader.getReader( reader.totalSize() - (IMAGE_HEADER_SIZE + offset_to_image_header) );

                    auto image_data = image.getRawImageData();

                    // The pixels seems to be compressed in a certian format.
                    // These combinations are 0xFF, 0xF0, 0x0F, 0x00.
                    // This compression reduces the image size to a half.
                    // Yes, they could have used one bit per pixel, but I think it would increase the loading times.
                    for( unsigned int x = 0; x < image.getHeight() * image.getWidth() / 2; x++ ) {
                        auto pixel_pack = readerImage.readU8();

                        if( (pixel_pack & 0xF0) != 0 ) {
                            *image_data = 0xFF;
                        }
                        else {
                            *image_data = 0x00;
                        }
                        image_data++;

                        if( (pixel_pack & 0x0F) != 0 ) {
                            *image_data = 0xFF;
                        }
                        else {
                            *image_data = 0x00;
                        }
                        image_data++;
                    }

                    return true;
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
    else
        return false;
}

Data::Mission::Resource * Data::Mission::FontResource::duplicate() const {
    return new Data::Mission::FontResource( *this );
}

Utilities::ImageData *const Data::Mission::FontResource::getImage() const {
    return const_cast<Utilities::ImageData *const>(&image);
}

int Data::Mission::FontResource::write( const char *const file_path, const std::vector<std::string> & arguments ) const {
    std::ofstream resource;
    bool export_enable = true;

    for( auto arg = arguments.begin(); arg != arguments.end(); arg++ ) {
        if( (*arg).compare("--dry") == 0 )
            export_enable = false;
    }

    if( export_enable )
    {
        resource.open( std::string(file_path) + "." + getFileExtension(), std::ios::out );

        if( resource.is_open() )
        {
            resource << "info face=\"" << file_path << "\" size=24 bold=0 italic=0 charset=\"ASCII\""
                     << " unicode=0 stretchH=100 smooth=0 aa=1 padding=0,0,0,0 spacing=0,0 outline=1"<< std::endl;

            resource << "common lineHeight=24 base=19 scaleW=" << image.getWidth() << " scaleH="
                     << image.getHeight() << " pages=1 packed=0 alphaChnl=0 redChnl=0 greenChnl=0 blueChnl=0"
                     << std::endl;

            auto file_png = std::string( std::string(file_path), std::string(file_path).find_last_of( '/' ) + 1 );

            resource << "page id=0 file=\"" << file_png << ".png\"" << std::endl;

            resource << "chars count=" << glyphs.size() << std::endl;

            for( unsigned int i = 0; i < glyphs.size(); i++ ) {
                resource << "char id=" << (static_cast<unsigned int>(glyphs.at(i).getGlyph()) & 0xFF)
                         << " x=" << glyphs.at(i).getLeft() << " y=" << glyphs.at(i).getTop()
                         << " width="  << (glyphs.at(i).getRight() - glyphs.at(i).getLeft())
                         << " height=" << (glyphs.at(i).getBottom() - glyphs.at(i).getTop())
                         << " xoffset=" << static_cast<int>( glyphs.at(i).getOffset().x )
                         << " yoffset=" << static_cast<int>( glyphs.at(i).getOffset().y )
                         << " xadvance=" << glyphs.at(i).getXAdvance() << " page=0 chnl=15"
                         << std::endl;
            }

            resource.close();

            std::string file_path_texture = std::string( file_path ) + ".png";
            return image.write( file_path_texture.c_str() );
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
