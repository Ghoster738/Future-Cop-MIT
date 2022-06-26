#include "Mesh.h"
#include <fstream>
#include <json/json.h>

namespace {
    const float TILE_CORNER_POSITION_X[4] = { 0.5, -0.5,  0.5, -0.5 };
    const float TILE_CORNER_POSITION_Z[4] = { 0.5,  0.5, -0.5, -0.5 };

    #include "TileSet.h"
}


unsigned int Data::Mission::Til::Mesh::getNeighboor( unsigned int index, int next_amount ) {
    int increment;
    bool not_found;

    if( next_amount > 0)
        increment = -1;
    else
        increment =  1;

    for( int timeout = 0; 0 != next_amount && timeout < 0x80; next_amount += increment, timeout++ ) {
        not_found = true;

        while( not_found ) {
            index -= increment;
            index = index & 0x7F;
            not_found = default_mesh[index].points[0].heightmap_channel == NO_ELEMENT;
        }
    }

    return index;
}

unsigned int Data::Mission::Til::Mesh::loadMeshScript( const char *const filepath, std::ostream *error_output ) {
    std::ifstream file_loaded;
    unsigned int result;

    file_loaded.open( filepath );

    if( file_loaded.is_open() )
    {
        Json::Value root;
        Json::CharReaderBuilder reader_settings;
        std::string error_stream;

        if( Json::parseFromStream( reader_settings, file_loaded, &root, &error_stream ) )
        {
            Json::Value tileset_type = root["FutureCopReverse"]["type"];
            Json::Value tiles = root["tiles"];

            if( tileset_type.isString() && tileset_type.asString().compare("tileSet") == 0 ) {
                Json::Value version = root["FutureCopReverse"]["version"];

                if( version.isInt() && version.asInt() != 0 && error_output != nullptr)
                {
                    *error_output << "Warning: This json file version does not match the current version." << std::endl;
                }

                if( tiles.isArray() ) {
                    // Finally loading code.

                    // This should be first cleaned of all the old stuff.
                    for( unsigned int i = 0; i < 0x80; i++ ) {
                        default_mesh[ i ].points[ 0 ].heightmap_channel = NO_ELEMENT; // This means that the value was not set
                    }

                    for( auto current_tile = tiles.begin(); current_tile != tiles.end(); current_tile++ ) {
                        Json::Value id = (*current_tile)["id"];
                        Json::Value flip = (*current_tile)["flip"];
                        Json::Value polygon = (*current_tile)["polygon"];

                        if( id.isInt() && polygon.isArray() && polygon.size() >= 3 ) {
                            Polygon poly;

                            // This is the defaults of the polygon
                            poly.flip = false;
                            poly.points[3].heightmap_channel = NO_ELEMENT; // This means that this polygon is a triangle by default.

                            // This field is optional!
                            if( flip.isInt() )
                                poly.flip = (flip.asInt() == 1);

                            // Do not go beyond four points unless you want a buffer overflow.
                            unsigned int amount = polygon.size();
                            if(amount > 4)
                                amount = 4;

                            for( unsigned int i = 0; i < amount; i++ ) {
                                Json::Value position = polygon[i]["position"];
                                Json::Value heightmapChannel = polygon[i]["heightmap_channel"];
                                Json::Value textureCoordinateIndex = polygon[i]["coord_index"];

                                // Ignore data that is not compatible only flare can be excluded.
                                if( position.isString() && heightmapChannel.isInt() && textureCoordinateIndex.isInt() ) {
                                    std::string position_string = position.asString();
                                    if( position_string.compare("FRONT_LEFT") == 0 )
                                        poly.points[ i ].facing_direction = FRONT_LEFT;
                                    else
                                    if( position_string.compare("FRONT_RIGHT") == 0 )
                                        poly.points[ i ].facing_direction = FRONT_RIGHT;
                                    else
                                    if( position_string.compare("BACK_LEFT") == 0 )
                                        poly.points[ i ].facing_direction = BACK_LEFT;
                                    else
                                    if( position_string.compare("BACK_RIGHT") == 0 )
                                        poly.points[ i ].facing_direction = BACK_RIGHT;

                                    poly.points[ i ].heightmap_channel = heightmapChannel.asUInt();
                                    poly.points[ i ].texture_coordinate_index = textureCoordinateIndex.asUInt();
                                }
                            }

                            default_mesh[ id.asInt() ] = poly;
                        }
                    }
                }
                else
                {
                    *error_output << "This json file must have arrays." << std::endl;
                    result = 0;
                }

                result = 1; // For success!
            }
            else
            {
                result = 0;
                if( error_output != nullptr ) {
                    *error_output << "This json file is not a tile set json. In short the wrong json file is loaded." << std::endl;
                }
            }
        }
        else
        {
            result = 0;
            if( error_output != nullptr ) {
                *error_output << "There is an error found with json parsing." << std::endl
                              << "    " << error_stream << std::endl;
            }
        }
    }
    else
    {
        result = -1;
        if( error_output != nullptr ) {
            *error_output << "The file \"" << filepath << "\" did not even open." << std::endl;
        }
    }

    return result;
}

