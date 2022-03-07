#include "PTCResource.h"

#include "../../Utilities/DataHandler.h"
#include <string>
#include <algorithm>
#include <iostream>

namespace {
    char LITTLE_GRDB [] = {'B','D','R','G'};
}

const std::string Data::Mission::PTCResource::FILE_EXTENSION = "ptc";
const uint32_t Data::Mission::PTCResource::IDENTIFIER_TAG = 0x43707463; // which is { 0x43, 0x70, 0x74, 0x63 } or { 'C', 'p', 't', 'c' } or "Cptc"

Data::Mission::PTCResource::PTCResource() {

}

Data::Mission::PTCResource::PTCResource( const PTCResource &obj ) : Resource( obj ), grid( obj.grid ) {
}

std::string Data::Mission::PTCResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::PTCResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool Data::Mission::PTCResource::makeTiles( const std::vector<Data::Mission::TilResource*> &tile_array_r ) {
    if( tile_array_r.size() > 0 )
    {
        debug_map_display.setWidth(  grid.getWidth()  * 0x11 );
        debug_map_display.setHeight( grid.getHeight() * 0x11 );
        debug_map_display.setFormat( Utilities::ImageData::RED_GREEN_BLUE, 1 );

        this->tile_array_r = std::vector<Data::Mission::TilResource*>( tile_array_r );

        for( unsigned int x = 0; x < grid.getWidth(); x++ ) {
            for( unsigned int y = 0; y < grid.getHeight(); y++ ) {
                auto pixel = grid.getPixel( x, y );

                if( pixel != nullptr && *reinterpret_cast<const uint8_t*>(pixel) != 0 )
                    debug_map_display.inscribeSubImage( x * 0x11, y * 0x11, *this->tile_array_r.at( (*reinterpret_cast<const uint8_t*>( pixel ) / sizeof( uint32_t ) - 1) % this->tile_array_r.size() )->getImage() );
            }
        }
        return true;
    }
    else
        return false;
}

Data::Mission::TilResource* Data::Mission::PTCResource::getTile( unsigned int x, unsigned int y ) {
    if( x < grid.getWidth() && y < grid.getHeight() ) {
        auto pixel = grid.getPixel( x, y );
        if( *reinterpret_cast<const uint32_t*>(pixel) != 0 )
            return tile_array_r.at( (*reinterpret_cast<const uint32_t*>( pixel ) / sizeof( uint32_t ) - 1) % this->tile_array_r.size() );
    }

    return nullptr;
}

const Data::Mission::TilResource* Data::Mission::PTCResource::getTile( unsigned int x, unsigned int y ) const {
    return const_cast< Data::Mission::PTCResource* >(this)->getTile( x, y );
}

bool Data::Mission::PTCResource::parse( const Utilities::Buffer &header, const Utilities::Buffer &reader_data, const ParseSettings &settings ) {
    auto raw_data = reader_data.getReader().getBytes();

    bool file_is_not_valid = false;
    auto data = raw_data.data();

    while( static_cast<unsigned int>(data - raw_data.data()) < raw_data.size() ) {
        auto identifier = Utilities::DataHandler::read_u32( data, settings.is_opposite_endian );
        auto tag_size   = Utilities::DataHandler::read_u32( data + sizeof( uint32_t ), settings.is_opposite_endian );

        if( identifier == *reinterpret_cast<uint32_t*>( LITTLE_GRDB ) ) {
            auto tile_amount = Utilities::DataHandler::read_u32( data + 0x0C, settings.is_opposite_endian );
            auto width  = Utilities::DataHandler::read_u32( data + 0x10, settings.is_opposite_endian );
            auto height = Utilities::DataHandler::read_u32( data + 0x14, settings.is_opposite_endian );
            auto image_read_head = data + 0x2C;

            // setup the grid
            grid.setWidth(  width );
            grid.setHeight( height );
            grid.setFormat( Utilities::ImageData::BLACK_WHITE, 4 );

            auto image_data = grid.getRawImageData();
            for( unsigned int a = 0; a < grid.getWidth() * grid.getHeight(); a++ ) {

                *reinterpret_cast<uint32_t*>(image_data) = Utilities::DataHandler::read_u32( image_read_head, settings.is_opposite_endian );

                image_data += grid.getPixelSize();
                image_read_head += sizeof( uint32_t );
            }
        }

        data += tag_size;
    }

    return !file_is_not_valid;
}

Data::Mission::Resource * Data::Mission::PTCResource::duplicate() const {
    return new PTCResource( *this );
}

int Data::Mission::PTCResource::write( const char *const file_path, const std::vector<std::string> & arguments ) const {
    std::string file_path_texture = std::string( file_path ) + ".png";
    bool enable_export = true;

    for( auto arg = arguments.begin(); arg != arguments.end(); arg++ ) {
        if( (*arg).compare("--dry") == 0 )
            enable_export = false;
    }

    if( enable_export )
        return debug_map_display.write( file_path_texture.c_str() );
    else
        return 0;
}

std::vector<Data::Mission::PTCResource*> Data::Mission::PTCResource::getVector( Data::Mission::IFF &mission_file ) {
    std::vector<Resource*> to_copy = mission_file.getResources( Data::Mission::PTCResource::IDENTIFIER_TAG );

    std::vector<PTCResource*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<PTCResource*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::PTCResource*> Data::Mission::PTCResource::getVector( const Data::Mission::IFF &mission_file ) {
    return Data::Mission::PTCResource::getVector( const_cast< Data::Mission::IFF& >( mission_file ) );
}
