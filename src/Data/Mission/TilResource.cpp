#include "TilResource.h"
#include "Til/Mesh.h"
#include "Til/Colorizer.h"

#include "IFF.h"

#include "../../Utilities/DataHandler.h"
#include "../../Utilities/ImageFormat/Chooser.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cassert>
#include <set>

namespace {
uint32_t TAG_SECT = 0x53656374; // which is { 0x53, 0x65, 0x63, 0x74 } or { 'S', 'e', 'c', 't' } or "Sect";
uint32_t TAG_SLFX = 0x534C4658; // which is { 0x53, 0x4C, 0x46, 0x58 } or { 'S', 'L', 'F', 'X' } or "SLFX";
uint32_t TAG_ScTA = 0x53635441; // which is { 0x53, 0x63, 0x54, 0x41 } or { 'S', 'c', 'T', 'A' } or "ScTA"; // Unknown, but it is significantly bigger than SLFX

void readCullingTile( Data::Mission::TilResource::CullingTile &tile, Utilities::Buffer::Reader &reader, Utilities::Buffer::Endian endian ) {
    tile.top_left = reader.readU16( endian );
    reader.readU16(); // Skip Unknown data.
    
    tile.top_right = reader.readU16( endian );
    reader.readU16(); // Skip Unknown data.
    
    tile.bottom_left = reader.readU16( endian );
    reader.readU16(); // Skip Unknown data.
    
    tile.bottom_right = reader.readU16( endian );
    reader.readU16(); // Skip Unknown data.
}
}

const std::string Data::Mission::TilResource::FILE_EXTENSION = "til";
const uint32_t Data::Mission::TilResource::IDENTIFIER_TAG = 0x4374696C; // which is { 0x43, 0x74, 0x69, 0x6C } or { 'C', 't', 'i', 'l' } or "Ctil"

Data::Mission::TilResource::TilResource() {

}

Data::Mission::TilResource::TilResource( const TilResource &obj ) : ModelResource( obj ), point_cloud_3_channel( obj.point_cloud_3_channel ) {
}

std::string Data::Mission::TilResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::TilResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

Utilities::Image2D Data::Mission::TilResource::getImage() const {
    Utilities::PixelFormatColor_R8G8B8 color_format;
    Utilities::Image2D image( point_cloud_3_channel.getWidth(), point_cloud_3_channel.getHeight(), color_format );
    Utilities::PixelFormatColor::GenericColor color(0.0f, 0.0f, 0.0f, 1.0f);
    
    for( unsigned y = 0; y < point_cloud_3_channel.getHeight(); y++ ) {
        for( unsigned x = 0; x < point_cloud_3_channel.getWidth(); x++ ) {
            auto height = point_cloud_3_channel.getValue( x, y );
            
            color.red   = static_cast<int16_t>( height.channel[0] ) + 128;
            color.green = static_cast<int16_t>( height.channel[1] ) + 128;
            color.blue  = static_cast<int16_t>( height.channel[2] ) + 128;
            
            color.red   *= 1.0 / 256.0;
            color.green *= 1.0 / 256.0;
            color.blue  *= 1.0 / 256.0;
            
            image.writePixel( x, y, color );
        }
    }
    
    return image;
}

