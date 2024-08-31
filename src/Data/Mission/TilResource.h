#ifndef MISSION_RESOURCE_TILE_HEADER
#define MISSION_RESOURCE_TILE_HEADER

#include "ModelResource.h"
#include "BMPResource.h"
#include "../../Utilities/GridBase2D.h"
#include "../../Utilities/Collision/Ray.h"
#include "../../Utilities/Collision/Triangle.h"
#include "../../Utilities/Random.h"

namespace Data {

namespace Mission {

/**
 * Til Resource Reader.
 *
 * This is the Til resource it holds the resource responsible for holding a chunk of a map.
 * In Future Cop, this resource has 16x16 tiles in it. Its size is 131072x131072.
 */
class TilResource : public ModelResource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

    struct HeightmapPixel {
        int8_t channel[3];
    };
    struct CullingChunk {
        int16_t radius; // The radius of the bounding sphere. Let w be the height and depth of chunk (w^2 + w^2 + h^2). Optain h by getting the distance between the highest of heights and the lowest of the height of the chunk).
        int16_t height; // This is the median between the heightest point and the lowest point of the polygons this chunk contains.
    };
    struct CullingData {
        CullingChunk trunk;
        CullingChunk branches[4];
        CullingChunk leaves[16];

        CullingData();
        CullingData(const CullingData& data);
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

        std::string getString() const;
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

        std::string getString() const;
    };
    struct DynamicMonoGraphics {
        uint16_t forth : 6;
        uint16_t third : 6;
        uint16_t second_lower : 4;
        
        DynamicMonoGraphics() {}
        DynamicMonoGraphics( const uint16_t bitfield ) {
            set( bitfield );
        }
        
        void set( const uint16_t bitfield ) {
            forth        = (bitfield >>  0) & ((1 << 6) - 1);
            third        = (bitfield >>  6) & ((1 << 6) - 1);
            second_lower = (bitfield >> 12) & ((1 << 4) - 1);
        }
    };
    struct DynamicColorGraphics {
        uint16_t third  : 8;
        uint16_t second : 8;
        
        DynamicColorGraphics() {}
        DynamicColorGraphics( const uint16_t bitfield ) {
            set( bitfield );
        }
        
        void set( const uint16_t bitfield ) {
            third  = (bitfield >>  0) & ((1 << 8) - 1);
            second = (bitfield >>  8) & ((1 << 8) - 1);
        }
    };
    struct TileGraphics {
        uint16_t shading : 8; // Lighting information, but they do change meaning depending type bitfield
        uint16_t texture_index : 3; // Holds the index of the texture the tile references.
        uint16_t animated : 1;
        uint16_t semi_transparent : 1;
        uint16_t rectangle : 1; // Indicates rectangle/square?
        uint16_t type : 2; // Tells how the tile will be drawn.
        
        TileGraphics() {}
        TileGraphics( const uint16_t bitfield ) {
            set( bitfield );
        }
        
        void set( const uint16_t bitfield ) {
            shading          = (bitfield >>  0) & ((1 << 8) - 1);
            texture_index    = (bitfield >>  8) & ((1 << 3) - 1);
            animated         = (bitfield >> 11) & 1;
            semi_transparent = (bitfield >> 12) & 1;
            rectangle        = (bitfield >> 13) & 1;
            type             = (bitfield >> 14) & ((1 << 2) - 1);
        }
        
        uint16_t get() const {
            return ((uint16_t)shading << 0) |
                ((uint16_t)texture_index << 8) |
                ((uint16_t)animated << 11) |
                ((uint16_t)semi_transparent << 12) |
                ((uint16_t)rectangle << 13) |
                ((uint16_t)type << 14);
        }

        std::string getString() const;
    };
    class InfoSLFX {
    public:
        union Data {
            struct Wave {
                // byte 1
                uint32_t speed:                  8; // This is the speed of the gradient.

                // byte 2
                uint32_t background_light_level: 8;

                // byte 3
                uint32_t gradient_width:         4; // This subtracts the width of the gradient. 0x0 smoothest. 0x2 perfect. 0x3 narrow. 0xF so narrow that no animation is seen.
                uint32_t gradient_light_level:   4; // The Light and Dark Levels of the gradient.
            } wave;
            struct Noise {
                // byte 1
                uint32_t reducer:    2; // The more tends to decrease the effect. Maybe a rightshift specifier.
                uint32_t unused_0:   4;
                uint32_t unknown_0:  1; // This does not seem to affect anything.
                uint32_t unused_1:   1;

                // byte 2
                uint32_t brightness: 8; // The more this value the brighter the animations become.

                // byte 3
                uint32_t unused_2:   8;
            } noise;
        } data;

        // byte 4
        uint32_t activate_diagonal: 4; // Looks like diagonal waves.
        uint32_t activate_noise:    1; // Looks like perlin noise. This bit overpowers activate_diagonal.
        uint32_t unknown_0:         2;
        uint32_t is_disabled:       1; // Enabling this would disable the animations.

    public:
        InfoSLFX() {}
        InfoSLFX( const uint32_t bitfield ) {
            set( bitfield );
        }

        std::string getString() const;
        uint32_t get() const;
        void     set( const uint32_t bitfield );
    };

    class AnimationSLFX {
    public:
        InfoSLFX info_slfx;

    private:
        Utilities::Random::Generator last, next;
        Utilities::Random random;
        float cycle;
        float speed;

