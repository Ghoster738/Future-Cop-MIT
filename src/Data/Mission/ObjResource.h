#ifndef MISSION_RESOURCE_OBJECT_HEADER
#define MISSION_RESOURCE_OBJECT_HEADER

#include "ModelResource.h"
#include "BMPResource.h"

#include "../../Utilities/Image2D.h"
#include "../../Utilities/DataTypes.h"

namespace Data {

namespace Mission {

class ObjResource : public ModelResource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

    static const std::string METADATA_COMPONENT_NAME;

    static const float FIXED_POINT_UNIT;
    static const float ANGLE_UNIT;

    enum VertexColorMode {
        NON = 0,
        MONOCHROME = 1,
        FULL = 2
    };
    enum VisabilityMode {
        OPAQUE   = 0,
        ADDITION = 1,
        MIX      = 2
    };
    enum PrimitiveType {
        UNKNOWN_0      = 0,
        TRIANGLE_OTHER = 2,
        TRIANGLE       = 3,
        QUAD           = 4,
        BILLBOARD      = 5,
        LINE           = 7
    };
    struct Material {
        uint8_t uses_texture:       1; // Does the face use a texture or not?
        uint8_t normal_shading:     1;
        uint8_t polygon_color_type: 2; // Please see enum VertexColorMode
        uint8_t visability:         2; // Please see enum VisabilityMode
        uint8_t is_reflective:      1;
    };
    struct FaceOverrideType {
        static constexpr float UNITS_TO_SECONDS = 0.001652018;

        uint8_t number_of_frames;
        uint8_t zero_0; // Guess
        uint8_t one; // Guess
        uint8_t unknown;
        uint16_t frame_duration;
        uint16_t zero_1;
        uint32_t uv_data_offset;
        uint32_t offset_to_3DTL_uv;

        float getFrameDuration() const { return static_cast<float>(frame_duration) * UNITS_TO_SECONDS; }
        float getEntireDuration() const { return getFrameDuration() * number_of_frames; }

        uint_fast32_t getEntireDurationUnits() const { return static_cast<uint_fast32_t>(frame_duration) * static_cast<uint_fast32_t>(number_of_frames); }
    };
    struct FaceType {
        uint8_t opcodes[4];

        uint32_t bmp_id; // This is the resource id of the BMPResource texture refernced.

        // TODO Find a use for these.
        bool has_transparent_pixel_t0;
        bool has_transparent_pixel_t1;

        glm::u8vec2 coords[4];

        uint32_t face_override_index;

        glm::u8vec4 getColor( Material material ) const;
    };
    struct Point {
        glm::vec3 position;
        glm::vec3 normal;
        glm::u8vec4 weights;
        glm::u8vec4 joints;
        glm::u8vec2 coords;
        int16_t     face_override_index;
    };
    struct MorphPoint {
        glm::vec3 position;
        glm::vec3 normal;
    };
    struct Triangle {
        uint32_t bmp_id;
        glm::u8vec4 color;
        Material visual;
        Point points[3];

        void switchPoints();
    };
    struct MorphTriangle {
        MorphPoint points[3];
    };
    class Bone {
    public:
        unsigned int parent_amount; // Minus one is the parent amount.
        unsigned int normal_start, normal_stride;
        unsigned int vertex_start, vertex_stride;
        glm::i16vec3 position, rotation; // They are all of a 3D system.
        struct Opcode {
            struct Axis {
                unsigned int x_const : 1;
                unsigned int y_const : 1;
                unsigned int z_const : 1;
            } position, rotation;
            unsigned int unknown: 2; // bone_index?
        } opcode;

        /**
         * @return The number of attributes in the bone.
         */
        unsigned int getNumAttributes() const;
    };
    struct Primitive {
        PrimitiveType type;

        Material visual;

        uint16_t  face_type_offset;
        FaceType *face_type_r;

        uint8_t v[4], n[4];

        uint32_t getBmpID() const;
        bool isWithinBounds( uint32_t vertex_limit, uint32_t normal_limit ) const;

