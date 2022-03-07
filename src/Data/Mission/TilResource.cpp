#include "TilResource.h"
#include "Til/Mesh.h"
#include "Til/Colorizer.h"

#include "IFF.h"

#include "../../Utilities/DataHandler.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cassert>

namespace {
uint32_t LITTLE_SECT = 0x53656374; // which is { 0x53, 0x65, 0x63, 0x74 } or { 'S', 'e', 'c', 't' } or "Sect";
void readCullingTile( Data::Mission::TilResource::CullingTile &tile, uint8_t *data ) {
    tile.top_left = Utilities::DataHandler::read_u16_little( data );
    data += sizeof( uint16_t );
    // Unknown data.
    data += sizeof( uint16_t );
    tile.top_right = Utilities::DataHandler::read_u16_little( data );
    data += sizeof( uint16_t );
    // Unknown data.
    data += sizeof( uint16_t );
    tile.bottom_left = Utilities::DataHandler::read_u16_little( data );
    data += sizeof( uint16_t );
    // Unknown data.
    data += sizeof( uint16_t );
    tile.bottom_right = Utilities::DataHandler::read_u16_little( data );
}
}

const std::string Data::Mission::TilResource::FILE_EXTENSION = "til";
const uint32_t Data::Mission::TilResource::IDENTIFIER_TAG = 0x4374696C; // which is { 0x43, 0x74, 0x69, 0x6C } or { 'C', 't', 'i', 'l' } or "Ctil"

Data::Mission::TilResource::TilResource() {

}

Data::Mission::TilResource::TilResource( const TilResource &obj ) : ModelResource( obj ), image( obj.image ) {
}

std::string Data::Mission::TilResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::TilResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

Utilities::ImageData *const Data::Mission::TilResource::getImage() const {
    return const_cast<Utilities::ImageData *const>(&image);
}

