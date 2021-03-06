#ifndef MISSION_RESOURCE_TILE_MESH_BUILDER_HEADER
#define MISSION_RESOURCE_TILE_MESH_BUILDER_HEADER

#include "../TilResource.h"

namespace Data {

namespace Mission {

namespace Til {

namespace Mesh {
    // Values for the facing_direction
    const unsigned int FRONT_LEFT  = 0b00;
    const unsigned int FRONT_RIGHT = 0b01;
    const unsigned int BACK_LEFT   = 0b10;
    const unsigned int BACK_RIGHT  = 0b11;

    // Values for the heightmap_channel
    const unsigned int RED        = 0;
    const unsigned int GREEN      = 1;
    const unsigned int BLUE       = 2;
    const unsigned int NO_ELEMENT = 3; // used to indicate that the index of the polygon is non existent. If the first point of the polygon is this value then the polygon does not exist.

    struct Point {
        unsigned int facing_direction;
        unsigned int heightmap_channel; // 0 red, 1 green, 2 blue, 3 abort.
        unsigned int texture_coordinate_index;
    };

    struct Polygon {
        bool flip; // Should the software flip the triangle?
        Point points[4];
    };

    struct VertexData {
        glm::vec3 *position;
        glm::u8vec2 *coords;
        glm::vec3 *colors;
        unsigned int element_start;
        unsigned int element_amount; // This describes the structual limit position and coords.
    };

    struct Input {
        const int8_t *pixels[4]; // Almost treated like an RGB 8 bit per channel.
        glm::vec3 colors[4]; // The colors for the tiles.
        unsigned int coord_index;
        unsigned int coord_index_limit;
        const glm::u8vec2 *coord_data; // This is the data that holds the coordinates.
    };

    /**
     * This function gets the next index of a valid polygon for the Til resource.
     * @param index the current index to be advanced.
     * @param next_amount This is the amount of times it will skip to.
     * @return A valid index to the next triangle.
     */
    unsigned int getNeighboor( unsigned int index, int next_amount );

    /**
     * This function loads the mesh builder file. It configures create tile to use the json tile set.
     * I do not normally practice globals, but this solution is only temporay.
     * TODO eventually turn this script into c++ code again.
     * @param filepath the path to this json file.
     * @param error_output If there is a problem the class will put into this. If null then the class will say nothing.
     * @return 1 for success, 0 for incompatibilties and -1 for not found.
     */
    unsigned int loadMeshScript( const char *const filepath, std::ostream *error_output );

    unsigned int BuildTriangle( const Input &input, const Polygon &triangle, VertexData &result );
    unsigned int BuildQuad( const Input &input, const Polygon &quad, VertexData &result );

    unsigned int createTile( const Input &input, VertexData &result, unsigned int tileType );
}

}

}

}

#endif // MISSION_RESOURCE_TILE_MESH_BUILDER_HEADER
