#include "PTCResource.h"

#include "../../Utilities/DataHandler.h"
#include "../../Utilities/ImageFormat/Chooser.h"
#include "../../Utilities/ModelBuilder.h"
#include <string>
#include <algorithm>
#include <iostream>

namespace {
    const uint32_t GRDB_TAG = 0x47524442; // which is { 0x47, 0x52, 0x44, 0x42 } or { 'G', 'R', 'D', 'B' } or "GRDB"
}

const std::string Data::Mission::PTCResource::FILE_EXTENSION = "ptc";
const uint32_t Data::Mission::PTCResource::IDENTIFIER_TAG = 0x43707463; // which is { 0x43, 0x70, 0x74, 0x63 } or { 'C', 'p', 't', 'c' } or "Cptc"

Data::Mission::PTCResource::PTCResource() : grid(), debug_map_display_p( nullptr ) {
}

Data::Mission::PTCResource::PTCResource( const PTCResource &obj ) : Resource( obj ), grid( obj.grid ), debug_map_display_p( nullptr ), tile_array_r( obj.tile_array_r ) {
        
    if( obj.debug_map_display_p != nullptr )
        debug_map_display_p = new Utilities::Image2D( *obj.debug_map_display_p );
}

Data::Mission::PTCResource::~PTCResource() {
    if( debug_map_display_p != nullptr )
        delete debug_map_display_p;
}

std::string Data::Mission::PTCResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::PTCResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool Data::Mission::PTCResource::makeTiles( const std::vector<Data::Mission::TilResource*> &tile_array_r )
{
    Utilities::PixelFormatColor_R8G8B8 color_format;
    
    if( tile_array_r.size() > 0 )
    {
        if( debug_map_display_p != nullptr )
            delete debug_map_display_p;
        
        debug_map_display_p = new Utilities::Image2D( grid.getWidth() * 0x11,  grid.getHeight() * 0x11, color_format );
        
        this->tile_array_r = std::vector<Data::Mission::TilResource*>( tile_array_r );

        for( unsigned int x = 0; x < grid.getWidth(); x++ ) {
            for( unsigned int y = 0; y < grid.getHeight(); y++ ) {
                auto tile = getTile( x, y );

                if( tile != nullptr ) {
                    auto grid_x = x * 0x11;
                    auto grid_y = y * 0x11;
                    
                    auto image = tile->getImage();
                    
                    debug_map_display_p->inscribeSubImage( grid_x, grid_y, image );
                    
                }
            }
        }
        return true;
    }
    else
        return false;
}

Data::Mission::TilResource* Data::Mission::PTCResource::getTile( unsigned int x, unsigned int y ) {
    if( x < grid.getWidth() && y < grid.getHeight() ) {
        if( grid.getValue( x, y ) != 0 )
            return tile_array_r.at( (grid.getValue( x, y ) / 4 - 1) % this->tile_array_r.size() );
    }

    return nullptr;
}

const Data::Mission::TilResource* Data::Mission::PTCResource::getTile( unsigned int x, unsigned int y ) const {
    return const_cast< Data::Mission::PTCResource* >(this)->getTile( x, y );
}

bool Data::Mission::PTCResource::parse( const ParseSettings &settings ) {
    if( this->data_p != nullptr )
    {
        auto reader = this->data_p->getReader();
        bool file_is_not_valid = false;

        while( reader.getPosition( Utilities::Buffer::BEGIN ) < reader.totalSize() ) {
            auto identifier = reader.readU32( settings.endian );
            auto tag_size   = reader.readU32( settings.endian );

            if( identifier == GRDB_TAG ) {
                auto readerGRDB = reader.getReader( tag_size - sizeof( uint32_t ) * 2 );
                readerGRDB.setPosition( sizeof( uint32_t ), Utilities::Buffer::BEGIN );
                
                auto tile_amount = readerGRDB.readU32( settings.endian );
                auto width       = readerGRDB.readU32( settings.endian );
                auto height      = readerGRDB.readU32( settings.endian );
                
                // Go to offset for boarder settings.
                readerGRDB.setPosition( 0x1C, Utilities::Buffer::BEGIN );
                border_range = readerGRDB.readU32( settings.endian );
                
                readerGRDB.setPosition( 0x24, Utilities::Buffer::BEGIN );

                // setup the grid
                grid.setDimensions( width, height );

                for( unsigned int y = 0; y < grid.getHeight(); y++ ) {
                    for( unsigned int x = 0; x < grid.getWidth(); x++) {
                        grid.setValue( x, y, readerGRDB.readU32( settings.endian ) );
                    }
                }
            }
            else
            {
                reader.setPosition( tag_size - sizeof( uint32_t ) * 2, Utilities::Buffer::CURRENT );
            }
        }

        return !file_is_not_valid;
    }
    else
        return false;
}

