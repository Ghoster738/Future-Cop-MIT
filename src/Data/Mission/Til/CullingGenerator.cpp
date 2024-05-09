#include "CullingGenerator.h"
#include <fstream>
#include <algorithm>

#include "TileSet.h"

#include <iostream>

using Data::Mission::Til::Mesh::BACK_LEFT;
using Data::Mission::Til::Mesh::BACK_RIGHT;
using Data::Mission::Til::Mesh::FRONT_RIGHT;
using Data::Mission::Til::Mesh::FRONT_LEFT;

Data::Mission::TilResource::CullingData Data::Mission::Til::CullingGenerator::create(
        Utilities::GridBase2D<TilResource::HeightmapPixel> &point_cloud_3_channel,
        std::vector<TilResource::Tile> &mesh_tiles,
        TilResource::Floor (&mesh_reference_grid)[ TilResource::AMOUNT_OF_TILES ][ TilResource::AMOUNT_OF_TILES ] ) {
    Data::Mission::TilResource::CullingData culling_data;

    TilResource::Floor current_floor;
    TilResource::Tile  current_tile;
    Data::Mission::Til::Mesh::Polygon current_polygon;

    int_fast16_t highest_point = std::numeric_limits<int_fast16_t>::min();
    int_fast16_t lowest_point  = std::numeric_limits<int_fast16_t>::max();
    int_fast16_t height = 0;

    const TilResource::HeightmapPixel* pixels[4];

    for(int_fast16_t y = 0; y < TilResource::AMOUNT_OF_TILES; y++) {
        for(int_fast16_t x = 0; x < TilResource::AMOUNT_OF_TILES; x++) {
            current_floor = mesh_reference_grid[x][y];

            pixels[ FRONT_LEFT  ] = point_cloud_3_channel.getRef( y + 0, x + 0 );
            pixels[  BACK_LEFT  ] = point_cloud_3_channel.getRef( y + 1, x + 0 );
            pixels[  BACK_RIGHT ] = point_cloud_3_channel.getRef( y + 1, x + 1 );
            pixels[ FRONT_RIGHT ] = point_cloud_3_channel.getRef( y + 0, x + 1 );

            for(int_fast16_t f = 0; f < current_floor.tile_amount; f++) {
                current_tile = mesh_tiles.at(current_floor.tiles_start + f);

                current_polygon = TileSet::default_mesh[current_tile.mesh_type];

                for(int p = 0; p < 4; p++) {
                    const Mesh::Point &point = current_polygon.points[p];

                    if(point.heightmap_channel == Mesh::NO_ELEMENT)
                        p = 4; // Cancel the entire loop.
                    else {
                        height = 16 * pixels[point.facing_direction]->channel[point.heightmap_channel];

                        highest_point = std::max(height, highest_point);
                        lowest_point  = std::min(height,  lowest_point);
                    }
                }
            }
        }
    }

    auto midvalue = std::abs(highest_point - lowest_point) / 2;

    culling_data.trunk.height = std::abs(highest_point - lowest_point) / 2 + lowest_point;
    culling_data.trunk.radius = std::sqrt((8*512)*(8*512) + (8*512)*(8*512) + midvalue * midvalue);

    return culling_data;
}
