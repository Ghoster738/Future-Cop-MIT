#ifndef MISSION_RESOURCE_OBJECT_HEADER
#define MISSION_RESOURCE_OBJECT_HEADER

#include "ModelResource.h"
#include "BMPResource.h"

#include "../../Utilities/ImageData.h"
#include "../../Utilities/DataTypes.h"

namespace Data {

namespace Mission {

class ObjResource : public ModelResource {
public:
	static const std::string FILE_EXTENSION;
	static const uint32_t IDENTIFIER_TAG;

    struct TextureQuad {
        unsigned int index;

        Utilities::DataTypes::Vec2UByte coords[4];

        bool isWithinBounds( size_t texture_amount ) const;
    };
    struct FaceTriangle {
        bool is_other_side; // This indicates that the triangle is mearly the other side of the quad.
        bool is_reflective;
        TextureQuad *texture_quad_ref;
        unsigned int v0, v1, v2;
        unsigned int n0, n1, n2;

        bool isWithinBounds( size_t vertex_limit, size_t normal_limit, size_t texture_quad_limit, const TextureQuad *origin ) const;

        bool operator() ( const FaceTriangle & l_operand, const FaceTriangle & r_operand ) const;
    };
    struct FaceQuad : public FaceTriangle {
        unsigned int v3;
        unsigned int n3;

        FaceTriangle firstTriangle() const;
        FaceTriangle secondTriangle() const;
    };
    class Bone {
    public:
        unsigned int parent_amount; // Minus one is the parent amount.
        unsigned int normal_start, normal_stride;
        unsigned int vertex_start, vertex_stride;
        Utilities::DataTypes::Vec3Short position, rotation; // They are all of a 3D system.
        unsigned int opcode; // Any modification to opcode would directly affect the number of attributes being used.
        
        /**
         * @return The number of attributes in the bone.
         */
        unsigned int getNumAttributes() const;
    };
private:
    std::vector< Utilities::DataTypes::Vec3Short > vertex_positions;
    std::vector< Utilities::DataTypes::Vec3Short > vertex_normals;
    std::vector< TextureQuad >     texture_quads;
    std::vector< FaceTriangle >    face_trinagles;
    std::vector< FaceQuad >        face_quads;
    std::vector< Bone >            bones;
    unsigned int                   max_bone_childern; // Holds the maxium childern amount.
    unsigned int                   bone_frames;
    int16_t                       *bone_animation_data; // Where the animation data is stored.
    unsigned int                   bone_animation_data_size;

    std::vector< std::vector< Utilities::DataTypes::Vec3Short > > vertex_anm_positions;
    std::vector< std::vector< Utilities::DataTypes::Vec3Short > > vertex_anm_normals;

    static const unsigned int opcode_mask[0x100];
    
    /**
     * This tests the opcode to see if it is valid.
     * @param opcode The opcode value to test.
     * @return If the opcode happens to exist in this program in some way this would return true.
     */
    static bool isValidOpcode( unsigned int opcode );
    
    /**
     * This gets the bytes per frame rating for the specific opcode.
     * @param opecode The opcode value to get the bytes per frame from.
     * @note This method does not tell you if the opcode exists. There are opcodes with bytes per frame with a zero value.
     * @return A zero if either the opcode does not exist or the bytes per frame rating for the opcode.
     */
    static unsigned int getOpcodeBytesPerFrame( unsigned int opcode );
public:
    ObjResource();
    ObjResource( const ObjResource &obj );
    virtual ~ObjResource();

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    virtual bool parse( const Utilities::Buffer &header, const Utilities::Buffer &data, const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    virtual int write( const char *const file_path, const std::vector<std::string> & arguments ) const;

    virtual Utilities::ModelBuilder * createModel( const std::vector<std::string> * arguments ) const;

    static std::vector<ObjResource*> getVector( Data::Mission::IFF &mission_file );
    static const std::vector<ObjResource*> getVector( const Data::Mission::IFF &mission_file );
};

}

}

#endif // MISSION_RESOURCE_OBJECT_HEADER
