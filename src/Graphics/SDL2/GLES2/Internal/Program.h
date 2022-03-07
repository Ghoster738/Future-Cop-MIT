#ifndef GL_PROGRAM_8742_INCLUDE
#define GL_PROGRAM_8742_INCLUDE

#include "Shader.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {
namespace Internal {

/**
 * This is a class that handles the OpenGL program. This is to handle
 * the allocation and deallocation of OpenGL GLSL Objects that are
 * essential for OpenGLES 2.
 * 
 * If you want to know the terminology of what an OpenGL program is and
 * the fragment and vertex shaders are then here are some of the links.
 * https://www.khronos.org/opengl/wiki/GLSL_Object
 * https://www.khronos.org/opengl/wiki/Shader
 */
class Program {
protected:
    bool is_allocated; // This is used to store the allocation state of the program.
    Shader *vertex_ref;
    Shader *fragment_ref;
    GLuint shader_program;

    /**
     * This is a helper method used to set the shader to a new shader.
     * @warning This does not deallocate the old shader in program memory.
     * @param new_shader This is the shader that will be the new shader.
     * @param old_shader This is the shader that will be replaced.
     * @return The newly attached shader. Be sure to make the pointer to the shader point to the new one.
     */
    Shader* setShader( Shader *new_shader, Shader *old_shader );

    /**
     * This is a helper method used to get the OpenGL shader id.
     * @param shader_id The shader to get the shader id from.
     * @return The shader id or else a 0 for a nullptr for a shader address.
     */
    GLuint getShaderID( Shader* shader_id ) const;
public:
    /**
     * This sets up the program and sets it to an empty memory state.
     */
    Program();

    /**
     * This sets up the program and sets it these shaders. This program should be fully usable after this method.
     * @param vertex_reference The vertex shader for this program to use.
     * @param fragment_reference The fragment shader or pixel shader for this program to use.
     */
    Program( Shader *vertex_reference, Shader *fragment_reference );

    /**
     * This calls deallocate to handle the deletion of this program.
     */
    virtual ~Program();

    /**
     * Allocate this program in OpenGL memory.
     * @warning Call this method before set*Shader and link or else this class will not work properly.
     */
    void allocate();

    /**
     * Delete this program in memory.
     */
    void deallocate();

    /**
     * Set the program to use this as a vertex shader.
     * @warning Call this method after allocate() and before link() or else this class will not work properly.
     * @param vertex_reference The vertex shader for this program to use.
     */
    void setVertexShader( Shader *vertex_reference );

    /**
     * Set the program to use this as a fragment shader or a pixel shader.
     * @warning Call this method after allocate() and before link() or else this class will not work properly.
     * @param fragment_reference The fragment shader or pixel shader for this program to use.
     */
    void setFragmentShader( Shader *fragment_reference );

    /**
     * This completes the process of allocating the program by settting it to link with all of the shaders.
     * @warning Call this method after allocate() and set*Shader() or else this program will not work properly.
     */
    void link();

    /**
     * Set the pipeline to use this as a shader.
     */
    void use();

    /**
     * This is the method that gets the vertex shader id from this program.
     * @return The OpenGL vertex shader id from this program. It will return zero if this program does not have a vertex shader.
     */
    GLuint getVertexShaderID() const;

    /**
     * This is the method that gets the fragment shader id from this program.
     * @return The OpenGL fragment shader id from this program. It will return zero if this program does not have a fragment shader.
     */
    GLuint getFragmentShaderID() const;

    /**
     * @return The pointer to the Shader object stored in the vertex shader.
     */
    Shader *getVertexShader() { return vertex_ref; }

    /**
     * @return The pointer to the Shader object stored in the fragment shader.
     */
    Shader *getFragmentShader() { return fragment_ref; }

    /**
     * @return The OpenGL id from this class. However, if this returns a zero then there is probably no program allocated.
     */
    GLuint getProgramID() const { return shader_program; }
};

}
}
}
}

#endif // GL_PROGRAM_8742_INCLUDE
