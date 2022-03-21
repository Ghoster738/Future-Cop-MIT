#ifndef GL_SHADER_4853_INCLUDE
#define GL_SHADER_4853_INCLUDE

#include "GLES2.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {
namespace Internal {

/**
 * This is the handles the shaders for OpenGLES.
 * 
 * This is designed for streamlining the shaders.
 */
class Shader {
public:
    enum TYPE {
        EMPTY    = 0, // No shader is allocated in this class
        VERTEX   = GL_VERTEX_SHADER, // The vertex shader deals with the vertices
        FRAGMENT = GL_FRAGMENT_SHADER  // The fragment shader deals with the pixels
    };
private:
    TYPE shader_type;
    GLuint shader_id;
public:
    /**
     * This does not allocate anything in OpenGLES.
     * This simply sets this shader to an empty state.
     */
    Shader();

    /**
     * This sets up and compiles that shader from memory.
     * This is to be used for internal shaders.
     * @param type This holds the type of the Shader. If this has the value of EMPTY then this constructor sets this Shader to its EMPTY state.
     * @param shader_source The memory pointer to the source code of the shader.
     */
    Shader( TYPE type, const GLchar *const shader_source );

    /**
     * This calls deallocate() in which it sets this Shader to an Empty state, and calls OpenGLES to delete this very shader.
     */
    virtual ~Shader();

    /**
     * This sets up and compiles that shader from memory.
     * This is to be used for internal shaders.
     * @param type This holds the type of the Shader. If this has the value of EMPTY then this method does nothing.
     * @param shader_source The memory pointer to the source code of the shader.
     */
    void setShader( TYPE type, const GLchar *const shader_source );

    /**
     * This loads a shader from a text file, and compiles it.
     * @param type This holds the type of the Shader. If this has the value of EMPTY then this method does load the text file into memory, but it does nothing with it.
     * @param shader_source The memory pointer to the source code of the shader.
     */
    int loadShader( TYPE type, const char *const file_path );

    /**
     * Tell OpenGL to delete the shader in memory, and set this class to an empty state.
     * @warning After this command this class will lose access to the OpenGL shader in memory.
     */
    void deallocate();

    /**
     * This returns the OpenGL access for the shader, or zero in which it is most likely not allocated.
     * @return The shader reference number from this class.
     */
    GLuint getShader() const { return shader_id; }

    /**
     * @return The shader type of the shader.
     */
    TYPE getType() const { return shader_type; }
};

}
}
}
}

#endif // GL_SHADER_4853_INCLUDE