void Data::Mission::TilResource::makeEmpty() {
    point_cloud_3_channel.setDimensions( AMOUNT_OF_TILES + 1, AMOUNT_OF_TILES + 1 );
    
    for( unsigned y = 0; y < point_cloud_3_channel.getHeight(); y++ ) {
        for( unsigned x = 0; x < point_cloud_3_channel.getWidth(); x++ ) {
            HeightmapPixel height;
            
            height.channel[0] = -128;
            height.channel[1] =  127;
            height.channel[2] = -128;
            
            point_cloud_3_channel.setValue( x, y, height );
        }
    }
    
    // I decided to set these anyways.
    this->culling_distance = 0;
    this->culling_top_left.primary = 0;
    this->culling_top_left.top_left = 0;
    this->culling_top_left.top_right = 0;
    this->culling_top_left.bottom_left = 0;
    this->culling_top_left.bottom_right = 0;
    this->culling_top_right    = this->culling_top_left;
    this->culling_bottom_left  = this->culling_top_left;
    this->culling_bottom_right = this->culling_top_left;
    
    this->texture_reference = 0;
    
    for( unsigned int x = 0; x < AMOUNT_OF_TILES; x++ ) {
        for( unsigned int y = 0; y < AMOUNT_OF_TILES; y++ ) {
            mesh_reference_grid[x][y].tile_amount = 1;
            mesh_reference_grid[x][y].tiles_start = 0; // It will refer to one tile.
        }
    }
    
    // Make a generic tile
    Tile one_tile( 0 );
    
    one_tile.end_column = 0;
    one_tile.texture_cord_index = 0;
    one_tile.front = 0;
    one_tile.back = 0;
    one_tile.unknown_1 = 0;
    one_tile.mesh_type = 60; // This should make an interesting pattern.
    one_tile.graphics_type_index = 0;
    
    this->mesh_tiles.clear();
    this->mesh_tiles.push_back( one_tile );
    
    this->texture_cords.clear();
    this->texture_cords.push_back( glm::u8vec2( 0,  0) );
    this->texture_cords.push_back( glm::u8vec2(32,  0) );
    this->texture_cords.push_back( glm::u8vec2(32, 32) );
    this->texture_cords.push_back( glm::u8vec2(32,  0) );
    
    this->colors.clear();
    
    this->tile_graphics_bitfield.clear();
    
    TileGraphics flat;
    
    flat.shading = 127;
    flat.texture_index = 0;
    flat.unknown_0 = 0;
    flat.rectangle = 1; // This is a rectangle.
    flat.type = 0; // Make a pure flat
    
    this->tile_graphics_bitfield.push_back( flat.get() );
    
    this->all_triangles.clear();
    
    // Create the physics cells for this Til.
    for( unsigned int x = 0; x < AMOUNT_OF_TILES; x++ ) {
        for( unsigned int z = 0; z < AMOUNT_OF_TILES; z++ ) {
            createPhysicsCell( x, z );
        }
    }
}

