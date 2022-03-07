#ifndef MISSION_RESOURCE_TILE_COLORIZER_HEADER
#define MISSION_RESOURCE_TILE_COLORIZER_HEADER

#include "../TilResource.h"

namespace Data {

namespace Mission {

namespace Til {

namespace Colorizer {

    struct Input {
        Data::Mission::TilResource::TileGraphics tile;
        const uint16_t *colors;
        size_t colors_amount;
        int unk;
    };

    /**
     * This function loads the mesh builder file. It configures create tile to use the json tile set.
     * I do not normally practice globals, but this solution is only temporay.
     * TODO eventually turn this script into c++ code too.
     * @param filepath the path to this json file.
     * @param error_output If there is a problem the class will put into this. If null then the class will say nothing.
     * @return 1 for success, 0 for incompatibilties and -1 for not found.
     */
    unsigned int loadShadowScript( const char *const filepath, std::ostream *error_output );

    /**
     * Set the colors for the square.
     * @param input This is where the inputs go.
     * @param result This is where the colors will be placed to. ALWAYS make sure that result refers to 4 vectors.
     * @return 1 for success, or 0 for failure, or -1 for nullptr
     */
    unsigned int setSquareColors( const Input &input, Utilities::DataTypes::Vec3 *result );
}

}

}

}

#endif // MISSION_RESOURCE_TILE_COLORIZER_HEADER