Data::Mission::Resource * Data::Mission::PTCResource::duplicate() const {
    return new PTCResource( *this );
}

int Data::Mission::PTCResource::write( const char *const file_path, const std::vector<std::string> & arguments ) const {
    bool enable_export = true;
    bool entire_map = false;
    bool entire_height_map = false;
    Utilities::ImageFormat::Chooser chooser;

    for( auto arg = arguments.begin(); arg != arguments.end(); arg++ ) {
        if( (*arg).compare("--dry") == 0 )
            enable_export = false;
        else
        if( (*arg).compare("--PTC_ENTIRE_MAP") == 0 ) {
            entire_map = true;
        }
        else
        if( (*arg).compare("--PTC_ENTIRE_HEIGHT_MAP") == 0 ) {
            entire_height_map = true;
        }
    }

    if( enable_export ) {
        if( !entire_map ) {
            Utilities::ImageFormat::ImageFormat* the_choosen_r = chooser.getWriterReference( *debug_map_display_p );

            if( the_choosen_r != nullptr ) {
                Utilities::Buffer buffer;
                the_choosen_r->write(  *debug_map_display_p, buffer );

                buffer.write( the_choosen_r->appendExtension( file_path ) );
            }
        }
        else {
            // Write the entire map.
            return writeEntireMap( std::string(file_path) );
        }
        
        if( entire_height_map ) {
            unsigned int rays_per_tile = 1;
            Utilities::PixelFormatColor_R8G8B8 color_format;
            
            Utilities::Image2D ptc_height_map( grid.getWidth() * rays_per_tile * 16, grid.getHeight() * rays_per_tile * 16, color_format );

            for( unsigned int x = 0; x < grid.getWidth(); x++ ) {
                for( unsigned int y = 0; y < grid.getHeight(); y++ ) {
                    auto tile_r = getTile( x, y );

                    if( tile_r != nullptr ) {
                        
                        auto height_map = tile_r->getHeightMap( rays_per_tile );
                        
                        ptc_height_map.inscribeSubImage( x * rays_per_tile * 16, y * rays_per_tile * 16, height_map );
                    }
                }
            }
            
            Utilities::ImageFormat::ImageFormat* the_choosen_r = chooser.getWriterReference( ptc_height_map );
            
            if( the_choosen_r != nullptr ) {
                Utilities::Buffer buffer;
                the_choosen_r->write( ptc_height_map, buffer );

                buffer.write( the_choosen_r->appendExtension( std::string( file_path ) + "_height" ) );
            }
        }
        return 1;
    }
    else
        return 0;
}

int Data::Mission::PTCResource::writeEntireMap( std::string file_path ) const {
    // Write the entire map
    std::vector<Utilities::ModelBuilder*> map_tils;
    
    for( unsigned i = 0; i < 8; i++ ) {
        // This is to combine single textured tils into one.
        std::vector<Utilities::ModelBuilder*> texture_tils;
        
        // Go through every til.
        for( unsigned w = 0; w < getWidth(); w++ ) {
            float x = static_cast<float>(w) - (static_cast<float>(getWidth()) / 2.0f);
            
            for( unsigned h = 0; h < getHeight(); h++ ) {
                float y = static_cast<float>(h) - (static_cast<float>(getHeight()) / 2.0f);
                auto tile_r = getTile( w, h );
                
                if( tile_r != nullptr ) {
                    auto model_p = tile_r->createPartial( i, -y * 16.0f, -x * 16.0f );
                    
                    if( model_p != nullptr )
                        texture_tils.push_back( model_p );
                }
            }
        }
        
        // Combine every texture in common texture into one.
        int result;
        auto combine_model_p = Utilities::ModelBuilder::combine( texture_tils, result );
        
        // Delete the other models.
        for( auto i : texture_tils ) {
            delete i;
        }
        
        // If a combine model is created add this to map_tils.
        if( combine_model_p != nullptr )
            map_tils.push_back( combine_model_p );
    }
    
    // Combine everything.
    int result;
    auto combine_model_p = Utilities::ModelBuilder::combine( map_tils, result );
    
    // Do a clean up.
    for( auto i : map_tils ) {
        delete i;
    }
    
    // If the combine fails return a negative one.
    if( combine_model_p == nullptr )
        return -1; // There is no model to write.
    
    if( combine_model_p->write( file_path ) )
        return 1; // The whole map had been written to the "disk"
    else
        return 0; // Combine model has failed to write.
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
