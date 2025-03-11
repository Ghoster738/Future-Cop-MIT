#ifndef MISSION_RESOURCE_PTC_HEADER
#define MISSION_RESOURCE_PTC_HEADER

#include "Resource.h"
#include "TilResource.h"
#include "../../Utilities/Image2D.h"

namespace Data {

namespace Mission {

/**
 * This is the resource stores the layout of the map.
 * It is effectively a pointer grid containing the pointers to the tiles.
 *
 */
class PTCResource : public Resource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

private:
    Utilities::GridBase2D<uint32_t> grid;
    Utilities::Image2D *debug_map_display_p;
    
    // How many CTils from the map boarder should not be crossed.
    uint32_t border_range; // Thank you BahKooJ.

    std::map<uint32_t, TilResource*> ctil_id_r;

public:
    PTCResource();
    PTCResource( const PTCResource &obj );
    ~PTCResource();

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;
    /**
     * This method organizes the added tiles, and makes the debug\_map\_display.
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
    unsigned int getAmountOfTiles() const { return ctil_id_r.size(); }

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
     * @param iff_option --dry for no writing operations.
     * @return zero if it is a dry run or the return results of the Image2D write operations.
     */
    virtual int write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;
    
    int writeEntireMap( const std::filesystem::path& file_path, bool make_culled = false ) const;
    
    float getRayCast3D( const Utilities::Collision::Ray &ray ) const;
    float getRayCast2D( float x, float y ) const;
    float getRayCastDownward( float x, float y, float from_highest_point ) const;

    static PTCResource* getTest( uint32_t resource_id, Utilities::Buffer::Endian endianess = Utilities::Buffer::Endian::LITTLE, Utilities::Logger *logger_r = nullptr );
};

}

}

#endif // MISSION_RESOURCE_PTC_HEADER
