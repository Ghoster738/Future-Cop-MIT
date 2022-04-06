#include "FontResource.h"

#include "../../Utilities/DataHandler.h"
#include <string.h>
#include <fstream>
#include <cassert>
#include <iostream>

namespace {
    const size_t HEADER_SIZE = 0x20;
    const uint32_t FNTP_TAG = 0x464E5450; // which is { 0x46, 0x4E, 0x54, 0x50 } or { 'F', 'N', 'T', 'P' } or "FNTP"
    const size_t NUMBER_OF_GLYPHS_OFFSET = 0xA;
    const size_t OFFSET_TO_GLYPH_DATA_OFFSET = 0x14;
    const size_t OFFSET_TO_IMAGE_HEADER_OFFSET = 0x1C;

    const size_t FONT_CHARACTER_SIZE = 0xB; // Contains the character code with parameters.

    const size_t IMAGE_HEADER_SIZE = 0x10;
    const size_t IMAGE_HEADER_OFFSET_FOR_WIDTH = 0x0;
    const size_t IMAGE_HEADER_OFFSET_FOR_HEIGHT = 0x6;

    // The image data starts after the end of the image header.
}

Data::Mission::FontGlyph::FontGlyph( const uint8_t *const data, bool swap_endian ) {
    this->glyphID   = Utilities::DataHandler::read_u8( data +  0 );
    this->width     = Utilities::DataHandler::read_u8( data +  2 );
    this->height    = Utilities::DataHandler::read_u8( data +  3 );
    this->left      = Utilities::DataHandler::read_u16( data + 4, swap_endian );
    this->top       = Utilities::DataHandler::read_u16( data + 6, swap_endian );
    this->x_advance = Utilities::DataHandler::read_8( data +  8 );
    this->offset.x  = Utilities::DataHandler::read_8( data +  9 );
    this->offset.y  = Utilities::DataHandler::read_8( data + 10 );
}

uint8_t Data::Mission::FontGlyph::getGlyph() const {
    return glyphID;
}

int Data::Mission::FontGlyph::getRight() const {
    return this->left + this->width;
}

int Data::Mission::FontGlyph::getLeft() const {
    return this->left;
}

int Data::Mission::FontGlyph::getTop() const {
    return this->top;
}

int Data::Mission::FontGlyph::getBottom() const {
    // This does not make sense!
    // What was I thinking!
    return this->top + this->height;
}

Utilities::DataTypes::Vec2Byte Data::Mission::FontGlyph::getOffset() const {
    return this->offset;
}

int Data::Mission::FontGlyph::getXAdvance() const {
    return this->x_advance;
}
int Data::Mission::FontGlyph::getWidth() const {
    return this->width;
}

int Data::Mission::FontGlyph::getHeight() const {
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
    for( int i = 0; i < sizeof( font_glyphs_r ) / sizeof( FontGlyph* ); i++ ) {
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

bool Data::Mission::FontResource::parse( const Utilities::Buffer &header, const Utilities::Buffer &buffer, const ParseSettings &settings ) {
    auto reader = buffer.getReader();

    bool file_is_not_valid;
    uint16_t number_of_glyphs;
    uint32_t offset_to_glyphs;
    uint32_t offset_to_image_header;

    if( reader.totalSize() > HEADER_SIZE )
    {
        auto header = reader.readU32( settings.endian );
        
        // Get the data first
        reader.setPosition( NUMBER_OF_GLYPHS_OFFSET, Utilities::Buffer::Reader::BEGINING );
        number_of_glyphs       = reader.readU16( settings.endian );
        reader.setPosition( OFFSET_TO_GLYPH_DATA_OFFSET, Utilities::Buffer::Reader::BEGINING );
        offset_to_glyphs       = reader.readU32( settings.endian );
        reader.setPosition( OFFSET_TO_IMAGE_HEADER_OFFSET, Utilities::Buffer::Reader::BEGINING );
        offset_to_image_header = reader.readU32( settings.endian );

        // Check to see if the data will work.
        file_is_not_valid = false;
        file_is_not_valid |= ( header == FNTP_TAG );
        file_is_not_valid |= ( (number_of_glyphs * FONT_CHARACTER_SIZE + offset_to_glyphs) > reader.totalSize() );
        file_is_not_valid |= ( (IMAGE_HEADER_SIZE + offset_to_image_header) > reader.totalSize() );

        if( !file_is_not_valid )
        {
            reader.setPosition( offset_to_image_header + IMAGE_HEADER_OFFSET_FOR_WIDTH, Utilities::Buffer::Reader::BEGINING );
            image.setWidth( reader.readU32( settings.endian ) * 4 ); // For some reason they had this value divided by 4.
            reader.setPosition( offset_to_image_header + IMAGE_HEADER_OFFSET_FOR_HEIGHT, Utilities::Buffer::Reader::BEGINING );
            image.setHeight( reader.readU32( settings.endian ) );

            // Set up the image.
            file_is_not_valid |= !image.setFormat( Utilities::ImageData::BLACK_WHITE, 1 );

            // Check to see if the image would go beyond the scope of the raw_data.
            file_is_not_valid |= ( (IMAGE_HEADER_SIZE + offset_to_image_header + (image.getWidth() / 2) * image.getHeight()) > reader.totalSize() );

            if( !file_is_not_valid ) {
                reader.setPosition( offset_to_glyphs, Utilities::Buffer::Reader::BEGINING );
                
                for( unsigned int i = 0; i < number_of_glyphs; i++ )
                {
                    glyphs.push_back( FontGlyph( reader.getBytes( FONT_CHARACTER_SIZE ).data(), settings.is_opposite_endian ) );
                }
                
                // The reason why this is in a seperate loop is because the vector glyphs would reallocate.
                for( unsigned int i = 0; i < number_of_glyphs; i++  ) {
                    font_glyphs_r[ glyphs[i].getGlyph() ] = &glyphs[ i ];
                }

                auto image_data = image.getRawImageData();
                
                reader.setPosition( IMAGE_HEADER_SIZE + offset_to_image_header, Utilities::Buffer::Reader::BEGINING );

                // The pixels seems to be compressed in a certian format.
                // These combinations are 0xFF, 0xF0, 0x0F, 0x00.
                // This compression reduces the image size to a half.
                // Yes, they could have used one bit per pixel, but I think it would increase the loading times.
                for( unsigned int x = 0; x < image.getHeight() * image.getWidth() / 2; x++ ) {
                    auto pixel_pack = reader.readU8();

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