bool Data::Mission::TilResource::parse( const ParseSettings &settings ) {
    if( this->data_p != nullptr ) {
        auto reader = this->data_p->getReader();
        
        bool file_is_not_valid = false;
        
        while( reader.getPosition( Utilities::Buffer::BEGIN ) < reader.totalSize() ) {
            const auto identifier = reader.readU32( settings.endian );
            const auto tag_size   = reader.readU32( settings.endian );
            auto data_size        = 0;
            
            if( tag_size > 2 * sizeof( tag_size ) )
                data_size = tag_size - 2 * sizeof( tag_size );
            
            if( identifier == TAG_SECT ) {
                auto reader_sect = reader.getReader( data_size );
                
                auto color_amount = reader_sect.readU16( settings.endian );
                auto texture_cordinates_amount = reader_sect.readU16( settings.endian );
                
                if( settings.output_level >= 1 )
                {
                    *settings.output_ref << "Mission::TilResource::load() id = " << getIndexNumber() << std::endl;
                    *settings.output_ref << "Mission::TilResource::load() loc = 0x" << std::hex << getOffset() << std::dec << std::endl;
                    *settings.output_ref << "Color amount = " << color_amount << std::endl;
                    *settings.output_ref << "texture_cordinates_amount = " << texture_cordinates_amount << std::endl;
                }
                
                // setup the point_cloud_3_channel.
                point_cloud_3_channel.setDimensions( AMOUNT_OF_TILES + 1, AMOUNT_OF_TILES + 1 );
                
                for( unsigned y = 0; y < point_cloud_3_channel.getHeight(); y++ ) {
                    for( unsigned x = 0; x < point_cloud_3_channel.getWidth(); x++ ) {
                        HeightmapPixel height;
                        
                        height.channel[0] = reader_sect.readI8();
                        height.channel[1] = reader_sect.readI8();
                        height.channel[2] = reader_sect.readI8();
                        
                        point_cloud_3_channel.setValue( x, y, height );
                    }
                }
                
                // These bytes seems to be only five zero bytes
                reader_sect.readU32(); // Skip 4 bytes
                reader_sect.readU8();  // Skip 1 byte
                
                this->culling_distance = reader_sect.readU16( settings.endian );
                // Padding?
                reader_sect.readU16(); // Skip 2 bytes
                this->culling_top_left.primary = reader_sect.readU16( settings.endian );
                // Padding?
                reader_sect.readU16(); // Skip 2 bytes
                this->culling_top_right.primary = reader_sect.readU16( settings.endian );
                // Padding?
                reader_sect.readU16(); // Skip 2 bytes
                this->culling_bottom_left.primary = reader_sect.readU16( settings.endian );
                // Padding?
                reader_sect.readU16(); // Skip 2 bytes
                this->culling_bottom_right.primary = reader_sect.readU16( settings.endian );
                // Padding?
                reader_sect.readU16(); // Skip 2 bytes
                
                readCullingTile( culling_top_left,     reader_sect, settings.endian );
                readCullingTile( culling_top_right,    reader_sect, settings.endian );
                readCullingTile( culling_bottom_left,  reader_sect, settings.endian );
                readCullingTile( culling_bottom_right, reader_sect, settings.endian );
                
                auto what1 = reader_sect.readU16( settings.endian ) & 0xFF;
                
                // Modifiying this to be other than what it is will cause an error?
                if( what1 != 0 && settings.output_level >= 1 )
                    *settings.output_ref << "Error expected zero in the Til resource." << std::endl;
                
                this->texture_reference = reader_sect.readU16( settings.endian );
                
                for( unsigned int x = 0; x < AMOUNT_OF_TILES; x++ ) {
                    for( unsigned int y = 0; y < AMOUNT_OF_TILES; y++ ) {
                        mesh_reference_grid[x][y].set( reader_sect.readU16( settings.endian ) );
                    }
                }
                
                this->mesh_library_size = reader_sect.readU16( settings.endian );
                
                // Skip 2 bytes
                reader_sect.readU16( settings.endian );
                
                // It turned out that Future Cop: LAPD does not care about this value.
                // Since, every original map that I encounted seemed to have this value,
                // my program would just pay attention to this value and use it for predicting
                // the size of the mesh_tiles for the vector.
                const size_t PREDICTED_POLYGON_TILE_AMOUNT = this->mesh_library_size >> 6;
                
                mesh_tiles.reserve( PREDICTED_POLYGON_TILE_AMOUNT );
                
                std::set<uint16_t> seen_graphics_tiles;

                size_t actual_polygon_tile_amount = 0;
                
                for( size_t i = 0; i < AMOUNT_OF_TILES * AMOUNT_OF_TILES; ) {

                    mesh_tiles.push_back( { reader_sect.readU32( settings.endian ) } );

                    seen_graphics_tiles.insert( mesh_tiles.back().graphics_type_index );

                    if( mesh_tiles.back().end_column )
                        i++;
                    actual_polygon_tile_amount++;
                }

                if( actual_polygon_tile_amount != PREDICTED_POLYGON_TILE_AMOUNT && settings.output_level >= 1  ) {
                    *settings.output_ref << "\n"
                    << "The resource id " << this->getResourceID() << " has mispredicted the polygon tile amount." << "\n"
                    << " mesh_library_size is 0x" << std::hex << this->mesh_library_size << std::dec << "\n"
                    << " The predicted polygons to be there are " << PREDICTED_POLYGON_TILE_AMOUNT << "\n"
                    << " The amount of polygons that exist are  " << actual_polygon_tile_amount << std::endl;
                }
                
                bool skipped_space = false;

                // There are dead uvs that are not being used!
                while( reader_sect.readU32( settings.endian ) == 0 )
                    skipped_space = true;

                // Undo the read after the bytes are skipped.
                reader_sect.setPosition( -static_cast<int>(sizeof( uint32_t )), Utilities::Buffer::CURRENT );

                if( skipped_space && settings.output_level >= 3 )
                {
                    *settings.output_ref << std::endl
                    << "The resource number " << this->getResourceID() << " has " << skipped_space << " skipped." << std::endl
                    << "mesh_library_size is 0x" << std::hex << this->mesh_library_size
                    << " or 0x" << (this->mesh_library_size >> 4)
                    << " or " << std::dec << PREDICTED_POLYGON_TILE_AMOUNT << std::endl;
                }

                // Read the UV's
                texture_cords.reserve( texture_cordinates_amount );
                
                for( size_t i = 0; i < texture_cordinates_amount; i++ ) {
                    texture_cords.push_back( glm::u8vec2() );
                    
                    texture_cords.back().x = reader_sect.readU8();
                    texture_cords.back().y = reader_sect.readU8();
                }
                
                Til::Colorizer::setColors( colors, color_amount, reader_sect, settings.endian );
                
                // Read the texture_references, and shading info.
                while( reader_sect.getPosition( Utilities::Buffer::END ) >= sizeof(uint16_t) ) {
                    const auto data = reader_sect.readU16( settings.endian );
                    
                    tile_graphics_bitfield.push_back( { data } );
                    
                    const auto tile_graphics = TileGraphics( data );
                    
                    if( tile_graphics.type == 3 ) {
                        if( seen_graphics_tiles.find( tile_graphics_bitfield.size() - 1 ) != seen_graphics_tiles.end() ) {
                            // assert( tile_graphics.shading >= 0 && tile_graphics.shading <= 3 );
                        }
                    }
                }
                
                // Create the physics cells for this Til.
                for( unsigned int x = 0; x < AMOUNT_OF_TILES; x++ ) {
                    for( unsigned int z = 0; z < AMOUNT_OF_TILES; z++ ) {
                        createPhysicsCell( x, z );
                    }
                }
            }
            else
            if( identifier == TAG_SLFX ) {
                auto reader_slfx = reader.getReader( data_size );
                
                // Read this bitfield!
                this->slfx_bitfield = reader_slfx.readU32( settings.endian );
            }
            else
            if( identifier == TAG_ScTA ) {
                auto reader_scta = reader.getReader( data_size );
                
                // TODO Find out what this does later. Known sizes 36, 44, 84, 100, 132, 164, and 204
            }
            else
            {
                char identifier_word[5] = {'\0'};
                const auto IDENTIFIER_SIZE = (sizeof( identifier_word ) - 1) / sizeof(identifier_word[0]);

                for( unsigned int i = 0; i < IDENTIFIER_SIZE; i++ ) {
                    identifier_word[ i ] = reinterpret_cast<const char*>( &identifier )[ i ];
                }
                Utilities::DataHandler::swapBytes( reinterpret_cast< uint8_t* >( identifier_word ), 4 );

                if( settings.output_level >= 0 ) {
                    *settings.output_ref << "Mission::TilResource::load() " << identifier_word << " not recognized" << std::endl;
                }
                
                reader.setPosition( data_size, Utilities::Buffer::CURRENT );
                
                assert( false );
            }
        }
        

        return !file_is_not_valid;
    }
    else
        return false;
}

