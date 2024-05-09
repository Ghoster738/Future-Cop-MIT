#ifndef MISSION_RESOURCE_TILE_CULLING_GENERATOR_HEADER
#define MISSION_RESOURCE_TILE_CULLING_GENERATOR_HEADER

#include "../TilResource.h"

namespace Data {

namespace Mission {

namespace Til {

namespace CullingGenerator {
    TilResource::CullingData create(
        Utilities::GridBase2D<TilResource::HeightmapPixel> &point_cloud_3_channel,
        std::vector<TilResource::Tile> &mesh_tiles,
        TilResource::Floor (&mesh_reference_grid)[ TilResource::AMOUNT_OF_TILES ][ TilResource::AMOUNT_OF_TILES ] );
}

}

}

}

#endif // MISSION_RESOURCE_TILE_CULLING_GENERATOR_HEADER
