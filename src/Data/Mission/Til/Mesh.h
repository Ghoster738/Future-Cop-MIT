#ifndef MISSION_RESOURCE_TILE_MESH_BUILDER_HEADER
#define MISSION_RESOURCE_TILE_MESH_BUILDER_HEADER

#include "../TilResource.h"

namespace Data {

namespace Mission {

namespace Til {

namespace Mesh {
    // Values for the facing_direction
    const unsigned FRONT_LEFT  = 0b00;
    const unsigned FRONT_RIGHT = 0b01;
    const unsigned BACK_LEFT   = 0b10;
    const unsigned BACK_RIGHT  = 0b11;

    // Values for the heightmap_channel
    const unsigned RED        = 0;
    const unsigned GREEN      = 1;
    const unsigned BLUE       = 2;
    const unsigned NO_ELEMENT = 3; // used to indicate that the index of the polygon is non existent. If the first point of the polygon is this value then the polygon does not exist.

    struct Point {
        unsigned facing_direction;
        unsigned heightmap_channel; // 0 red, 1 green, 2 blue, 3 abort.
    };

    struct Polygon {
        Point points[4];
        bool is_opposite;
    };

    struct VertexData {
        glm::vec3 *position;
        glm::u8vec2 *coords;
        glm::vec3 *colors;
        unsigned *stca_animation_index; // 0 means normal, 1 and above is index - 1.
        unsigned *uv_positions;
        unsigned element_start;
        unsigned element_amount; // This describes the structual limit position and coords.
    };

    struct Input {
        const TilResource::HeightmapPixel* pixels[4]; // Almost treated like an RGB 8 bit per channel.
        glm::vec3 colors[4]; // The colors for the tiles.
        unsigned coord_index;
        unsigned coord_index_limit;
        unsigned x, y;
        const glm::u8vec2 *coord_data; // This is the data that holds the coordinates.
        const std::vector<TilResource::InfoSCTA> *SCTA_info_r;
    };

    /**
     * This function gets the next index of a valid polygon for the Til resource.
     * @param index the current index to be advanced.
     * @param next_amount This is the amount of times it will skip to.
     * @return A valid index to the next triangle.
     */
    unsigned getNeighboor( unsigned index, int next_amount );

    unsigned BuildTriangle( const Input &input, const Polygon &triangle, VertexData &result, unsigned offset = 0 );
    unsigned BuildQuad( const Input &input, const Polygon &quad, VertexData &result );

    unsigned createTile( const Input &input, VertexData &result, unsigned tileType );

    bool isWall( unsigned tile_type );

    bool isSlope( unsigned tile_type );

    bool isFlipped( unsigned tile_type );
}

}

}

}

#endif // MISSION_RESOURCE_TILE_MESH_BUILDER_HEADER
