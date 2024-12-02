#ifndef GL_SHADER_4853_INCLUDE
#define GL_SHADER_4853_INCLUDE

#include <vector>
#include <string>

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
    
    class Type {
    public:
        enum PRECISION {
            LOW,    // Int: (-2^8,  2^8)  Float Range: (-2,       2) Floating Point Precision: 2^( -8) Absolute
            MEDIUM, // Int: (-2^10, 2^10) Float Range: (-2^14, 2^14) Floating Point Precision: 2^(-10) Relative
            HIGH,   // Int: (-2^16, 2^16) Float Range: (-2^62, 2^62) Floating Point Precision: 2^(-16) Relative Might not be available on fragment shader.
            ALL
        };
    protected:
        PRECISION precision;
        std::basic_string<GLchar> variable; // This holds the name with type.
    public:
        Type( PRECISION precision, std::basic_string<GLchar> variable_entry );
        
        virtual std::basic_string<GLchar> getEntryES2() const = 0;
        virtual std::basic_string<GLchar> getEntry2() const = 0;
    };
    class Attribute : public Type {
    public:
        Attribute( PRECISION precision, std::basic_string<GLchar> variable_entry );
        
        virtual std::basic_string<GLchar> getEntryES2() const;
        virtual std::basic_string<GLchar> getEntry2() const;
    };
    class Varying : public Type {
    public:
        Varying( PRECISION precision, std::basic_string<GLchar> variable_entry );
        
        virtual std::basic_string<GLchar> getEntryES2() const;
        virtual std::basic_string<GLchar> getEntry2() const;
    };
private:
    TYPE shader_type;
    GLuint shader_id;
    
    std::basic_string<GLchar> generated_shader;
    const GLchar *address_generated_shader_r;
public:
    /**
     * This does not allocate anything in OpenGL ES.
     * This simply sets this shader to an empty state.
     */
    Shader();

    /**
     * This sets up and compiles that shader from memory.
     * This is to be used for internal shaders.
     * @param type This holds the type of the Shader. If this has the value of EMPTY then this constructor sets this Shader to its EMPTY state.
     * @param shader_source The memory pointer to the source code of the shader.
     * @param attributes These are the attributes.
     * @param varyings These are the varyings.
     */
    Shader( TYPE type, const std::basic_string<GLchar> &shader_source, std::vector<Attribute> attributes = {}, std::vector<Varying> varyings = {} );

    /**
     * This calls deallocate() in which it sets this Shader to an Empty state, and calls OpenGLES to delete this very shader.
     */
    virtual ~Shader();

    /**
     * This sets up and compiles that shader from memory.
     * This is to be used for internal shaders.
     * @param type This holds the type of the Shader. If this has the value of EMPTY then this method does nothing.
     * @param shader_source The memory pointer to the source code of the shader.
     * @param attributes These are the attributes.
     * @param varyings These are the varyings.
     */
    void setShader( TYPE type, const std::basic_string<GLchar> &shader_source, std::vector<Attribute> attributes = {}, std::vector<Varying> varyings = {} );

    /**
     * This loads a shader from a text file, and compiles it.
     * @param type This holds the type of the Shader. If this has the value of EMPTY then this method does load the text file into memory, but it does nothing with it.
     * @param file_path The path to the shader.
     * @param attributes These are the attributes.
     * @param varyings These are the varyings.
     */
    int loadShader( TYPE type, const char *const file_path, std::vector<Attribute> attributes = {}, std::vector<Varying> varyings = {} );

    /**
     * Tell OpenGL to delete the shader in memory, and set this class to an empty state.
     * @warning After this command this class will lose access to the OpenGL shader in memory.
     */
    void deallocate();
    
    /**
     * This gets the compliation errors from the shader.
     * @return The error log if there is any.
     */
    std::string getInfoLog() const;

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