unsigned int Data::Mission::Til::Mesh::BuildTriangle( const Input &input, const Polygon &triangle, VertexData &result ) {
    const unsigned int ELEMENT_AMOUNT = 3;

    if( result.element_amount >= ELEMENT_AMOUNT + result.element_start )
    {
        for( unsigned int i = 0; i < ELEMENT_AMOUNT; i++ ) {
            result.position[ result.element_start ].x = TILE_CORNER_POSITION_X[ triangle.points[ i ].facing_direction ];
            result.position[ result.element_start ].y = static_cast<float>(input.pixels[ triangle.points[ i ].facing_direction ][ triangle.points[ i ].heightmap_channel ]) * 0.05f;
            result.position[ result.element_start ].z = TILE_CORNER_POSITION_Z[ triangle.points[ i ].facing_direction ];

            result.coords[ result.element_start ].x = input.coord_data[ triangle.points[ i ].texture_coordinate_index ].x;
            result.coords[ result.element_start ].y = input.coord_data[ triangle.points[ i ].texture_coordinate_index ].y;

            result.element_start++;
        }

        if( triangle.flip ) {
            std::swap( result.position[ result.element_start - 2 ], result.position[ result.element_start - 3 ] );
            std::swap( result.coords[ result.element_start - 2 ], result.coords[ result.element_start - 3 ] );
        }

        return ELEMENT_AMOUNT;
    }
    else
        return 0;
}

unsigned int Data::Mission::Til::Mesh::BuildQuad( const Input &input, const Polygon &quad, VertexData &result ) {
    unsigned int number_of_written_vertices = 0;

    number_of_written_vertices += Data::Mission::Til::Mesh::BuildTriangle( input, quad, result );

    Polygon other_triangle;
    other_triangle.flip = quad.flip;
    other_triangle.points[0] = quad.points[2];
    other_triangle.points[1] = quad.points[3];
    other_triangle.points[2] = quad.points[0];

    number_of_written_vertices += Data::Mission::Til::Mesh::BuildTriangle( input, other_triangle, result );

    return number_of_written_vertices;
}

unsigned int Data::Mission::Til::Mesh::createTile( const Input &input, VertexData &vertex_data, unsigned int tileType ) {
    unsigned number_of_written_vertices = 0;
    bool found = false;
    Polygon tile_polygon;

    tile_polygon = default_mesh[ tileType ];

    tile_polygon.points[0].texture_coordinate_index += input.coord_index;
    tile_polygon.points[0].texture_coordinate_index %= input.coord_index_limit;
    tile_polygon.points[1].texture_coordinate_index += input.coord_index;
    tile_polygon.points[1].texture_coordinate_index %= input.coord_index_limit;
    tile_polygon.points[2].texture_coordinate_index += input.coord_index;
    tile_polygon.points[2].texture_coordinate_index %= input.coord_index_limit;
    tile_polygon.points[3].texture_coordinate_index += input.coord_index;
    tile_polygon.points[3].texture_coordinate_index %= input.coord_index_limit;

    if( tile_polygon.points[0].heightmap_channel != NO_ELEMENT )
    {
        vertex_data.colors[ 0 ] = input.colors[ 0 ];
        vertex_data.colors[ 1 ] = input.colors[ 1 ];
        vertex_data.colors[ 2 ] = input.colors[ 2 ];

        if( tile_polygon.points[3].heightmap_channel == NO_ELEMENT )
            number_of_written_vertices += BuildTriangle( input, tile_polygon, vertex_data );
        else
        {
            vertex_data.colors[ 3 ] = input.colors[ 2 ];
            vertex_data.colors[ 4 ] = input.colors[ 3 ];
            vertex_data.colors[ 5 ] = input.colors[ 0 ];
            number_of_written_vertices += BuildQuad( input, tile_polygon, vertex_data );
        }
    }


    return number_of_written_vertices;
}
