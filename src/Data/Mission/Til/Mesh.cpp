#include "Mesh.h"
#include <fstream>

#include "TileSet.h"

namespace {
    const float TILE_CORNER_POSITION_X[4] = { 0.5, -0.5,  0.5, -0.5 };
    const float TILE_CORNER_POSITION_Z[4] = { 0.5,  0.5, -0.5, -0.5 };
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
            not_found = Data::Mission::Til::TileSet::default_mesh[index].points[0].heightmap_channel == NO_ELEMENT;
        }
    }

    return index;
}

unsigned int Data::Mission::Til::Mesh::BuildTriangle( const Input &input, const Polygon &triangle, VertexData &result, unsigned offset ) {
    const unsigned int ELEMENT_AMOUNT = 3;

    static const unsigned INDEX_TABLE[3][3] = { {0, 1, 2}, { 0, 1, 3 }, { 1, 2, 3 } };
    
    if( result.element_amount >= ELEMENT_AMOUNT + result.element_start )
    {
        for( unsigned int i = 0; i < ELEMENT_AMOUNT; i++ ) {
            
            result.position[ result.element_start ].x = TILE_CORNER_POSITION_X[ triangle.points[ i ].facing_direction ];
            result.position[ result.element_start ].y = static_cast<float>(input.pixels[ triangle.points[ i ].facing_direction ]->channel[ triangle.points[ i ].heightmap_channel ]) * TilResource::SAMPLE_HEIGHT;
            result.position[ result.element_start ].z = TILE_CORNER_POSITION_Z[ triangle.points[ i ].facing_direction ];
            
            const unsigned int INDEX = (input.coord_index + INDEX_TABLE[ offset ][ i ] ) % input.coord_index_limit;
            result.coords[ result.element_start ].x = input.coord_data[ INDEX ].x;
            result.coords[ result.element_start ].y = input.coord_data[ INDEX ].y;

            const unsigned x = ((triangle.points[ i ].facing_direction & FRONT_RIGHT) != 0) + input.x;
            const unsigned y = ((triangle.points[ i ].facing_direction & BACK_LEFT)   != 0) + input.y;

            result.uv_positions[ result.element_start ] = (x % 16) + 16 * (y % 16);

            result.stca_animation_index[ result.element_start ] = 0;

            for( unsigned scta_index = 0; scta_index < input.SCTA_info_r->size(); scta_index++ ) {
                const TilResource::InfoSCTA &info_scta = input.SCTA_info_r->at( scta_index );

                if( info_scta.isMemorySafe() && info_scta.source_uv_offset / 2 <= INDEX && info_scta.source_uv_offset / 2 + 4 > INDEX) {
                    // result.stca_animation_index[ result.element_start ] = 1 + scta_index;
                    result.stca_animation_index[ result.element_start ] = 1 + 4 * scta_index + INDEX_TABLE[ offset ][ i ];
                }
            }

            result.element_start++;
        }

        return ELEMENT_AMOUNT;
    }
    else
        return 0;
}

unsigned int Data::Mission::Til::Mesh::BuildQuad( const Input &input, const Polygon &quad, VertexData &result ) {
    unsigned int number_of_written_vertices = 0;

    Polygon other_triangle;
    other_triangle.points[0] = quad.points[0];
    other_triangle.points[1] = quad.points[1];
    other_triangle.points[2] = quad.points[3];

    number_of_written_vertices += Data::Mission::Til::Mesh::BuildTriangle( input, other_triangle, result, 1 );

    other_triangle.points[0] = quad.points[1];
    other_triangle.points[1] = quad.points[2];
    other_triangle.points[2] = quad.points[3];

    number_of_written_vertices += Data::Mission::Til::Mesh::BuildTriangle( input, other_triangle, result, 2 );

    return number_of_written_vertices;
}

