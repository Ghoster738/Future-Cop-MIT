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

namespace {
uint32_t LITTLE_SECT = 0x53656374; // which is { 0x53, 0x65, 0x63, 0x74 } or { 'S', 'e', 'c', 't' } or "Sect";
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

Utilities::ImageData *const Data::Mission::TilResource::getImage() const {
    return const_cast<Utilities::ImageData *const>(&point_cloud_3_channel);
}

void Data::Mission::TilResource::makeEmpty() {
    point_cloud_3_channel.setWidth(  AMOUNT_OF_TILES + 1 );
    point_cloud_3_channel.setHeight( AMOUNT_OF_TILES + 1 );
    point_cloud_3_channel.setFormat( Utilities::ImageData::RED_GREEN_BLUE, 1 );
    
    auto image_data = point_cloud_3_channel.getRawImageData();
    for( unsigned int a = 0; a < point_cloud_3_channel.getWidth() * point_cloud_3_channel.getHeight(); a++ ) {

        image_data[0] = -128;
        image_data[1] =  127;
        image_data[2] = -128;

        image_data += point_cloud_3_channel.getPixelSize();
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
            mesh_reference_grid[x][y].floor = 0;
            mesh_reference_grid[x][y].tile_amount = 1;
            mesh_reference_grid[x][y].tiles_start = 0; // It will refer to one tile.
        }
    }
    
    // Make a generic tile
    Tile one_tile;
    
    one_tile.tile = 0;
    
    one_tile.unknown_0 = 0;
    one_tile.texture_cord_index = 0;
    one_tile.collision_type = 0; // This means the floor
    one_tile.unknown_1 = 0;
    one_tile.mesh_type = 60; // This should make an interesting pattern.
    one_tile.graphics_type_index = 0;
    
    this->mesh_tiles.clear();
    this->mesh_tiles.push_back( one_tile );
    
    this->texture_cords.clear();
    this->texture_cords.push_back( Utilities::DataTypes::Vec2UByte( 0,  0) );
    this->texture_cords.push_back( Utilities::DataTypes::Vec2UByte(32,  0) );
    this->texture_cords.push_back( Utilities::DataTypes::Vec2UByte(32, 32) );
    this->texture_cords.push_back( Utilities::DataTypes::Vec2UByte(32,  0) );
    
    this->colors.clear();
    
    this->tile_texture_type.clear();
    
    TileGraphics flat;
    
    flat.shading = 127;
    flat.texture_index = 0;
    flat.unknown_0 = 0;
    flat.rectangle = 1; // This is a rectangle.
    flat.type = 0; // Make a pure flat
    
    this->tile_texture_type.push_back( flat );
    
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

        auto identifier = reader.readU32( settings.endian );
        auto tag_size   = reader.readU32( settings.endian );

