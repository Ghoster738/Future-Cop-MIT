#ifndef GL_TEXTURE_3591_INCLUDE
#define GL_TEXTURE_3591_INCLUDE

#include "GLES2.h"

#include <string>

namespace Graphics {
namespace SDL2 {
namespace GLES2 {
namespace Internal {

/**
 * This is the texture handler for OpenGLES 2.0.
 */
class Texture2D {
private:
    GLuint texture_id;
    bool is_allocated;
public:
    Texture2D();
    virtual ~Texture2D();

    /**
     * This method generates the texture.
     */
    void generate();

    /**
     * This allocates the texture along as the texture is not allocated.
     * @param active_texture This is the active texture slot.
     * @param mag This is the mag filter for the texture.
     * @param min This is the min filter for the texture.
     * @param wrap_s This is the wrap method for the s coordinate.
     * @param wrap_t This is the wrap method for the t coordinate.
     * @return True if the filters was successfuly set.
     */
    void setFilters( GLuint active_texture, GLint mag, GLint min, GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT );

    /**
     * This sets the image format of the texture.
     * @note http://docs.gl/es2/glTexImage2D could explain what the parameters would do.
     * @param active_texture This is the active texture slot.
     * @param level The texture level of the image.
     * @param internalformat This is the internal format for the texture.
     * @param width This is the width of the texture. Values must be a power of 2 factor.
     * @param height This is the height of the texture. Values must be a power of 2 factor.
     * @param border This is the boarder of the image.
     * @param format This is the format of a texel.
     * @param type The data type of the texel
     * @param data_r The pointer to the raw pixels of the texture.
     */
    void setImage( GLuint active_texture, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void * data_r );

    /**
     * This updates the image.
     * @note http://docs.gl/es2/glTexSubImage2D could explain what the parameters would do.
     * @param active_texture This is the active texture slot.
     * @param level The texture level of the image.
     * @param width This is the width of the texture. Values must be a power of 2 factor.
     * @param height This is the height of the texture. Values must be a power of 2 factor.
     * @param format This is the format of a texel.
     * @param type The data type of the texel
     * @param data_r The pointer to the raw pixels of the texture.
     */
    void updateImage( GLuint active_texture, GLint level, GLsizei width, GLsizei height, GLenum format, GLenum type, const void * data_r );

    /**
     * This binds the texture when a uniform is used in a shader.
     * @param active_texture The texture slot to be used.
     * @param texture_uniform_id uniform location of the texture.
     */
    void bind( GLuint active_texture, GLint texture_uniform_id ) const;

    /**
     * This binds the texture without the uniform.
     * @param active_texture The texture slot to be used.
     */
    void bind( GLuint active_texture ) const;

    /**
     * This generates a checker board pattern for a texture. This is
     * useful for testing proposes.
     * @param width of the texture.
     * @param height of the texture.
     * @param pixel1 A 24 bit red green and blue pixel value for one of the colors.
     * @param pixel2 A 24 bit red green and blue pixel value for one of the colors.
     */
    void genChecker( GLuint active_texture, GLsizei width, GLsizei height, GLubyte pixel1[3], GLubyte pixel2[3] );
};

}
}
}
}

#endif // GL_TEXTURE_3591_INCLUDE