    public:
        AnimationSLFX();
        AnimationSLFX( InfoSLFX info_slfx );

        InfoSLFX getInfo() const { return info_slfx; }

        void setInfo( InfoSLFX info_slfx );

        void advanceTime( float delta_seconds );

        Utilities::Image2D* getImage() const;
        void setImage( Utilities::Image2D &image ) const;
    };

    struct InfoSCTA {
        static constexpr float units_to_seconds = 1. / 300.;
        static constexpr float seconds_to_units = 300.;

        int_fast32_t  frame_count;
        uint_fast32_t duration_per_frame;
        uint_fast32_t animated_uv_offset;
        uint_fast32_t source_uv_offset;

        InfoSCTA() {}

        InfoSCTA( int_fast32_t p_frame_count, uint_fast32_t p_duration_per_frame, uint_fast32_t p_animated_uv_offset, uint_fast32_t p_source_uv_offset ) : frame_count( p_frame_count ), duration_per_frame( p_duration_per_frame ), animated_uv_offset( p_animated_uv_offset ), source_uv_offset( p_source_uv_offset ) {}

        std::string getString() const;

        int_fast32_t getFrameCount() const { return std::abs( frame_count ); }
        float getSecondsPerFrame() const { return duration_per_frame * units_to_seconds; }
        float getSecondsPerCycle() const { return getSecondsPerFrame() * getFrameCount(); }
        float getDurationToSeconds() const { return seconds_to_units / duration_per_frame; }

        bool isMemorySafe() const { return frame_count >= 0; }

        /**
         * This method sets the variables inside the struct to be memory safe.
         * @return false if an element is found to be unstable.
         */
        bool setMemorySafe( size_t source_size, size_t animated_size );
    };
    
    static const std::string TILE_TYPE_COMPONENT_NAME;
    static constexpr size_t AMOUNT_OF_TILES = 16;
    static constexpr float  SPAN_OF_TIL = AMOUNT_OF_TILES / 2;
    static constexpr float MAX_HEIGHT = 4.0f; // The highest value is actually MAX_HEIGHT - SAMPLE_HEIGHT due to the span of the pixels being [127, -128].
    static constexpr float MIN_HEIGHT = -MAX_HEIGHT;
    static constexpr float SAMPLE_HEIGHT = (2.0 * MAX_HEIGHT) / 256.0f; // 256 values is contained in a pixel.
private:
    Utilities::GridBase2D<HeightmapPixel> point_cloud_3_channel; // I liked the Point Cloud Name. These are 3 channel signed bytes.

    uint8_t polygon_action_types[4];

    CullingData culling_data;

    glm::i8vec2 uv_animation;

    uint16_t mesh_library_size;
    Floor mesh_reference_grid[ AMOUNT_OF_TILES ][ AMOUNT_OF_TILES ];

    std::vector<Tile> mesh_tiles; // These are descriptions of tiles that are used to make up the map format. The 32 bit numbers are packed with information

    std::vector<glm::u8vec2> texture_cords; // They contain the UV's for the tiles, they are often read as quads
    std::vector<Utilities::PixelFormatColor::GenericColor> colors;
    std::vector<uint16_t> tile_graphics_bitfield;

    std::vector<InfoSCTA> SCTA_info;
    std::vector<glm::u8vec2> scta_texture_cords;
    
    uint32_t slfx_bitfield;
    
    struct TextureInfo {
        std::string name;
    };
    TextureInfo texture_info[8]; // There can only be 2*2*2 or 8 texture resource IDs.
    
    std::vector<Utilities::Collision::Triangle> all_triangles; // This stores all the triangles in the Til Resource.
public:
    static constexpr size_t TEXTURE_INFO_AMOUNT = sizeof( texture_info ) / sizeof( texture_info[0] );
    
    TilResource();
    TilResource( const TilResource &obj );

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    Utilities::Image2D getImage() const;
    
    void makeEmpty();

    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    bool loadTextures( const std::vector<BMPResource*> &textures );

    virtual int write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;

    virtual Utilities::ModelBuilder * createModel() const { return createModel( false, true ); }
    virtual Utilities::ModelBuilder * createCulledModel() const { return createModel( true, true ); }

    virtual Utilities::ModelBuilder * createModel( bool is_culled, bool metadata, Utilities::Logger &logger = Utilities::logger ) const;
    
    Utilities::ModelBuilder * createPartial( unsigned int texture_index, bool is_culled, bool metadata, float x_offset = 0.0f, float z_offset = 0.0f, Utilities::Logger &logger = Utilities::logger ) const;
    
    void createPhysicsCell( unsigned int x, unsigned int z );
    
    float getRayCast3D( const Utilities::Collision::Ray &ray ) const;
    float getRayCast2D( float x, float y ) const;
    float getRayCastDownward( float x, float y, float from_highest_point ) const;

    const std::vector<Utilities::Collision::Triangle>& getAllTriangles() const;
    Utilities::Image2D getHeightMap( unsigned int rays_per_tile = 4 ) const;
    
    glm::i8vec2 getUVAnimation() const { return uv_animation; }
    const std::vector<InfoSCTA>& getInfoSCTA() const { return SCTA_info; }
    const std::vector<glm::u8vec2>& getSCTATextureCords() const { return scta_texture_cords; }

    const InfoSLFX getInfoSLFX() const { return InfoSLFX( slfx_bitfield ); }
};

}

}

#endif // MISSION_RESOURCE_TILE_HEADER