Data::Mission::Resource * Data::Mission::TilResource::duplicate() const {
    return new Data::Mission::TilResource( *this );
}

bool Data::Mission::TilResource::loadTextures( const std::vector<Data::Mission::BMPResource*> &textures ) {
    const static size_t TEXTURE_LIMIT = sizeof(texture_names) / sizeof( texture_names[0] );
    bool valid = true;

    for( auto cur = textures.begin(); cur != textures.end(); cur++ ) {
        if( (*cur)->getResourceID() - 1 < TEXTURE_LIMIT ) {
            if( (*cur)->getImageFormat() != nullptr )
                texture_names[ (*cur)->getResourceID() - 1 ] = (*cur)->getImageFormat()->appendExtension( (*cur)->getFullName( (*cur)->getResourceID() ) );
        }
    }

    for( size_t i = 0; i < TEXTURE_LIMIT; i++ ) {
        if( texture_names[ i ].empty() )
            valid = false;
    }

    return valid;
}

using Data::Mission::Til::Mesh::BACK_LEFT;
using Data::Mission::Til::Mesh::BACK_RIGHT;
using Data::Mission::Til::Mesh::FRONT_RIGHT;
using Data::Mission::Til::Mesh::FRONT_LEFT;

int Data::Mission::TilResource::write( const std::string& file_path, const std::vector<std::string> & arguments ) const {
    bool enable_point_cloud_export = false;
    bool enable_height_map_export = false;
    bool enable_til_export_model = false;
    bool enable_export = true;
    int glTF_return = 0;
    Utilities::ImageFormat::Chooser chooser;

    for( auto arg = arguments.begin(); arg != arguments.end(); arg++ ) {
        if( (*arg).compare("--TIL_EXPORT_POINT_CLOUD_MAP") == 0 )
            enable_point_cloud_export = true;
        else
        if( (*arg).compare("--TIL_EXPORT_HEIGHT_MAP") == 0 )
            enable_height_map_export = true;
        else
        if( (*arg).compare("--TIL_EXPORT_MODEL") == 0 )
            enable_til_export_model = true;
        else
        if( (*arg).compare("--dry") == 0 )
            enable_export = false;
    }

    Utilities::PixelFormatColor_R8G8B8 rgb;
    Utilities::ImageFormat::ImageFormat* the_choosen_r = chooser.getWriterReference( rgb );

    if( the_choosen_r != nullptr && enable_export ) {
        if( enable_point_cloud_export ) {
            // Write the three heightmaps encoded in three color channels.
            // TODO Find out what to do if the image cannot be written.
            Utilities::Buffer buffer;
            
            the_choosen_r->write( getImage(), buffer );
            buffer.write( the_choosen_r->appendExtension( file_path ) );
        }
        if( enable_height_map_export ) {
            // Write out the depth field of the Til Resource.
            Utilities::Image2D heightmap = getHeightMap( 8 );
            
            Utilities::Buffer buffer;
            the_choosen_r->write( heightmap, buffer );
            buffer.write( the_choosen_r->appendExtension( std::string( file_path ) + "_height" ) );
        }
    }

    if( enable_til_export_model ) {
        Utilities::ModelBuilder *model_output_p = createModel( &arguments );
        
        if( enable_export && model_output_p != nullptr )
            glTF_return = model_output_p->write( file_path, "til_"+ std::to_string( getResourceID() ) );
        
        delete model_output_p;
    }
    else
        glTF_return = true; // Nothing is wrong since Til was not set to be exported.

    return glTF_return;
}

