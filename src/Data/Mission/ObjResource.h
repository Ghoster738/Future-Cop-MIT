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

    static const std::string SPECULAR_COMPONENT_NAME;

    struct FaceType {
        uint8_t opcodes[4];

        uint32_t bmp_id; // This is the resource id of the BMPResource texture refernced.
        bool has_transparent_pixel_t0;
        bool has_transparent_pixel_t1;

        glm::u8vec2 coords[4];
    };
    struct FaceTriangle {
        bool is_other_side; // This indicates that the triangle is mearly the other side of the quad.
        bool is_reflective;
        uint16_t  face_type_offset;
        FaceType *face_type_r;
        uint16_t v0, v1, v2;
        uint16_t n0, n1, n2;

        bool isWithinBounds( uint32_t vertex_limit, uint32_t normal_limit ) const;

        bool getTransparency() const;

        bool operator() ( const FaceTriangle & l_operand, const FaceTriangle & r_operand ) const;
    };
    struct FaceQuad : public FaceTriangle {
        int16_t v3;
        int16_t n3;

        FaceTriangle firstTriangle() const;
        FaceTriangle secondTriangle() const;
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
    // This is all that a GLTF needs to know.
    struct TextureReference {
        uint32_t resource_id;
        std::string name;
    };
private:
    std::vector<glm::i16vec3> vertex_positions;
    std::vector<glm::i16vec3> vertex_normals;
    std::map<uint_fast16_t, FaceType>  face_types;
    std::vector<FaceTriangle> face_trinagles;
    std::vector<FaceQuad>     face_quads;
    std::vector<Bone>         bones;
    unsigned int              max_bone_childern; // Holds the maxium childern amount.
    unsigned int              bone_frames;
    int16_t                  *bone_animation_data; // Where the animation data is stored.
    unsigned int              bone_animation_data_size;

    std::vector<std::vector<glm::i16vec3>> vertex_anm_positions;
    std::vector<std::vector<glm::i16vec3>> vertex_anm_normals;
    
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
