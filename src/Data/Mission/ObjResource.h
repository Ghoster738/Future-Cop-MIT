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
    static const std::filesystem::path FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

    static const std::string METADATA_COMPONENT_NAME;

    static const float FIXED_POINT_UNIT;
    static const float FIXED_NORMAL_UNIT;
    static const float ANGLE_UNIT;

    enum VertexColorMode {
        BLACK      = 0,
        MONOCHROME = 1,
        FULL       = 2
    };
    enum VisabilityMode {
        OPAQUE   = 0,
        ADDITION = 1,
        MIX      = 2
    };
    enum PrimitiveType {
        STAR            = 0,
        TRIANGLE_QUAD_0 = 1,
        TRIANGLE_QUAD_1 = 2,
        TRIANGLE        = 3,
        QUAD            = 4,
        BILLBOARD       = 5,
        LINE            = 7
    };
    struct Material {
        uint8_t uses_texture:              1; // Does the face use a texture or not?
        uint8_t normal_shading:            1;
        uint8_t polygon_color_transparent: 1;
        uint8_t polygon_color_type:        2; // Please see enum VertexColorMode
        uint8_t visability:                2; // Please see enum VisabilityMode
        uint8_t is_reflective:             1;
        uint8_t is_color_fade:             1; // Only circles use this kind of value.

        Material() : uses_texture(0), normal_shading(0), polygon_color_transparent(0), polygon_color_type(0), visability(0), is_reflective(0), is_color_fade(0)
        {}

        Material(const Material& m) : uses_texture(m.uses_texture), normal_shading(m.normal_shading), polygon_color_transparent(m.polygon_color_transparent), polygon_color_type(m.polygon_color_type), visability(m.visability), is_reflective(m.is_reflective), is_color_fade(m.is_color_fade)
        {}
    };
    struct FaceOverrideType {
        static constexpr float UNITS_TO_SECONDS = 0.001652018;

        uint8_t number_of_frames;
        uint8_t zero_0;
        uint8_t one;
        uint8_t unknown_bitfield;
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

        uint16_t face_override_index;

        glm::u8vec4 getColor( Material material ) const;
    };
    struct VertexColorOverride {
        unsigned face_index;
        float speed_factor;
        glm::vec3 colors[2];
    };
    struct Point {
        glm::vec3 position;
        glm::vec3 normal;
        glm::u8vec4 weights;
        glm::u8vec4 joints;
        glm::u8vec2 coords;
        uint16_t    face_override_index;
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
        void generateNormals();
    };
    struct MorphTriangle {
        MorphPoint points[3];

        void generateNormals();
    };
    struct DecodedBone {
        glm::vec3 position;
        glm::quat rotation;

        glm::mat4 toMatrix() const;
        DecodedBone transform(const glm::mat4 &matrix) const;
    };
    struct Bone {
        Bone *parent_r;
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

        DecodedBone decode(int16_t *bone_animation_data_r, unsigned frame) const;

        /**
         * @return The number of attributes in the bone.
         */
        unsigned int getNumAttributes() const;

        std::string getString() const;
    };
    class VertexData {
    private:
        std::vector<uint32_t>     reference_ids;
        std::vector<glm::i16vec3> positions;
        std::vector<glm::i16vec3> normals;
        std::vector<uint16_t>     lengths;

        int32_t size_of_4DVL;
        int32_t size_of_4DNL;
        int32_t size_of_3DRL;

    public:
        enum Tag {
            C_4DVL = 0,
            C_4DNL = 1,
            C_3DRL = 2
        };

    public:
        VertexData();

        uint32_t get3DRFSize() const;
        void     set3DRFSize(uint32_t size);

        void     set3DRFItem(Tag tag, uint32_t index, uint32_t id);
        uint32_t get3DRFItem(Tag tag, uint32_t index) const;

        void set4DVLSize(int32_t size_of_4DVL);
        void set4DNLSize(int32_t size_of_4DNL);
        void set3DRLSize(int32_t size_of_3DRL);

        int32_t get4DVLSize() const;
        int32_t get4DNLSize() const;
        int32_t get3DRLSize() const;

        glm::i16vec3* get4DVLPointer(uint32_t id);
        const glm::i16vec3* const get4DVLPointer(uint32_t id) const;

        glm::i16vec3* get4DNLPointer(uint32_t id);
        const glm::i16vec3* const get4DNLPointer(uint32_t id) const;

        uint16_t* get3DRLPointer(uint32_t id);
        const uint16_t* const get3DRLPointer(uint32_t id) const;
    };
    struct Primitive {
        PrimitiveType type;

        Material visual;

        uint16_t  face_type_offset;
        FaceType *face_type_r;
        unsigned vertex_color_override_index;

        uint8_t v[4], n[4];

        uint32_t getBmpID() const;
        bool isWithinBounds( uint32_t vertex_limit, uint32_t normal_limit ) const;

        int setStar(const VertexData& vertex_data, std::vector<Triangle> &triangles, std::vector<MorphTriangle> &morph_triangles, const std::vector<Bone> &bones) const;
        int setTriangle(const VertexData& vertex_data, std::vector<Triangle> &triangles, std::vector<MorphTriangle> &morph_triangles, const std::vector<Bone> &bones) const;
        int setQuad(const VertexData& vertex_data, std::vector<Triangle> &triangles, std::vector<MorphTriangle> &morph_triangles, const std::vector<Bone> &bones) const;
        int setBillboard(const VertexData& vertex_data, std::vector<Triangle> &triangles, std::vector<MorphTriangle> &morph_triangles, const std::vector<Bone> &bones) const;
        int setLine(const VertexData& vertex_data, std::vector<Triangle> &triangles, std::vector<MorphTriangle> &morph_triangles, const std::vector<Bone> &bones) const;

        static size_t getTriangleAmount( PrimitiveType type );

        bool operator() ( const Primitive &l_operand, const Primitive &r_operand ) const;
    };
    struct BoundingBox3D {
        int16_t x;
        int16_t y;
        int16_t z;
        uint16_t length_x;
        uint16_t length_y;
        uint16_t length_z;
        uint16_t length_pyth_3; // (length_x^2 + length_y^2 + length_z^2) square rooted.
        uint16_t length_pyth_2; // (length_x^2 + length_z^2) square rooted.
    };
    struct TextureReference {
        uint32_t resource_id;
        std::filesystem::path path;
    };
    struct AllowedPrimitives {
        unsigned star:      1;
        unsigned triangle:  1;
        unsigned quad:      1;
        unsigned billboard: 1;
        unsigned line:      1;
    };
    struct FacerPolygon {
        struct Point {
            glm::vec3 position;
            glm::u8vec4 weights;
            glm::u8vec4 joints;
        };

        PrimitiveType type;
        VisabilityMode visability_mode;
        Point point; // TODO If line drawing routine is added then make this into an array of 2.
        float width; // TODO If line drawing routine is added then make this into an array of 2.
        glm::vec3 color;
        uint32_t time_index;
        union {
            struct {
                uint32_t vertex_count;
                glm::vec3 other_color;
            } star;
            struct {
                uint32_t bmp_id;
                glm::u8vec2 coords[4];
            } texture;
        } graphics;
    };