Utilities::ModelBuilder * Data::Mission::TilResource::createModel( const std::vector<std::string> * arguments ) const {
    if( !mesh_tiles.empty() ) // Make sure there is no out of bounds error.
    {
        // Single texture models are to be generated first.
        std::vector<Utilities::ModelBuilder*> texture_models;
        
        for( unsigned int i = 0; i < TEXTURE_NAMES_AMOUNT; i++ ) {
            auto texture_model_p = createPartial( i );
            
            if( texture_model_p != nullptr )
                texture_models.push_back( texture_model_p );
        }
        
        int status;
        Utilities::ModelBuilder* model_output = Utilities::ModelBuilder::combine( texture_models, status );
        
        if( status != 1 ) {
            std::cout << "Data::Mission::TilResource::createModel has a problem" << std::endl;
            std::cout << "  combine has resulted in status " << status << std::endl;
        }
        
        // Delete the other models.
        for( auto i : texture_models ) {
            delete i;
        }

        return model_output;
    }
    else
        return nullptr;
}

Utilities::ModelBuilder * Data::Mission::TilResource::createPartial( unsigned int texture_index, float x_offset, float y_offset ) const {
    if( texture_index > TEXTURE_NAMES_AMOUNT + 1 )
        return nullptr;
    else {
        Utilities::ModelBuilder *model_output = new Utilities::ModelBuilder();
        bool display_unread = false;
        bool has_displayed = false;
        bool has_texture_displayed = false;
        
        unsigned int position_compon_index = model_output->addVertexComponent( Utilities::ModelBuilder::POSITION_COMPONENT_NAME, Utilities::DataTypes::ComponentType::FLOAT, Utilities::DataTypes::Type::VEC3 );
        unsigned int normal_compon_index = model_output->addVertexComponent( Utilities::ModelBuilder::NORMAL_COMPONENT_NAME, Utilities::DataTypes::ComponentType::FLOAT, Utilities::DataTypes::Type::VEC3 );
        unsigned int color_compon_index = model_output->addVertexComponent( Utilities::ModelBuilder::COLORS_0_COMPONENT_NAME, Utilities::DataTypes::ComponentType::FLOAT, Utilities::DataTypes::Type::VEC3 );
        unsigned int tex_coord_0_compon_index = model_output->addVertexComponent( Utilities::ModelBuilder::TEX_COORD_0_COMPONENT_NAME, Utilities::DataTypes::ComponentType::UNSIGNED_BYTE, Utilities::DataTypes::Type::VEC2, true );
        unsigned int tile_type_compon_index = model_output->addVertexComponent( "_TileType", Utilities::DataTypes::ComponentType::INT, Utilities::DataTypes::SCALAR, false );

        model_output->setupVertexComponents();

        glm::vec3   position_displacement;
        glm::vec3   position[6];
        glm::vec3   normal[6];
        glm::vec3   color[6];
        glm::u8vec2 coord[6];

        position_displacement.x = SPAN_OF_TIL + x_offset;
        position_displacement.y = 0.0;
        position_displacement.z = SPAN_OF_TIL + y_offset;
        
        TileGraphics tileGraphic;

        has_texture_displayed = false;
        
        if( texture_index < TEXTURE_NAMES_AMOUNT )
            model_output->setMaterial( texture_names[ texture_index ], texture_index + 1 );

        for( unsigned int x = 0; x < AMOUNT_OF_TILES; x++ ) {
            for( unsigned int y = 0; y < AMOUNT_OF_TILES; y++ ) {
                for( auto t = 0; t < mesh_reference_grid[x][y].tile_amount; t++ )
                {
                    unsigned int current_tile_polygon_amount = 0;

                    const Tile current_tile = mesh_tiles.at( (t + mesh_reference_grid[x][y].tiles_start) % mesh_tiles.size() );

                    Data::Mission::Til::Mesh::Input input;
                    input.pixels[ FRONT_LEFT  ] = point_cloud_3_channel.getRef( y + 0, x + 0 );
                    input.pixels[  BACK_LEFT  ] = point_cloud_3_channel.getRef( y + 1, x + 0 );
                    input.pixels[  BACK_RIGHT ] = point_cloud_3_channel.getRef( y + 1, x + 1 );
                    input.pixels[ FRONT_RIGHT ] = point_cloud_3_channel.getRef( y + 0, x + 1 );
                    input.coord_index = current_tile.texture_cord_index;
                    input.coord_index_limit = this->texture_cords.size();
                    input.coord_data = this->texture_cords.data();

                    Data::Mission::Til::Mesh::VertexData vertex_data;
                    vertex_data.position = position;
                    vertex_data.coords = coord;
                    vertex_data.colors = color;
                    vertex_data.element_amount = 6;
                    vertex_data.element_start = 0;

                    Data::Mission::Til::Colorizer::Input input_color = { this->colors, this->tile_graphics_bitfield };
                    input_color.tile_index = current_tile.graphics_type_index;
                    input_color.unk = 0;
                    input_color.position.x = x;
                    input_color.position.y = y;
                    input_color.position.z = 0;

                    Data::Mission::Til::Colorizer::setSquareColors( input_color, input.colors );

                    if( TileGraphics( this->tile_graphics_bitfield.at( input_color.tile_index ) ).texture_index == texture_index || texture_index == TEXTURE_NAMES_AMOUNT ) {
                        current_tile_polygon_amount = createTile( input, vertex_data, current_tile.mesh_type );

                        if( current_tile_polygon_amount == 0 && display_unread ) {
                            if( !has_displayed ) {
                                std::cout << "Starting error log for " << this->getIndexNumber() << std::endl;
                                has_displayed = true;
                            }

                            if( !has_texture_displayed ) {
                                std::cout << "For texture index of " << texture_index << std::endl;
                                has_texture_displayed = true;
                            }

                            std::cout << "Unknown tile at (" << x << "," << y << ") for " << current_tile.mesh_type << std::endl;
                        }

                        for( unsigned int i = 0; i < current_tile_polygon_amount; i++ ) {
                            position[ i ].x += position_displacement.x;
                            position[ i ].z += position_displacement.z;

                            std::swap( position[ i ].x, position[ i ].z );
                            position[ i ].x = -position[ i ].x;
                            position[ i ].z = -position[ i ].z;
                        }
                    }

                    // Generate the normals
                    {
                        glm::vec3 u;
                        glm::vec3 v;

                        // Generate the normals
                        for( unsigned int p = 0; p < current_tile_polygon_amount / 3; p++ )
                        {
                            u.x = position[ p * 3 + 1 ].x - position[ p * 3 + 0 ].x;
                            u.y = position[ p * 3 + 1 ].y - position[ p * 3 + 0 ].y;
                            u.z = position[ p * 3 + 1 ].z - position[ p * 3 + 0 ].z;

                            v.x = position[ p * 3 + 2 ].x - position[ p * 3 + 0 ].x;
                            v.y = position[ p * 3 + 2 ].y - position[ p * 3 + 0 ].y;
                            v.z = position[ p * 3 + 2 ].z - position[ p * 3 + 0 ].z;

                            normal[3 * p].x = u.y * v.z - u.z * v.y;
                            normal[3 * p].y = u.z * v.x - u.x * v.z;
                            normal[3 * p].z = u.x * v.y - u.y * v.x;
                            
                            normal[3 * p] = glm::normalize( normal[3 * p] );

                            // Fill in the value on all points
                            for( unsigned int i = 1; i < 3; i++ ) {
                                normal[3 * p + i ].x = normal[3 * p].x;
                                normal[3 * p + i ].y = normal[3 * p].y;
                                normal[3 * p + i ].z = normal[3 * p].z;
                            }
                        }
                    }

                    {
                        // The write loop for the tiles.
                        for( unsigned int p = 0; p < current_tile_polygon_amount; p++ )
                        {
                            model_output->startVertex();

                            model_output->setVertexData(    position_compon_index, Utilities::DataTypes::Vec3Type( position[p] ) );
                            model_output->setVertexData(      normal_compon_index, Utilities::DataTypes::Vec3Type( normal[p] ) );
                            model_output->setVertexData(       color_compon_index, Utilities::DataTypes::Vec3Type( color[p] ) );
                            model_output->setVertexData( tex_coord_0_compon_index, Utilities::DataTypes::Vec2UByteType( coord[p] ) );
                            model_output->setVertexData(   tile_type_compon_index, Utilities::DataTypes::ScalarUIntType( current_tile.mesh_type ) );
                        }
                    }
                }

                position_displacement.z -= 1.0;
            }
            position_displacement.x -= 1.0;
            position_displacement.z = SPAN_OF_TIL + y_offset;
        }
        
        if( model_output->getNumVertices() < 3 ) {
            delete model_output;
            return nullptr;
        }
        else
            return model_output;
    }
}