bool Data::Mission::TilResource::parse( const Utilities::Buffer &header, const Utilities::Buffer &reader_data, const ParseSettings &settings ) {
    auto raw_data = reader_data.getReader().getBytes();

    bool file_is_not_valid = false;
    auto data = raw_data.data();

    while( static_cast<unsigned int>(data - raw_data.data()) < raw_data.size() ) {
        auto identifier = Utilities::DataHandler::read_u32( data, settings.is_opposite_endian );
        auto tag_size   = Utilities::DataHandler::read_u32( data + sizeof( uint32_t ), settings.is_opposite_endian );

        if( identifier == LITTLE_SECT ) {
            auto color_amount = Utilities::DataHandler::read_u16( data + sizeof( uint32_t ) * 2, settings.is_opposite_endian );
            auto texture_cordinates_amount = Utilities::DataHandler::read_u16( data + sizeof( uint32_t ) * 2 + sizeof( uint16_t ), settings.is_opposite_endian );
            auto image_read_head = data + sizeof( uint32_t ) * 2 + sizeof( uint16_t ) * 2;

            
            if( settings.output_level >= 3 )
            {
                *settings.output_ref << "Mission::TilResource::load() id = " << getIndexNumber() << std::endl;
                *settings.output_ref << "Mission::TilResource::load() loc = 0x" << std::hex << getOffset() << std::dec << std::endl;
                *settings.output_ref << "Color amount = " << color_amount << std::endl;
                *settings.output_ref << "texture_cordinates_amount = " << texture_cordinates_amount << std::endl;
                *settings.output_ref << "texture_quads_amount = " << texture_cordinates_amount / 4 << std::endl;
            }

            // setup the image
            image.setWidth( 0x11 );
            image.setHeight( 0x11 );
            image.setFormat( Utilities::ImageData::RED_GREEN_BLUE, 1 );

            auto image_data = image.getRawImageData();
            for( unsigned int a = 0; a < image.getWidth() * image.getHeight(); a++ ) {

                image_data[0] = image_read_head[0];
                image_data[1] = image_read_head[1];
                image_data[2] = image_read_head[2];

                image_data += image.getPixelSize();
                image_read_head += image.getPixelSize();
            }

            // These bytes seems to be only five zero bytes
            image_read_head += sizeof( uint8_t ) * 5;

            auto back = image_read_head;

            this->culling_distance = Utilities::DataHandler::read_u16( image_read_head, settings.is_opposite_endian );
            image_read_head += sizeof( uint16_t );
            // Padding?
            image_read_head += sizeof( uint16_t );
            this->culling_top_left.primary = Utilities::DataHandler::read_u16( image_read_head, settings.is_opposite_endian );
            image_read_head += sizeof( uint16_t );
            // Padding?
            image_read_head += sizeof( uint16_t );
            this->culling_top_right.primary = Utilities::DataHandler::read_u16( image_read_head, settings.is_opposite_endian );
            image_read_head += sizeof( uint16_t );
            // Padding?
            image_read_head += sizeof( uint16_t );
            this->culling_bottom_left.primary = Utilities::DataHandler::read_u16( image_read_head, settings.is_opposite_endian );
            image_read_head += sizeof( uint16_t );
            // Padding?
            image_read_head += sizeof( uint16_t );
            this->culling_bottom_right.primary = Utilities::DataHandler::read_u16( image_read_head, settings.is_opposite_endian );
            image_read_head += sizeof( uint16_t );
            // Padding?
            image_read_head += sizeof( uint16_t );

            readCullingTile( culling_top_left, image_read_head );
            image_read_head += 0x10;
            readCullingTile( culling_top_right, image_read_head );
            image_read_head += 0x10;
            readCullingTile( culling_bottom_left, image_read_head );
            image_read_head += 0x10;
            readCullingTile( culling_bottom_right, image_read_head );
            image_read_head += 0x10;

            auto what1 = Utilities::DataHandler::read_u16( image_read_head, settings.is_opposite_endian ) & 0xFF;

            // Modifiying this to be other than what it is will cause an error?
            if( what1 != 0 && settings.output_level >= 1 )
                *settings.output_ref << "Error expected zero in the Til resource." << std::endl;

            image_read_head += sizeof( uint16_t );
            this->texture_reference = Utilities::DataHandler::read_u16( image_read_head, settings.is_opposite_endian );
            image_read_head += sizeof( uint16_t );

            const unsigned int SIZE_OF_GRID = sizeof( mesh_reference_grid[0] ) / sizeof( mesh_reference_grid[0][0] );

            for( unsigned int x = 0; x < SIZE_OF_GRID; x++ ) {
                for( unsigned int y = 0; y < SIZE_OF_GRID; y++ ) {
                    mesh_reference_grid[x][y].floor = Utilities::DataHandler::read_u16( image_read_head, settings.is_opposite_endian );
                    image_read_head += sizeof( uint16_t );
                }
            }

            this->mesh_library_size = Utilities::DataHandler::read_u16( image_read_head, settings.is_opposite_endian );
            image_read_head += sizeof( uint16_t );
            image_read_head += sizeof( uint16_t );

            const size_t ACTUAL_MESH_LIBRARY_SIZE = (this->mesh_library_size >> 4) / sizeof( uint32_t );

            mesh_tiles.reserve( ACTUAL_MESH_LIBRARY_SIZE );

            for( size_t i = 0; i < ACTUAL_MESH_LIBRARY_SIZE; i++ ) {
                mesh_tiles.push_back( { Utilities::DataHandler::read_u32( image_read_head, settings.is_opposite_endian ) } );
                image_read_head += sizeof( uint32_t );
            }
            
            bool skipped_space = false;

            // There are dead uvs that are not being used!
            while( Utilities::DataHandler::read_u32( image_read_head ) == 0 ) {
                image_read_head += sizeof( uint32_t );
                skipped_space = true;
            }

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

                texture_cords.back().x = Utilities::DataHandler::read_u8( image_read_head + 0 );
                texture_cords.back().y = Utilities::DataHandler::read_u8( image_read_head + 1 );

                image_read_head += sizeof( uint16_t );
            }

            // TODO Find out what this shader data does or if it is even shading data.
            colors.reserve( color_amount );

            uint16_t colorful[] = { 0x7c00, 0x03e0, 0x001f, 0x7c1f };

            for( size_t i = 0; i < color_amount; i++ ) {
                colors.push_back( Utilities::DataHandler::read_u16( image_read_head, settings.is_opposite_endian ) );
                image_read_head += sizeof( uint16_t );
            }

            // Read the texture_references, and shading info.
            const size_t TILE_TEXTURE_TYPE_LENGTH = (raw_data.size() - ( image_read_head - raw_data.data() )) / sizeof( uint16_t );

            tile_texture_type.reserve( TILE_TEXTURE_TYPE_LENGTH );
            for( size_t i = 0; i < TILE_TEXTURE_TYPE_LENGTH; i++ ) {
                TileGraphics grp;
                grp.tile_graphics = Utilities::DataHandler::read_u16( image_read_head, settings.is_opposite_endian );

                tile_texture_type.push_back( { Utilities::DataHandler::read_u16( image_read_head, settings.is_opposite_endian ) } );
                image_read_head += sizeof( uint16_t );
            }
        }

        data += tag_size;
    }

    return !file_is_not_valid;
}

