#ifndef MISSION_RESOURCE_TILE_HEADER
#define MISSION_RESOURCE_TILE_HEADER

#include "ModelResource.h"
#include "BMPResource.h"
#include "../../Utilities/ImageData.h"
#include "../../Utilities/Collision/Ray.h"
#include "../../Utilities/Collision/Triangle.h"

namespace Data {

namespace Mission {

class TilResource : public ModelResource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

    struct CullingTile {
        uint16_t primary;

        uint16_t top_left;
        uint16_t top_right;
        uint16_t bottom_left;
        uint16_t bottom_right;
    };
    union Floor {
        uint16_t floor;
        struct {
            uint16_t tile_amount : 6;
            uint16_t tiles_start : 10;
        };
    };
    union Tile {
        uint32_t tile;
        struct {
            uint32_t unknown_0 : 1;
            uint32_t texture_cord_index : 10;
            uint32_t collision_type : 2; // 0b00 for floor; 0b01 for wall facing +x and -y; 0b10 for wall facing -x and +y; 0b11 for slopes
            uint32_t unknown_1 : 2; // Apperently this holds what this tile would do to the playable character. However, it appears that the action this tile would do to the player is stored elsewhere.
            uint32_t mesh_type : 7;
            uint32_t graphics_type_index : 10;
        };
    };
    union TileGraphics {
        uint16_t tile_graphics;
        struct {
            uint16_t shading : 8; // Lighting information, but they do change meaning depending type bitfield
            uint16_t texture_index : 3; // Holds the index of the texture the tile references.
            uint16_t unknown_0 : 2;
            uint16_t rectangle : 1; // Indicates rectangle/square?
            uint16_t type : 2; // Tells how the tile will be drawn.
        };
    };
    
    static constexpr size_t AMOUNT_OF_TILES = 16;
    static constexpr float  SPAN_OF_TIL = AMOUNT_OF_TILES / 2;
    static constexpr float MAX_HEIGHT = 6.0f; // The highest value is actually MAX_HEIGHT - SAMPLE_HEIGHT due to the span of the pixels being [127, -128].
    static constexpr float MIN_HEIGHT = -MAX_HEIGHT;
    static constexpr float SAMPLE_HEIGHT = (2.0 * MAX_HEIGHT) / 256.0f; // 256 values is contained in a pixel.
private:
    Utilities::ImageData point_cloud_3_channel; // I liked the Point Cloud Name. These are 3 channel signed bytes.

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
    std::vector<uint16_t> colors;
    std::vector<TileGraphics> tile_texture_type;
    
    std::string texture_names[8]; // There can only be 2*2*2 or 8 texture names;
    
    std::vector<Utilities::Collision::Triangle> all_triangles; // This stores all the triangles in the Til Resource.
    // std::pair<size_t,size_t>[16][16] triangle_map;
public:
    static constexpr size_t TEXTURE_NAMES_AMOUNT = sizeof( texture_names ) / sizeof( texture_names[0] );
    
    TilResource();
    TilResource( const TilResource &obj );

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    Utilities::ImageData *const getImage() const;
    
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
    Utilities::ImageData* getHeightMap( unsigned int rays_per_tile = 4 ) const;
    
    static std::vector<TilResource*> getVector( IFF &mission_file );
    static const std::vector<TilResource*> getVector( const IFF &mission_file );
};

}

}

#endif // MISSION_RESOURCE_TILE_HEADER