void Data::Mission::TilResource::createPhysicsCell( unsigned int x, unsigned int z ) {
    if( x < AMOUNT_OF_TILES && z < AMOUNT_OF_TILES ) {
        glm::vec3 position[6];
        glm::u8vec2 cord[6];
        glm::vec3 color[6];
        Tile current_tile;
        Data::Mission::Til::Mesh::Input input;
        Data::Mission::Til::Mesh::VertexData vertex_data;
        
        input.pixels[ FRONT_LEFT  ] = point_cloud_3_channel.getRef( z + 0, x + 0 );
        input.pixels[  BACK_LEFT  ] = point_cloud_3_channel.getRef( z + 1, x + 0 );
        input.pixels[  BACK_RIGHT ] = point_cloud_3_channel.getRef( z + 1, x + 1 );
        input.pixels[ FRONT_RIGHT ] = point_cloud_3_channel.getRef( z + 0, x + 1 );
        
        vertex_data.position = position;
        vertex_data.coords = cord;
        vertex_data.colors = color;
        
        input.coord_index_limit = this->texture_cords.size();
        input.coord_data = this->texture_cords.data();
        
        for( auto current_tile_index = 0; current_tile_index < mesh_reference_grid[x][z].tile_amount; current_tile_index++ ) {
            
            current_tile = mesh_tiles.at( (current_tile_index + mesh_reference_grid[x][z].tiles_start) % mesh_tiles.size() );
            
            input.coord_index = current_tile.texture_cord_index;
            
            vertex_data.element_amount = 6;
            vertex_data.element_start = 0;
            
            auto amount_of_vertices = createTile( input, vertex_data, current_tile.mesh_type );
            
            for( unsigned int i = 0; i < amount_of_vertices; i++ ) {
                position[ i ].x += (SPAN_OF_TIL - x) - 0.5;
                position[ i ].z += (SPAN_OF_TIL - z) - 0.5;

                // Flip Both Axis's
                position[ i ].x = -position[ i ].x;
                position[ i ].z = -position[ i ].z;
            }
            
            for( unsigned int i = 0; i < amount_of_vertices; i += 3 ) {
                all_triangles.push_back( Utilities::Collision::Triangle( &position[ i ] ) );
            }
        }
    }
}

