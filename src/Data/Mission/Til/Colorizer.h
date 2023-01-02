#ifndef MISSION_RESOURCE_TILE_COLORIZER_HEADER
#define MISSION_RESOURCE_TILE_COLORIZER_HEADER

#include "../TilResource.h"

namespace Data {

namespace Mission {

namespace Til {

namespace Colorizer {

    struct Input {
        const std::vector<Utilities::PixelFormatColor::GenericColor> &colors;
        Data::Mission::TilResource::TileGraphics tile;
        int unk;
    };

    /**
     * Set the colors for the square.
     * @param input This is where the inputs go.
     * @param result This is where the colors will be placed to. ALWAYS make sure that result refers to 4 vectors.
     * @return 1 for success, or 0 for failure, or -1 for nullptr
     */
    unsigned int setSquareColors( const Input &input, glm::vec3 *result_r );
}

}

}

}

#endif // MISSION_RESOURCE_TILE_COLORIZER_HEADER
