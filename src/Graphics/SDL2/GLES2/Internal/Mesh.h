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
        GLsizei count;
        const Texture2D *texture_ref;
    };
protected:
    Program *programR;
    GLuint vertex_buffer_object;
    VertexAttributeArray vertex_array;

    std::vector<DrawCommand> draw_command;
    GLenum draw_command_array_mode; // This holds the settings for draw_command.

    unsigned int vertex_buffer_size;
    unsigned int morph_buffer_size;
    unsigned int morph_frame_amount;
    unsigned int frame_amount;

    void addCommand( GLint first, GLsizei count, const Texture2D *texture_ref );
public:
    Mesh( Program *program );
    virtual ~Mesh();

    void setup( Utilities::ModelBuilder &model, const std::vector<Texture2D*> textures );

    void setDrawCommandArrayMode( GLenum draw_command_array_mode ) { this->draw_command_array_mode = draw_command_array_mode; }

    void bindArray() const;

    void noPreBindDraw( GLuint active_switch_texture, GLuint texture_switch_uniform ) const;

    void draw( GLuint active_switch_texture, GLuint texture_switch_uniform ) const;

    Program *getProgram() const { return programR; }

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
};

}
}
}
}

#endif // GL_MESH_1295_INCLUDE