float Data::Mission::TilResource::getRayCast3D( const Utilities::Collision::Ray &ray ) const {
    // TODO Develop a more complex, but more effient raycasting implementation like DDA.
    bool found_triangle = false;
    float final_distance = 1000000.0f;
    float temp_distance;
    glm::vec3 point;
    glm::vec3 barycentric;
    
    for( auto i : all_triangles ) {
        // Get the intersection distance from the plane first.
        temp_distance = i.getIntersectionDistance( ray );
        
        // If temp_distance is positive and
        // if temp_distance is shorter than final distance. Then, this ray should be checked if it is in the triangle.
        if( temp_distance > 0.0f && temp_distance < final_distance ) {
            
            // Get the point in 3D space.
            point = ray.getSpot( temp_distance );
            
            // Get the barycentric cordinates.
            barycentric = i.getBarycentricCordinates( point );
            
            // If these cordinates can indicate that they are in the triangle then the ray collides with the triangle.
            if( i.isInTriangle( barycentric ) ) {
                
                // A triangle has been found.
                found_triangle = true;
                
                // The final_distance is now at the triangle.
                final_distance = temp_distance;
            }
        }
    }
    
    // If the triangle has been found then return a positive number.
    if( found_triangle )
        return final_distance;
    else
        return -1.0f;
}

