#ifndef MISSION_RESOURCE_PTC_HEADER
#define MISSION_RESOURCE_PTC_HEADER

#include "Resource.h"
#include "TilResource.h"
#include "../../Utilities/ImageData.h"

namespace Data {

namespace Mission {

/**
 * This is the resource that basically stores the entire map.
 * It is effectively a pointer grid containing the pointers to the tiles.
 *
 */
class PTCResource : public Resource {
public:
	static const std::string FILE_EXTENSION;
	static const uint32_t IDENTIFIER_TAG;
private:
    Utilities::ImageData grid;
    Utilities::ImageData debug_map_display;

    std::vector<TilResource*> tile_array_r;
public:
    PTCResource();
    PTCResource( const PTCResource &obj );

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;
    /**
     * This method organizes the added tiles, and makes the debug_map_display.
     */
    bool makeTiles( const std::vector<TilResource*> &tile_array_r );

    /**
     * Get the til resource from a certain point.
     * @param x the x position in pointer/index space.
     * @param y the y position in pointer/index space.
     * @return A nullptr if the coordinates are out of bounds or if the tile is a nullptr to begin with; If these two conditions are not meet then it is a valid tile reference pointer.
     */
    TilResource* getTile( unsigned int x, unsigned int y );
    const TilResource* getTile( unsigned int x, unsigned int y ) const;

    /**
     * Get the number of tiles that are in this class.
     * @return A nullptr if the coordinates are out of bounds or if the tile is a nullptr to begin with; If these two conditions are not meet then it is a valid tile reference pointer.
     */
    unsigned int getAmountOfTiles() const { return tile_array_r.size(); }

    TilResource ** getTileReference() { return tile_array_r.data(); }

    /**
     * Get the width of the map.
     */
    unsigned int getWidth() const { return grid.getWidth(); }

    /**
     * Get the height of the map.
     */
    unsigned int getHeight() const { return grid.getHeight(); }

    /**
     * This is only the first step in loading this. It needs TilResource's in order for this to work properly.
     */
    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    /**
     * This exports all three heightmaps of all the tiles in one texture. It looks interesting in its own way.
     * @param file_path The path the file will be written.
     * @param agruments --dry for no writing operations.
     * @return zero if it is a dry run or the return results of the ImageData write operations.
     */
    virtual int write( const char *const file_path, const std::vector<std::string> & arguments ) const;

    static std::vector<PTCResource*> getVector( IFF &mission_file );
    static const std::vector<PTCResource*> getVector( const IFF &mission_file );
};

}

}

#endif // MISSION_RESOURCE_PTC_HEADER
