#ifndef GL_MESH_1295_INCLUDE
#define GL_MESH_1295_INCLUDE

#include "Program.h"
#include "Texture2D.h"
#include "../../../../Utilities/ModelBuilder.h"
#include "VertexAttributeArray.h"
#include <vector>

namespace Graphics {
namespace SDL2 {
namespace GLES2 {
namespace Internal {

class Mesh {
public:
    struct DrawCommand {
        GLint first;
        GLsizei opeque_count;
        GLsizei count;
        const Texture2D *texture_r;
    };
protected:
    Program *program_r;
    GLuint vertex_buffer_object;
    VertexAttributeArray vertex_array;

    std::vector<DrawCommand> draw_command;
    GLenum draw_command_array_mode; // This holds the settings for draw_command.

    unsigned int vertex_buffer_size;
    unsigned int morph_buffer_size;
    unsigned int morph_frame_amount;
    unsigned int frame_amount;
    
    glm::vec3 culling_sphere_position;
    float culling_sphere_radius;

    void addCommand( GLint first, GLsizei opeque_count, GLsizei count, const Texture2D *texture_r );
public:
    Mesh( Program *program_r );
    virtual ~Mesh();

    void setup( Utilities::ModelBuilder &model, const std::map<uint32_t, Internal::Texture2D*>& textures );

    void setDrawCommandArrayMode( GLenum draw_command_array_mode ) { this->draw_command_array_mode = draw_command_array_mode; }

    void bindArray() const;

    void noPreBindDraw( GLuint active_switch_texture, GLuint texture_switch_uniform ) const;

    void draw( GLuint active_switch_texture, GLuint texture_switch_uniform ) const;

    Program *getProgram() const { return program_r; }

    /**
     * This gets where the morph frames of the mesh.
     */
    unsigned int getMorphFrameAmount() const { return morph_frame_amount; }

    /**
     * This gets where the frames of the mesh.
     */
    unsigned int getFrameAmount() const { return frame_amount; }

    void setFrameAmount( unsigned int new_frame_amount ) { frame_amount = new_frame_amount; }

    /**
     * This method gets the offset of the morph frame.
     * @param morph_frame_index This is the morph frame index to access.
     * @return offset of the morph buffer.
     */
    size_t getMorphOffset( unsigned int morph_frame_index ) const;
    
    /**
     * This method makes a bounding sphere for the entire mesh.
     * @param position This holds the sphere's center location.
     * @param radius This holds the rotation of the sphere.
     * @return true if a bounding sphere is generated.
     */
    bool getBoundingSphere( glm::vec3 &position, float &radius ) const;
};

}
}
}
}

#endif // GL_MESH_1295_INCLUDE