        if( identifier == LITTLE_SECT ) {
            auto readerSect = reader.getReader( tag_size - 2 * sizeof( tag_size ) );
            
            auto color_amount = readerSect.readU16( settings.endian );
            auto texture_cordinates_amount = readerSect.readU16( settings.endian );
            
            if( settings.output_level >= 3 )
            {
                *settings.output_ref << "Mission::TilResource::load() id = " << getIndexNumber() << std::endl;
                *settings.output_ref << "Mission::TilResource::load() loc = 0x" << std::hex << getOffset() << std::dec << std::endl;
                *settings.output_ref << "Color amount = " << color_amount << std::endl;
                *settings.output_ref << "texture_cordinates_amount = " << texture_cordinates_amount << std::endl;
                *settings.output_ref << "texture_quads_amount = " << texture_cordinates_amount / 4 << std::endl;
            }

            // setup the point_cloud_3_channel.
            point_cloud_3_channel.setWidth(  AMOUNT_OF_TILES + 1 );
            point_cloud_3_channel.setHeight( AMOUNT_OF_TILES + 1 );
            point_cloud_3_channel.setFormat( Utilities::ImageData::RED_GREEN_BLUE, 1 );

            auto image_data = point_cloud_3_channel.getRawImageData();
            for( unsigned int a = 0; a < point_cloud_3_channel.getWidth() * point_cloud_3_channel.getHeight(); a++ ) {

                image_data[0] = readerSect.readU8();
                image_data[1] = readerSect.readU8();
                image_data[2] = readerSect.readU8();

                image_data += point_cloud_3_channel.getPixelSize();
            }

            // These bytes seems to be only five zero bytes
            readerSect.readU32(); // Skip 4 bytes
            readerSect.readU8();  // Skip 1 byte

            this->culling_distance = readerSect.readU16( settings.endian );
            // Padding?
            readerSect.readU16(); // Skip 2 bytes
            this->culling_top_left.primary = readerSect.readU16( settings.endian );
            // Padding?
            readerSect.readU16(); // Skip 2 bytes
            this->culling_top_right.primary = readerSect.readU16( settings.endian );
            // Padding?
            readerSect.readU16(); // Skip 2 bytes
            this->culling_bottom_left.primary = readerSect.readU16( settings.endian );
            // Padding?
            readerSect.readU16(); // Skip 2 bytes
            this->culling_bottom_right.primary = readerSect.readU16( settings.endian );
            // Padding?
            readerSect.readU16(); // Skip 2 bytes

            readCullingTile( culling_top_left,     readerSect, settings.endian );
            readCullingTile( culling_top_right,    readerSect, settings.endian );
            readCullingTile( culling_bottom_left,  readerSect, settings.endian );
            readCullingTile( culling_bottom_right, readerSect, settings.endian );

            auto what1 = readerSect.readU16( settings.endian ) & 0xFF;

            // Modifiying this to be other than what it is will cause an error?
            if( what1 != 0 && settings.output_level >= 1 )
                *settings.output_ref << "Error expected zero in the Til resource." << std::endl;

            this->texture_reference = readerSect.readU16( settings.endian );

            for( unsigned int x = 0; x < AMOUNT_OF_TILES; x++ ) {
                for( unsigned int y = 0; y < AMOUNT_OF_TILES; y++ ) {
                    mesh_reference_grid[x][y].floor = readerSect.readU16( settings.endian );
                }
            }

            this->mesh_library_size = readerSect.readU16( settings.endian );
            
            // Skip 2 bytes
            readerSect.readU16( settings.endian );

            const size_t ACTUAL_MESH_LIBRARY_SIZE = (this->mesh_library_size >> 4) / sizeof( uint32_t );

            mesh_tiles.reserve( ACTUAL_MESH_LIBRARY_SIZE );

            for( size_t i = 0; i < ACTUAL_MESH_LIBRARY_SIZE; i++ )
                mesh_tiles.push_back( { readerSect.readU32( settings.endian ) } );
            
            bool skipped_space = false;

            // There are dead uvs that are not being used!
            while( readerSect.readU32( settings.endian ) == 0 )
                skipped_space = true;
            
            // Undo the read after the bytes are skipped.
            readerSect.setPosition( -static_cast<int>(sizeof( uint32_t )), Utilities::Buffer::Reader::CURRENT );

            if( skipped_space && settings.output_level >= 3 )
            {
                *settings.output_ref << std::endl
                                     << "The resource number " << this->getIndexNumber() << " has " << skipped_space << " skipped." << std::endl
                                     << "mesh_library_size is 0x" << std::hex << this->mesh_library_size
                                     << " or 0x" << (this->mesh_library_size >> 4)
                                     << " or " << std::dec << ACTUAL_MESH_LIBRARY_SIZE << std::endl;
            }

            // Read the UV's
            texture_cords.reserve( texture_cordinates_amount );
            
            for( size_t i = 0; i < texture_cordinates_amount; i++ ) {
                texture_cords.push_back( Utilities::DataTypes::Vec2UByte() );

                texture_cords.back().x = readerSect.readU8();
                texture_cords.back().y = readerSect.readU8();
            }

            // TODO Find out what this shader data does or if it is even shading data.
            colors.reserve( color_amount );

            uint16_t colorful[] = { 0x7c00, 0x03e0, 0x001f, 0x7c1f };

            for( size_t i = 0; i < color_amount; i++ )
                colors.push_back( readerSect.readU16( settings.endian ) );

            // Read the texture_references, and shading info.
            while( readerSect.getPosition( Utilities::Buffer::Reader::END ) >= sizeof(uint16_t) ) {
                // TileGraphics grp;
                // grp.tile_graphics = Utilities::DataHandler::read_u16( image_read_head, settings.is_opposite_endian );

                tile_texture_type.push_back( { readerSect.readU16( settings.endian ) } );
            }
            
            // Create the physics cells for this Til.
            for( unsigned int x = 0; x < AMOUNT_OF_TILES; x++ ) {
                for( unsigned int z = 0; z < AMOUNT_OF_TILES; z++ ) {
                    createPhysicsCell( x, z );
                }
            }
        }
        else
            reader.setPosition( tag_size - 2 * sizeof( uint32_t ), Utilities::Buffer::Reader::CURRENT );

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
                texture_names[ (*cur)->getResourceID() - 1 ] = (*cur)->getImageFormat()->appendExtension( (*cur)->getFullName( (*cur)->getIndexNumber() ) );
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

int Data::Mission::TilResource::write( const char *const file_path, const std::vector<std::string> & arguments ) const {
    bool enable_point_cloud_export = false;
    bool enable_height_map_export = false;
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
        if( (*arg).compare("--dry") == 0 )
            enable_export = false;
    }