Data::Mission::Resource * Data::Mission::TilResource::duplicate() const {
    return new Data::Mission::TilResource( *this );
}

using Data::Mission::Til::Mesh::BACK_LEFT;
using Data::Mission::Til::Mesh::BACK_RIGHT;
using Data::Mission::Til::Mesh::FRONT_RIGHT;
using Data::Mission::Til::Mesh::FRONT_LEFT;

int Data::Mission::TilResource::write( const char *const file_path, const std::vector<std::string> & arguments ) const {
    bool enable_height_map_export = false;
    bool enable_export = true;
    int glTF_return = 0;

    for( auto arg = arguments.begin(); arg != arguments.end(); arg++ ) {
        if( (*arg).compare("--TIL_EXPORT_HEIGHT_MAP") == 0 )
            enable_height_map_export = true;
        else
        if( (*arg).compare("--dry") == 0 )
            enable_export = false;
    }

    if( enable_height_map_export && enable_export )
    {
        // Write the three heightmaps
        std::string file_path_texture = std::string( file_path ) + ".png";
        image.write( file_path_texture.c_str() ); // TODO Find out what to do if the image cannot be written.
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
        // Now, export it in the glTF format.
        Utilities::ModelBuilder *model_output = new Utilities::ModelBuilder();
        bool display_unread = false;
        bool has_displayed = false;
        bool has_texture_displayed = false;

        if( arguments != nullptr )
        {
            for( auto i = arguments->begin(); i != arguments->end(); i++ )
            {
                if( (*i).compare( "--sayMissingAll" ) == 0)
                    display_unread = true;
                else
                if( (*i).find( "--sayMissing=" ) == 0)
                {
                    std::string sub = (*i).substr( 13, (*i).length() - 13 );
                    if( std::stoi(sub) == getIndexNumber() )
                        display_unread = true;
                }
            }
        }

        unsigned int position_compon_index = model_output->addVertexComponent( Utilities::ModelBuilder::POSITION_COMPONENT_NAME, Utilities::DataTypes::ComponentType::FLOAT, Utilities::DataTypes::Type::VEC3 );
        unsigned int normal_compon_index = model_output->addVertexComponent( Utilities::ModelBuilder::NORMAL_COMPONENT_NAME, Utilities::DataTypes::ComponentType::FLOAT, Utilities::DataTypes::Type::VEC3 );
        unsigned int color_compon_index = model_output->addVertexComponent( Utilities::ModelBuilder::COLORS_0_COMPONENT_NAME, Utilities::DataTypes::ComponentType::FLOAT, Utilities::DataTypes::Type::VEC3 );
        unsigned int tex_coord_0_compon_index = model_output->addVertexComponent( Utilities::ModelBuilder::TEX_COORD_0_COMPONENT_NAME, Utilities::DataTypes::ComponentType::UNSIGNED_BYTE, Utilities::DataTypes::Type::VEC2, true );
        unsigned int tile_type_compon_index = model_output->addVertexComponent( "_TileType", Utilities::DataTypes::ComponentType::INT, Utilities::DataTypes::SCALAR, false );

        model_output->setupVertexComponents();

        Utilities::DataTypes::Vec3 position_displacement;
        Utilities::DataTypes::Vec3 position_floor_space[4];
        Utilities::DataTypes::Vec3 position[6];
        Utilities::DataTypes::Vec3 normal[6];
        Utilities::DataTypes::Vec3 color[6];
        Utilities::DataTypes::Vec2UByte coord[6];

        position_floor_space[FRONT_LEFT].x =  0.5;
        position_floor_space[FRONT_LEFT].z =  0.5;

        position_floor_space[FRONT_RIGHT].x = -0.5;
        position_floor_space[FRONT_RIGHT].z =  0.5;

        position_floor_space[BACK_LEFT].x =  0.5;
        position_floor_space[BACK_LEFT].z = -0.5;

        position_floor_space[BACK_RIGHT].x = -0.5;
        position_floor_space[BACK_RIGHT].z = -0.5;

        position_displacement.x = 8.0;
        position_displacement.y = 0.0;
        position_displacement.z = 8.0;

        unsigned int current_tile_polygon_amount = 0;
        unsigned int material_index = 0;

        TileGraphics tileGraphic;

        for( unsigned int a = 0; a < 0x8; a++ ) {
            bool is_first = true;

            unsigned int mesh_tile_index = 0;

            has_texture_displayed = false;

            for( unsigned int x = 0; x < 16; x++ ) {
                for( unsigned int y = 0; y < 16; y++ ) {
                    for( auto t = 0; t < mesh_reference_grid[x][y].tile_amount; t++ )
                    {
                        current_tile_polygon_amount = 0;

                        const Tile current_tile = mesh_tiles.at( t + mesh_reference_grid[x][y].tiles_start );

                        Data::Mission::Til::Mesh::Input input;
                        input.pixels[ FRONT_LEFT  ] = reinterpret_cast<const int8_t*>( image.getPixel( y + 0, x + 0 ) );
                        input.pixels[  BACK_LEFT  ] = reinterpret_cast<const int8_t*>( image.getPixel( y + 1, x + 0 ) );
                        input.pixels[  BACK_RIGHT ] = reinterpret_cast<const int8_t*>( image.getPixel( y + 1, x + 1 ) );
                        input.pixels[ FRONT_RIGHT ] = reinterpret_cast<const int8_t*>( image.getPixel( y + 0, x + 1 ) );
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

                        if( input_color.tile.texture_index == a ) {
                            current_tile_polygon_amount = createTile( input, vertex_data, current_tile.mesh_type );

                            if( current_tile_polygon_amount == 0 && display_unread ) {
                                if( !has_displayed ) {
                                    std::cout << "Starting error log for " << this->getIndexNumber() << std::endl;
                                    has_displayed = true;
                                }

                                if( !has_texture_displayed ) {
                                    std::cout << "For texture index of " << a << std::endl;
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

                            if( is_first && current_tile_polygon_amount != 0 ) {
                                model_output->setMaterial( a );

                                is_first = false;
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

                                model_output->setVertexData( position_compon_index, Utilities::DataTypes::Vec3Type( position[p] ) );
                                model_output->setVertexData( normal_compon_index, Utilities::DataTypes::Vec3Type( normal[p] ) );
                                model_output->setVertexData( color_compon_index,  Utilities::DataTypes::Vec3Type( color[p] ) );
                                model_output->setVertexData( tex_coord_0_compon_index, Utilities::DataTypes::Vec2UByteType( coord[p] ) );
                                model_output->setVertexData( tile_type_compon_index, Utilities::DataTypes::ScalarUIntType( TileMeshValue ) );
                            }
                        }
                    }

                    position_displacement.z -= 1.0;
                }
                position_displacement.x -= 1.0;
                position_displacement.z = 8.0;
            }
            position_displacement.x = 8.0;
        }

        model_output->finish();

        return model_output;
    }
    else
        return nullptr;
}

bool test_MissionTilResource() {
    bool is_correct = true;

    Data::Mission::TilResource::Tile floor = { 0x05e28011 };

    // First test if my getters are correct!
    if( floor.graphics_type_index != 23 ) {
        std::cout << "graphics_type_index is wrong!" << std::endl;
        is_correct = false;
    }

    if( floor.texture_cord_index != 8 ) {
        std::cout << "texture_cord_index is wrong!" << std::endl;
        is_correct = false;
    }

    if( floor.mesh_type != 0x44 ) {
        std::cout << "mesh_type is wrong!" << std::endl;
        is_correct = false;
    }

    if( floor.unknown_0 != 1 ) {
        std::cout << "unknown_0 is wrong!" << std::endl;
        is_correct = false;
    }

    return is_correct;
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