float Data::Mission::TilResource::getRayCast2D( float x, float z ) const {
    return getRayCastDownward( x, z, MAX_HEIGHT );
}

float Data::Mission::TilResource::getRayCastDownward( float x, float z, float from_highest_point ) const {
    // TODO I have an algorithm in mind to make this much faster. It involves using planes and a 2D grid.
    Utilities::Collision::Ray downRay( glm::vec3( x, from_highest_point, z ), glm::vec3( x, from_highest_point - 1.0f, z ) );
    
    return getRayCast3D( downRay );
}

const std::vector<Utilities::Collision::Triangle>& Data::Mission::TilResource::getAllTriangles() const {
    return all_triangles;
}

Utilities::Image2D Data::Mission::TilResource::getHeightMap( unsigned int rays_per_tile ) const {
    Utilities::PixelFormatColor_R8G8B8 color_format;
    Utilities::Image2D heightmap( AMOUNT_OF_TILES * rays_per_tile, AMOUNT_OF_TILES * rays_per_tile, color_format );
    Utilities::PixelFormatColor::GenericColor color;
    
    const float LENGTH = static_cast<float>(AMOUNT_OF_TILES ) - (1.0f / static_cast<float>( rays_per_tile ));
    const float HALF_LENGTH = LENGTH / 2.0f;
    const float STEPER = LENGTH / static_cast<float>((AMOUNT_OF_TILES * rays_per_tile - 1));
    
    for( unsigned int x = 0; x < AMOUNT_OF_TILES * rays_per_tile; x++ ) {
        
        float x_pos = static_cast<float>(x) * STEPER - HALF_LENGTH;
        
        for( unsigned int z = 0; z < AMOUNT_OF_TILES * rays_per_tile; z++ ) {
            
            float z_pos = static_cast<float>(z) * STEPER - HALF_LENGTH;
            
            float distance = getRayCast2D( z_pos, x_pos );
            
            // This means that no triangles had been hit
            if( distance < 0.0f ) {
                color.red   = 1.0f;
                color.green = 0.0f;
                color.blue  = 0.0f;
            }
            else // This means beyond of range.
            if( distance > 2.0f * MAX_HEIGHT ) {
                color.red   = 0.0f;
                color.green = 1.0f;
                color.blue  = 1.0f;
            }
            else { // This means that the pixel works for the image format.
                distance = 2.0f * MAX_HEIGHT - distance;
                distance *= 1.0f / (2.0f * MAX_HEIGHT);
                
                color.red   = distance;
                color.green = distance;
                color.blue  = distance;
            }
            
            heightmap.writePixel( x, z, color );
        }
    }
    
    return heightmap;
}

std::vector<Data::Mission::TilResource*> Data::Mission::TilResource::getVector( Data::Mission::IFF &mission_file ) {
    std::vector<Resource*> to_copy = mission_file.getResources( Data::Mission::TilResource::IDENTIFIER_TAG );

    std::vector<TilResource*> copy;

    copy.reserve( to_copy.size() );

    for( Data::Mission::Resource* it : to_copy )
    {
        assert( dynamic_cast<TilResource*>( it ) != nullptr );
        copy.push_back( dynamic_cast<TilResource*>( it ) );
    }

    return copy;
}

const std::vector<Data::Mission::TilResource*> Data::Mission::TilResource::getVector( const Data::Mission::IFF &mission_file ) {
    return Data::Mission::TilResource::getVector( const_cast< Data::Mission::IFF& >( mission_file ) );
}