        int setTriangle( std::vector<Triangle> &triangles, const std::vector<glm::i16vec3> &positions, const std::vector<glm::i16vec3> &normals, const std::vector<uint16_t> &lengths, std::vector<MorphTriangle> &morph_triangles, const std::vector<std::vector<glm::i16vec3>> &vertex_anm_positions, const std::vector<std::vector<glm::i16vec3>> &vertex_anm_normals, const std::vector<std::vector<uint16_t>> &anm_lengths, const std::vector<Bone> &bones ) const;
        int setQuad( std::vector<Triangle> &triangles, const std::vector<glm::i16vec3> &positions, const std::vector<glm::i16vec3> &normals, const std::vector<uint16_t> &lengths, std::vector<MorphTriangle> &morph_triangles, const std::vector<std::vector<glm::i16vec3>> &vertex_anm_positions, const std::vector<std::vector<glm::i16vec3>> &vertex_anm_normals, const std::vector<std::vector<uint16_t>> &anm_lengths, const std::vector<Bone> &bones ) const;
        int setBillboard( std::vector<Triangle> &triangles, const std::vector<glm::i16vec3> &positions, const std::vector<glm::i16vec3> &normals, const std::vector<uint16_t> &lengths, std::vector<MorphTriangle> &morph_triangles, const std::vector<std::vector<glm::i16vec3>> &vertex_anm_positions, const std::vector<std::vector<glm::i16vec3>> &vertex_anm_normals, const std::vector<std::vector<uint16_t>> &anm_lengths, const std::vector<Bone> &bones ) const;
        int setLine( std::vector<Triangle> &triangles, const std::vector<glm::i16vec3> &positions, const std::vector<glm::i16vec3> &normals, const std::vector<uint16_t> &lengths, std::vector<MorphTriangle> &morph_triangles, const std::vector<std::vector<glm::i16vec3>> &vertex_anm_positions, const std::vector<std::vector<glm::i16vec3>> &vertex_anm_normals, const std::vector<std::vector<uint16_t>> &anm_lengths, const std::vector<Bone> &bones ) const;

        static size_t getTriangleAmount( PrimitiveType type );

        bool operator() ( const Primitive &l_operand, const Primitive &r_operand ) const;
    };
    // Warning: I do not know if this is actually the bounding box's data structure.
    struct BoundingBox3D {
        int16_t x;
        int16_t y;
        int16_t z;
        uint16_t length_x;
        uint16_t length_y;
        uint16_t length_z;
        uint16_t rotation_x;
        uint16_t rotation_y;
    };
    struct TextureReference {
        uint32_t resource_id;
        std::string name;
    };
private:
    struct {
        unsigned has_skeleton:     1;
        unsigned always_on:        1; // This always seems to be on.
        unsigned semi_transparent: 1; // This bit makes environment map polygons semi transparent.
        unsigned environment_map:  1;
        unsigned animation:        1;
    } info;
    unsigned position_indexes[4];

    std::vector<glm::i16vec3> vertex_positions;
    std::vector<glm::i16vec3> vertex_normals;
    std::vector<uint16_t>     lengths;

    std::map<uint_fast16_t, FaceType> face_types;
    std::vector<FaceOverrideType>     face_type_overrides;
    std::vector<glm::u8vec2>          override_uvs;

    std::vector<Primitive>    face_triangles;
    std::vector<Primitive>    face_quads;
    std::vector<Primitive>    face_billboards;
    std::vector<Primitive>    face_lines;

    std::vector<Bone>         bones;
    unsigned int              max_bone_childern; // Holds the maxium childern amount.
    unsigned int              bone_frames;
    int16_t                  *bone_animation_data; // Where the animation data is stored.
    unsigned int              bone_animation_data_size;

    std::vector<std::vector<glm::i16vec3>> vertex_anm_positions;
    std::vector<std::vector<glm::i16vec3>> vertex_anm_normals;
    std::vector<std::vector<uint16_t>>     anm_lengths;
    
    unsigned int bounding_box_per_frame;
    unsigned int bounding_box_frames;
    std::vector<BoundingBox3D> bounding_boxes;

    std::vector<TextureReference> texture_references;
    
    /**
     * This gets the bytes per frame rating for the specific opcode.
     * @param opecode The opcode value to get the bytes per frame from.
     * @note This method does not tell you if the opcode exists. There are opcodes with bytes per frame with a zero value.
     * @return A zero if either the opcode does not exist or the bytes per frame rating for the opcode.
     */
    static unsigned int getOpcodeBytesPerFrame( Bone::Opcode opcode );
public:
    ObjResource();
    ObjResource( const ObjResource &obj );
    virtual ~ObjResource();

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    bool isPositionValid( unsigned index ) const;

    glm::vec3 getPosition( unsigned index ) const;

    const std::vector<FaceOverrideType>& getFaceOverrideTypes() const { return face_type_overrides; }
    const std::vector<glm::u8vec2>& getFaceOverrideData() const { return override_uvs; }

    bool loadTextures( const std::vector<BMPResource*> &textures );

    virtual int write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;

    virtual Utilities::ModelBuilder * createModel() const;
    
    Utilities::ModelBuilder * createBoundingBoxes() const;

    static std::vector<ObjResource*> getVector( Data::Mission::IFF &mission_file );
    static const std::vector<ObjResource*> getVector( const Data::Mission::IFF &mission_file );
};

}

}

#endif // MISSION_RESOURCE_OBJECT_HEADER