    Utilities::ImageFormat::ImageFormat* the_choosen_r = chooser.getWriterReference( point_cloud_3_channel );

    if( the_choosen_r != nullptr && enable_export ) {
        if( enable_point_cloud_export ) {
            // Write the three heightmaps encoded in three color channels.
            // TODO Find out what to do if the image cannot be written.
            Utilities::Buffer buffer;
            the_choosen_r->write( point_cloud_3_channel, buffer );
            buffer.write( the_choosen_r->appendExtension( file_path ) );
        }
        if( enable_height_map_export ) {
            // Write out the depth field of the Til Resource.
            Utilities::ImageData *heightmap_p = getHeightMap( 8 );
            
            if( heightmap_p != nullptr ) {
                Utilities::Buffer buffer;
                the_choosen_r->write( *heightmap_p, buffer );
                buffer.write( the_choosen_r->appendExtension( std::string( file_path ) + "_height" ) );
            }
        }
    }

    Utilities::ModelBuilder *model_output = createModel( &arguments );

    if( enable_export && model_output != nullptr )
        glTF_return = model_output->write( file_path );

    delete model_output;

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

        Utilities::DataTypes::Vec3 position_displacement;
        Utilities::DataTypes::Vec3 position[6];
        Utilities::DataTypes::Vec3 normal[6];
        Utilities::DataTypes::Vec3 color[6];
        Utilities::DataTypes::Vec2UByte coord[6];

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

                    const Tile current_tile = mesh_tiles.at( t + mesh_reference_grid[x][y].tiles_start );

                    Data::Mission::Til::Mesh::Input input;
                    input.pixels[ FRONT_LEFT  ] = reinterpret_cast<const int8_t*>( point_cloud_3_channel.getPixel( y + 0, x + 0 ) );
                    input.pixels[  BACK_LEFT  ] = reinterpret_cast<const int8_t*>( point_cloud_3_channel.getPixel( y + 1, x + 0 ) );
                    input.pixels[  BACK_RIGHT ] = reinterpret_cast<const int8_t*>( point_cloud_3_channel.getPixel( y + 1, x + 1 ) );
                    input.pixels[ FRONT_RIGHT ] = reinterpret_cast<const int8_t*>( point_cloud_3_channel.getPixel( y + 0, x + 1 ) );
                    input.coord_index = current_tile.texture_cord_index;
                    input.coord_index_limit = this->texture_cords.size();
                    input.coord_data = this->texture_cords.data();

                    Data::Mission::Til::Mesh::VertexData vertex_data;
                    vertex_data.position = position;
                    vertex_data.coords = coord;
                    vertex_data.colors = color;
                    vertex_data.element_amount = 6;
                    vertex_data.element_start = 0;

                    Data::Mission::Til::Colorizer::Input input_color;
                    input_color.tile = this->tile_texture_type.at( current_tile.graphics_type_index );
                    input_color.colors = colors.data();
                    input_color.colors_amount = colors.size();
                    input_color.unk = 0;

                    Data::Mission::Til::Colorizer::setSquareColors( input_color, input.colors );

