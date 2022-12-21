#ifndef GL_PROGRAM_8742_INCLUDE
#define GL_PROGRAM_8742_INCLUDE

#include "Shader.h"
#include <string>
#include <vector>
#include <ostream>

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
    Shader *vertex_r;
    Shader *fragment_r;
    GLuint shader_program_id;

    /**
     * This is a helper method used to set the shader to a new shader.
     * @warning This does not deallocate the old shader in program memory.
     * @param new_shader_r This is the shader that will be the new shader.
     * @param old_shader_r This is the shader that will be replaced.
     * @return The newly attached shader. Be sure to make the pointer to the shader point to the new one.
     */
    Shader* setShader( Shader *new_shader_r, Shader *old_shader_r );

    /**
     * This is a helper method used to get the OpenGL shader id.
     * @param shader_r The shader to get the shader id from.
     * @return The shader id or else a 0 for a nullptr for a shader address.
     */
    GLuint getShaderID( Shader* shader_r ) const;
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
    Program( Shader *vertex_r, Shader *fragment_r );

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
    void setVertexShader( Shader *vertex_r );

    /**
     * Set the program to use this as a fragment shader or a pixel shader.
     * @warning Call this method after allocate() and before link() or else this class will not work properly.
     * @param fragment_r The fragment shader or pixel shader for this program to use.
     */
    void setFragmentShader( Shader *fragment_r );

    /**
     * This completes the process of allocating the program by settting it to link with all of the shaders.
     * @warning Call this method after allocate() and setShader() or else this program will not work properly.
     * @return If the shader is successfully linked then this would return true.
     */
    bool link();
    
    /**
     * This gets the any log that the program has.
     * @return The error log if there is any.
     */
    std::string getInfoLog() const;

    /**
     * Set the pipeline to use this as a shader.
     */
    void use();
    
    /**
     * This exports the attribute.
     * @name This is the name of the attribute.
     * @output This is the output of the method if it finds a non-existent output.
     * @return If the attribute exists return true.
     */
    bool isAttribute( const std::basic_string<GLchar> &name, std::ostream *output_r = nullptr ) const;

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
    Shader *getVertexShader() { return vertex_r; }

    /**
     * @return The pointer to the Shader object stored in the fragment shader.
     */
    Shader *getFragmentShader() { return fragment_r; }

    /**
     * @return The OpenGL id from this class. However, if this returns a zero then there is probably no program allocated.
     */
    GLuint getProgramID() const { return shader_program_id; }
};

}
}
}
}

#endif // GL_PROGRAM_8742_INCLUDE
