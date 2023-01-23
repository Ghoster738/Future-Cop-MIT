#ifndef MISSION_RESOURCE_TILE_HEADER
#define MISSION_RESOURCE_TILE_HEADER

#include "ModelResource.h"
#include "BMPResource.h"
#include "../../Utilities/GridBase2D.h"
#include "../../Utilities/Collision/Ray.h"
#include "../../Utilities/Collision/Triangle.h"

namespace Data {

namespace Mission {

class TilResource : public ModelResource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

    struct HeightmapPixel {
        int8_t channel[3];
    };
    struct CullingTile {
        uint16_t primary;

        uint16_t top_left;
        uint16_t top_right;
        uint16_t bottom_left;
        uint16_t bottom_right;
    };
    struct Floor {
        uint16_t tile_amount : 6;
        uint16_t tiles_start : 10;
        
        Floor() {}
        Floor( const uint16_t bitfield ) {
            set( bitfield );
        }
        
        void set( const uint16_t bitfield ) {
            tile_amount = (bitfield >> 0) & ((1 <<  6) - 1);
            tiles_start = (bitfield >> 6) & ((1 << 10) - 1);
        }
    };
    struct Tile {
        uint32_t end_column: 1;
        uint32_t texture_cord_index : 10;
        uint32_t front : 1;
        uint32_t back  : 1;
        uint32_t unknown_1 : 2; // Apperently this holds what this tile would do to the playable character. However, it appears that the action this tile would do to the player is stored elsewhere.
        uint32_t mesh_type : 7;
        uint32_t graphics_type_index : 10;
        
        Tile() {}
        Tile( const uint32_t bitfield ) {
            set( bitfield );
        }
        
        void set( const uint32_t bitfield ) {
            end_column          = (bitfield >>  0) & 1;
            texture_cord_index  = (bitfield >>  1) & ((1 << 10) - 1);
            front               = (bitfield >> 11) & 1;
            back                = (bitfield >> 12) & 1;
            unknown_1           = (bitfield >> 13) & ((1 <<  2) - 1);
            mesh_type           = (bitfield >> 15) & ((1 <<  7) - 1);
            graphics_type_index = (bitfield >> 22) & ((1 << 10) - 1);
        }
    };
    struct TileGraphics {
        uint16_t shading : 8; // Lighting information, but they do change meaning depending type bitfield
        uint16_t texture_index : 3; // Holds the index of the texture the tile references.
        uint16_t unknown_0 : 2;
        uint16_t rectangle : 1; // Indicates rectangle/square?
        uint16_t type : 2; // Tells how the tile will be drawn.
        
        TileGraphics() {}
        TileGraphics( const uint16_t bitfield ) {
            set( bitfield );
        }
        
        void set( const uint16_t bitfield ) {
            shading        = (bitfield >>  0) & ((1 << 8) - 1);
            texture_index  = (bitfield >>  8) & ((1 << 3) - 1);
            unknown_0      = (bitfield >> 11) & ((1 << 2) - 1);
            rectangle      = (bitfield >> 13) & ((1 << 1) - 1);
            type           = (bitfield >> 14) & ((1 << 2) - 1);
        }
    };
    class ColorMap {
    private:
        Utilities::GridBase2D<TileGraphics> map;
    public:
        ColorMap();
        
        Utilities::PixelFormatColor::GenericColor getColor( glm::u8vec3 position, const std::vector<Utilities::PixelFormatColor::GenericColor>& colors ) const;
        void gatherColors(
            const std::vector<TileGraphics>& tile_graphics,
            const Tile *const tiles_r, unsigned number,
            glm::u8vec2 position );
    };
    
    static constexpr size_t AMOUNT_OF_TILES = 16;
    static constexpr float  SPAN_OF_TIL = AMOUNT_OF_TILES / 2;
    static constexpr float MAX_HEIGHT = 6.0f; // The highest value is actually MAX_HEIGHT - SAMPLE_HEIGHT due to the span of the pixels being [127, -128].
    static constexpr float MIN_HEIGHT = -MAX_HEIGHT;
    static constexpr float SAMPLE_HEIGHT = (2.0 * MAX_HEIGHT) / 256.0f; // 256 values is contained in a pixel.
private:
    Utilities::GridBase2D<HeightmapPixel> point_cloud_3_channel; // I liked the Point Cloud Name. These are 3 channel signed bytes.

    uint16_t culling_distance; // This affects the radius of the circle where the culling happens
    CullingTile culling_top_left;
    CullingTile culling_top_right;
    CullingTile culling_bottom_left;
    CullingTile culling_bottom_right;

    uint16_t texture_reference; // This is an unknown number, but it affects all the textures in the file. One change might mess up the tiles.
    Floor mesh_reference_grid[ AMOUNT_OF_TILES ][ AMOUNT_OF_TILES ];

    uint16_t mesh_library_size; // This is the number of unknown numbers but 4 times bigger for some reason.
    std::vector<Tile> mesh_tiles; // These are descriptions of tiles that are used to make up the map format. The 32 bit numbers are packed with information

    std::vector<glm::u8vec2> texture_cords; // They contain the UV's for the tiles, they are often read as quads
    std::vector<Utilities::PixelFormatColor::GenericColor> colors;
    std::vector<TileGraphics> tile_texture_type;
    ColorMap color_map;
    
    std::string texture_names[8]; // There can only be 2*2*2 or 8 texture names;
    
    std::vector<Utilities::Collision::Triangle> all_triangles; // This stores all the triangles in the Til Resource.
public:
    static constexpr size_t TEXTURE_NAMES_AMOUNT = sizeof( texture_names ) / sizeof( texture_names[0] );
    
    TilResource();
    TilResource( const TilResource &obj );

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    Utilities::Image2D getImage() const;
    
    void makeEmpty();

    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    bool loadTextures( const std::vector<BMPResource*> &textures );

    virtual int write( const char *const file_path, const std::vector<std::string> & arguments ) const;

    virtual Utilities::ModelBuilder * createModel( const std::vector<std::string> * arguments ) const;
    
    Utilities::ModelBuilder * createPartial( unsigned int texture_index, float x_offset = 0.0f, float z_offset = 0.0f ) const;
    
    void createPhysicsCell( unsigned int x, unsigned int z );
    
    float getRayCast3D( const Utilities::Collision::Ray &ray ) const;
    float getRayCast2D( float x, float z ) const;
    float getRayCastDownward( float x, float z, float from_highest_point ) const;

    const std::vector<Utilities::Collision::Triangle>& getAllTriangles() const;
    Utilities::Image2D getHeightMap( unsigned int rays_per_tile = 4 ) const;
    
    static std::vector<TilResource*> getVector( IFF &mission_file );
    static const std::vector<TilResource*> getVector( const IFF &mission_file );
};

}

}

#endif // MISSION_RESOURCE_TILE_HEADER
