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