private:
    struct {
        unsigned has_skeleton:     1;
        unsigned always_on:        1; // This always seems to be on.
        unsigned semi_transparent: 1; // This bit makes environment map polygons semi transparent.
        unsigned environment_map:  1;
        unsigned animation:        1;
    } info;

    VertexData vertex_data;

    std::map<uint_fast16_t, FaceType> face_types;
    std::vector<FaceOverrideType>     face_type_overrides;
    std::vector<glm::u8vec2>          override_uvs;
    std::vector<VertexColorOverride>  face_color_overrides;

    unsigned position_indexes[4];
    unsigned num_vertex_position_channel;
    std::vector<glm::i16vec3> vertex_position_data; // Overides position_indexes when present.

    std::vector<Primitive> primitives;

    std::vector<Bone>         bones;
    unsigned int              max_bone_childern; // Holds the maxium childern amount.
    unsigned int              bone_frames;
    int16_t                  *bone_animation_data; // Where the animation data is stored.
    unsigned int              bone_animation_data_size;
    
    unsigned int bounding_box_per_frame;
    unsigned int bounding_box_frames;
    std::vector<BoundingBox3D> bounding_boxes;

    std::vector<TextureReference> texture_references;
    
    /**
     * This gets the bytes per frame rating for the specific opcode.
     * @param opcode The opcode value to get the bytes per frame from.
     * @note This method does not tell you if the opcode exists. There are opcodes with bytes per frame with a zero value.
     * @return A zero if either the opcode does not exist or the bytes per frame rating for the opcode.
     */
    static unsigned int getOpcodeBytesPerFrame( Bone::Opcode opcode );

public:
    ObjResource();
    virtual ~ObjResource();

    virtual std::filesystem::path getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    bool isPositionValid( unsigned index ) const;

    glm::vec3 getPosition( unsigned index, unsigned frame_index ) const;

    DecodedBone getBone( unsigned bone_index, unsigned frame_index ) const;

    const std::vector<FaceOverrideType>& getFaceOverrideTypes() const { return face_type_overrides; }
    const std::vector<glm::u8vec2>& getFaceOverrideData() const { return override_uvs; }
    const std::vector<VertexColorOverride>& getVertexColorOverrides() const { return face_color_overrides; }

    std::vector<FacerPolygon> generateFacingPolygons(unsigned &triangle_amount, unsigned &frame_stride) const;
    bool getBoundingSphereFacingPolygons(const std::vector<FacerPolygon> polygons, glm::vec3 &sphere_position, float &radius) const;

    bool loadTextures( const std::vector<BMPResource*> &textures );

    virtual int write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;

    virtual Utilities::ModelBuilder * createModel() const;
    
    Utilities::ModelBuilder * createMesh( bool exclude_metadata, bool force_normal, AllowedPrimitives allowed_primitives ) const;
    Utilities::ModelBuilder * createBoundingBoxes() const;
};

}

}

#endif // MISSION_RESOURCE_OBJECT_HEADER
