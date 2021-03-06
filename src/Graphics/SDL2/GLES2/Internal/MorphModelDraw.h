#ifndef GRAPHICS_GLES2_INTERNAL_MORPH_MODEL_DRAW_H
#define GRAPHICS_GLES2_INTERNAL_MORPH_MODEL_DRAW_H

#include "StaticModelDraw.h"
#include "VertexAttributeArray.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {
namespace Internal {

class MorphModelDraw : public StaticModelDraw {
public:
    static const GLchar* default_vertex_shader;
protected:
    VertexAttributeArray morph_attribute_array_last;
    VertexAttributeArray morph_attribute_array_next;

    // uniforms are used for morpth attributes.
    GLuint sample_next_uniform_id;
    GLuint sample_last_uniform_id;
public:
    MorphModelDraw();
    virtual ~MorphModelDraw();

    /**
     * This sets up and compiles this shader from memory.
     * This is to be used for internal shaders.
     * @param shader_source The memory pointer to the source code of the shader.
     */
    void setVertexShader( const GLchar *const shader_source = default_vertex_shader ) { StaticModelDraw::setVertexShader( shader_source ); }

    /**
     * This sets up and compiles this shader from memory.
     * This is to be used for internal shaders.
     * @param shader_source The memory pointer to the source code of the shader.
     */
    void setFragmentShader( const GLchar *const shader_source = default_fragment_shader ) { StaticModelDraw::setFragmentShader( shader_source ); }

    /**
     * Link every shader to the program.
     * @return false if one of the shaders are not loaded.
     */
    int compilieProgram();

    /**
     * This draws all of the models with the morph attribute.
     * @note Make sure setFragmentShader, loadFragmentShader, compilieProgram and setWorld in this order are called SUCCESSFULLY.
     * @param This is the camera data to be passed into world.
     */
    void draw( const Camera &camera );
};

}
}
}
}

#endif // GRAPHICS_GLES2_INTERNAL_MORPH_MODEL_DRAW_H