                    if( input_color.tile.texture_index == texture_index || texture_index == TEXTURE_NAMES_AMOUNT ) {
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

                            // Flip the x-axis.
                            position[ i ].x = -position[ i ].x;
                        }
                    }

                    // Generate the normals
                    {
                        Utilities::DataTypes::Vec3 u;
                        Utilities::DataTypes::Vec3 v;

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
                            
                            normal[3 * p].normalize();

                            // Fill in the value on all points
                            for( unsigned int i = 1; i < 3; i++ ) {
                                normal[3 * p + i ].x = normal[3 * p].x;
                                normal[3 * p + i ].y = normal[3 * p].y;
                                normal[3 * p + i ].z = normal[3 * p].z;
                            }
                        }
                    }

                    {
                        Utilities::DataTypes::ScalarUInt TileMeshValue;
                        TileMeshValue.x = current_tile.mesh_type;

                        // The write loop for the tiles.
                        for( unsigned int p = 0; p < current_tile_polygon_amount; p++ )
                        {
                            model_output->startVertex();

                            model_output->setVertexData(    position_compon_index, Utilities::DataTypes::Vec3Type( position[p] ) );
                            model_output->setVertexData(      normal_compon_index, Utilities::DataTypes::Vec3Type( normal[p] ) );
                            model_output->setVertexData(       color_compon_index, Utilities::DataTypes::Vec3Type( color[p] ) );
                            model_output->setVertexData( tex_coord_0_compon_index, Utilities::DataTypes::Vec2UByteType( coord[p] ) );
                            model_output->setVertexData(   tile_type_compon_index, Utilities::DataTypes::ScalarUIntType( TileMeshValue ) );
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
        Utilities::DataTypes::Vec3 position[6];
        Utilities::DataTypes::Vec2UByte cord[6];
        Utilities::DataTypes::Vec3 color[6];
        
        Tile current_tile;
        Data::Mission::Til::Mesh::Input input;
        Data::Mission::Til::Mesh::VertexData vertex_data;
        
        input.pixels[ FRONT_LEFT  ] = reinterpret_cast<const int8_t*>( point_cloud_3_channel.getPixel( z + 0, x + 0 ) );
        input.pixels[  BACK_LEFT  ] = reinterpret_cast<const int8_t*>( point_cloud_3_channel.getPixel( z + 1, x + 0 ) );
        input.pixels[  BACK_RIGHT ] = reinterpret_cast<const int8_t*>( point_cloud_3_channel.getPixel( z + 1, x + 1 ) );
        input.pixels[ FRONT_RIGHT ] = reinterpret_cast<const int8_t*>( point_cloud_3_channel.getPixel( z + 0, x + 1 ) );
        input.coord_index = current_tile.texture_cord_index;
        input.coord_index_limit = this->texture_cords.size();
        input.coord_data = this->texture_cords.data();
        
        vertex_data.position = position;
        vertex_data.coords = cord;
        vertex_data.colors = color;
        vertex_data.element_amount = 6;
        vertex_data.element_start = 0;
        
        for( auto current_tile_index = 0; current_tile_index < mesh_reference_grid[x][z].tile_amount; current_tile_index++ ) {
            current_tile = mesh_tiles.at( current_tile_index + mesh_reference_grid[x][z].tiles_start );
            
            auto amount_of_vertices = createTile( input, vertex_data, current_tile.mesh_type );
            
            for( unsigned int i = 0; i < amount_of_vertices; i++ ) {
                position[ i ].x += (SPAN_OF_TIL - x) - 0.5;
                position[ i ].z += (SPAN_OF_TIL - z) - 0.5;

                // Flip the x-axis.
                position[ i ].x = -position[ i ].x;
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
    Utilities::DataTypes::Vec3 point;
    Utilities::DataTypes::Vec3 barycentric;
    
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
    // TODO I have an algorithm in mind to make this much faster. It involves using planes and a 2D grid.
    Utilities::Collision::Ray downRay( Utilities::DataTypes::Vec3( x, MAX_HEIGHT, z ), Utilities::DataTypes::Vec3( x, MAX_HEIGHT - 1.0f, z ) );
    
    return getRayCast3D( downRay );
}


const std::vector<Utilities::Collision::Triangle>& Data::Mission::TilResource::getAllTriangles() const {
    return all_triangles;
}

Utilities::ImageData* Data::Mission::TilResource::getHeightMap( unsigned int rays_per_tile ) const {
    auto heightmap_p = new Utilities::ImageData;
    
    heightmap_p->setWidth(  AMOUNT_OF_TILES * rays_per_tile );
    heightmap_p->setHeight( AMOUNT_OF_TILES * rays_per_tile );
    heightmap_p->setFormat( Utilities::ImageData::RED_GREEN_BLUE, 1 );
    auto image_data = reinterpret_cast<uint8_t*>( heightmap_p->getRawImageData() );
    
    const float LENGTH = static_cast<float>(AMOUNT_OF_TILES ) - (1.0f / static_cast<float>( rays_per_tile ));
    const float HALF_LENGTH = LENGTH / 2.0f;
    const float STEPER = LENGTH / static_cast<float>((AMOUNT_OF_TILES * rays_per_tile - 1));
    
    for( unsigned int x = 0; x < AMOUNT_OF_TILES * rays_per_tile; x++ ) {
        
        float x_pos = static_cast<float>(x) * STEPER - HALF_LENGTH;
        
        for( unsigned int z = 0; z < AMOUNT_OF_TILES * rays_per_tile; z++ ) {
            
            float z_pos = static_cast<float>(z) * STEPER - HALF_LENGTH;
            
            float distance = getRayCast2D( x_pos, z_pos );
            
            // This means that no triangles had been hit
            if( distance < 0.0f ) {
                image_data[0] = 255;
                image_data[1] = 0;
                image_data[2] = 0;
            }
            else // This means beyond of range.
            if( distance > 2.0f * MAX_HEIGHT ) {
                image_data[0] = 0;
                image_data[1] = 255;
                image_data[2] = 255;
            }
            else { // This means that the pixel works for the image format.
                distance *= 1.0f / SAMPLE_HEIGHT;
                
                image_data[0] = distance;
                image_data[1] = distance;
                image_data[2] = distance;
            }

            image_data += heightmap_p->getPixelSize();
        }
    }
    
    return heightmap_p;
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