unsigned int Data::Mission::Til::Mesh::createTile( const Input &input, VertexData &vertex_data, unsigned int tileType ) {
    unsigned number_of_written_vertices = 0;
    Polygon tile_polygon;

    tile_polygon = Data::Mission::Til::TileSet::default_mesh[ tileType ];

    if( tile_polygon.points[0].heightmap_channel != NO_ELEMENT )
    {
        vertex_data.colors[ 0 ] = input.colors[ 0 ];
        vertex_data.colors[ 1 ] = input.colors[ 1 ];
        vertex_data.colors[ 2 ] = input.colors[ 2 ];

        if( tile_polygon.points[3].heightmap_channel == NO_ELEMENT )
            number_of_written_vertices += BuildTriangle( input, tile_polygon, vertex_data );
        else
        {
            vertex_data.colors[ 0 ] = input.colors[ 0 ];
            vertex_data.colors[ 1 ] = input.colors[ 1 ];
            vertex_data.colors[ 2 ] = input.colors[ 3 ];
            vertex_data.colors[ 3 ] = input.colors[ 1 ];
            vertex_data.colors[ 4 ] = input.colors[ 2 ];
            vertex_data.colors[ 5 ] = input.colors[ 3 ];
            number_of_written_vertices += BuildQuad( input, tile_polygon, vertex_data );
        }
    }


    return number_of_written_vertices;
}

bool Data::Mission::Til::Mesh::isWall( unsigned int tile_type ) {
    const unsigned ARRAY_LENGTH = 4;
    unsigned number_array[ARRAY_LENGTH] = {0, 0, 0, 0};
    unsigned number_of_twos = 0;
    unsigned number_of_ones = 0;

    // Buffer overflow check.
    if( tile_type >= Data::Mission::Til::TileSet::POLYGON_COUNT ) {
        return false;
    }

    unsigned number_of_corners = 4;

    if( Data::Mission::Til::TileSet::default_mesh[tile_type].points[3].heightmap_channel == NO_ELEMENT )
        number_of_corners = 3;

    for( unsigned i = 0; i < number_of_corners; i++ ) {
        number_array[ Data::Mission::Til::TileSet::default_mesh[tile_type].points[i].facing_direction ]++;
    }

    if( number_of_corners == 3 ) { // 3 side case.
        for( unsigned i = 0; i < ARRAY_LENGTH; i++ ) {
            if( number_array[ i ] == 0 ) { // Do nothing.
            } else
            if( number_array[ i ] == 1 ) { // Increment the number of ones.
                number_of_ones++;
            } else
            if( number_array[ i ] == 2 ) { // Increment the number of twos.
                number_of_twos++;
            } else
                return false; // Abort this search.
        }

        if( number_of_ones == 1 && number_of_twos == 1 )
            return true; // Succeeded.
        else
            return false;
    }
    else { // 4 side case.
        for( unsigned i = 0; i < ARRAY_LENGTH; i++ ) {
            if( number_array[ i ] == 0 ) { // Do nothing.
            } else
            if( number_array[ i ] == 1 ) { // This is not a wall then.
                return false;
            } else
            if( number_array[ i ] == 2 ) { // Increment the number of twos.
                number_of_twos++;
            } else
                return false; // Abort this search.
        }

        if( number_of_twos == 2 )
            return true; // Succeeded.
        else
            return false;
    }
}

bool Data::Mission::Til::Mesh::isSlope( unsigned int tile_type ) {
    unsigned number_of_corners = 4;

    // Buffer overflow check.
    if( tile_type >= Data::Mission::Til::TileSet::POLYGON_COUNT ) {
        return false;
    }

    if( Data::Mission::Til::TileSet::default_mesh[tile_type].points[0].heightmap_channel == NO_ELEMENT )
        return false;

    if( Data::Mission::Til::TileSet::default_mesh[tile_type].points[3].heightmap_channel == NO_ELEMENT )
        number_of_corners = 3;

    for( unsigned i = 1; i < number_of_corners; i++ ) {
        if( Data::Mission::Til::TileSet::default_mesh[tile_type].points[ 0 ].heightmap_channel != Data::Mission::Til::TileSet::default_mesh[tile_type].points[ i ].heightmap_channel )
            return true;
    }
    return false;
}


bool Data::Mission::Til::Mesh::isFlipped( unsigned int tile_type ) {
    // Buffer overflow check.
    if( tile_type >= Data::Mission::Til::TileSet::POLYGON_COUNT ) {
        return false;
    }

    return Data::Mission::Til::TileSet::default_mesh[tile_type].is_opposite;
}
