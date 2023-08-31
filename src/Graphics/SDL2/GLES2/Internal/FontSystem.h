#ifndef GRAPHICS_GLES2_INTERNAL_FONT_SYSTEM_H
#define GRAPHICS_GLES2_INTERNAL_FONT_SYSTEM_H

#include "../../../../Data/Mission/FontResource.h"
#include "../../../Camera.h"
#include "Program.h"
#include "VertexAttributeArray.h"
#include "Texture2D.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {
namespace Internal {

class FontSystem {
public:
    class Text2D;
    struct Font {
        uint32_t resource_id;
        const Data::Mission::FontResource *font_resource_r;
        Texture2D texture;
        glm::u32vec2 texture_scale;

        /**
         * @return A pointer to Text2D.
         */
        Text2D * allocateText2D();
    };
    class Text2D {
    protected:
        Font    *font_r; // This is the font that the text uses.
        uint8_t *buffer_p; // This holds the OpenGL text of the font.

        size_t amount_of_characters;
        size_t max_amount_of_characters;

        GLuint vertex_buffer_object;

        glm::vec2 pen_position;
        uint32_t  pen_color;
    public:
        Text2D( Font *fontR );
        virtual ~Text2D();

        /**
         * Get the font that is attached to this.
         * @return Always a valid pointer to font
         */
        Font* getFont() const { return font_r; }

        /**
         * This is the color of the text.
         * @note The pen starts out with white, you can change the color.
         * @param pen_color Indexes 0-3 are red, green, blue, alpha.
         */
        void setPenColor( const uint8_t *pen_color);

        /**
         * @note The pen's position starts at (0,0).
         * @param position the position of the pen.
         */
        void setPenPosition( const glm::vec2 &pen_position );

        /**
         * Steal the color and the position from this other pen to take its place.
         * @return true if pen_r is not a nullptr.
         */
        bool stealPen( const Text2D *const pen_r );

        /**
         * Set the text limit for the Text2D class.
         * @note This operation is actually expensive.
         * @return If everything was correct then it would return the
         *     max_text value.
         *       If the max_text is too small then it will return 0.
         *    If a buffer cannot allocate due to allocation issues it
         *     will return a -1.
         */
        int setTextMax( size_t max_text );

        /**
         * This appends the text to the Text2D.
         * @note setPenPosition and setPenColor will set the position of
         *    the written text.
         * @param text The text to be added.
         * @return the text amount that was added to the text, or a negative one if the max_text is reached, or negative two if max_text is zero.
         */
        int addText( const std::string &text, char centering = 'l' );

        /**
         * This clears all the text on the Text2D, and sets the font type of the font.
         * @note This is the only way that the font type can be switched.
         */
        bool clearText( Font *fontR = nullptr );

        /**
         * @return the amount of characters added to the string.
         */
        size_t getCharAmount() const { return amount_of_characters; }

        /**
         * This draws the font.
         */
        void draw( const VertexAttributeArray &vertex_array ) const;
    };
    static const GLchar* default_vertex_shader;
    static const GLchar* default_fragment_shader;
protected:
    Program program;
    std::vector<Shader::Attribute> attributes;
    std::vector<Shader::Varying>   varyings;
    Shader  vertex_shader;
    Shader  fragment_shader;
    VertexAttributeArray vertex_array;
    GLuint texture_uniform_id;
    GLuint matrix_uniform_id;

    uint32_t invalid_text_resource_id; // This is a font with a DEL key symbol.

    std::vector<Font> font_bank;
public:
    /**
     */
    FontSystem( const std::vector<Data::Mission::FontResource*> &font_resources );

    /**
     * Delete everything.
     */
    ~FontSystem();
    
    std::map<uint32_t, Text2D*> getText2D();

    const VertexAttributeArray *const getVertexAttributeArray() const { return &vertex_array; };
    
    /**
     * @return the number of fonts in the class.
     */
    int getNumFonts() const;

    /**
     * @return A valid resource id for the font with DEL key.
     */
    uint32_t getInvalidBackupFontID() const { return invalid_text_resource_id; }

    /**
     * This gets a font from the class.
     */
    Font* accessFont( unsigned int index );
    
    /**
     * @return vertex buffer size in bytes.
     */
    static size_t getVertexSize();

    /**
     * This method gets the default vertex shader depending on the GL version.
     * @warning make sure the correct context is binded, or else you would get an improper shader.
     * @return a pointer to a vertex shader.
     */
    static const GLchar* getDefaultVertexShader();

    /**
     * This method gets the default fragment shader depending on the GL version.
     * @warning make sure the correct context is binded, or else you would get an improper shader.
     * @return a pointer to a fragment shader.
     */
    static const GLchar* getDefaultFragmentShader();

    /**
     * This sets up and compiles this shader from memory.
     * This is to be used for internal shaders.
     * @param shader_source The memory pointer to the source code of the shader.
     */
    void setVertexShader( const GLchar *const shader_source = getDefaultVertexShader() );

    /**
     * This loads a shader from a text file, and compiles it.
     * @param shader_source The memory pointer to the source code of the shader.
     */
    int loadVertexShader( const char *const file_path );

    /**
     * This sets up and compiles this shader from memory.
     * This is to be used for internal shaders.
     * @param shader_source The memory pointer to the source code of the shader.
     */
    void setFragmentShader( const GLchar *const shader_source = getDefaultFragmentShader() );

    /**
     * This loads a shader from a text file, and compiles it.
     * @param shader_source The memory pointer to the source code of the shader.
     */
    int loadFragmentShader( const char *const file_path );

    /**
     * Link every shader to the program.
     * @return false if one of the shaders are not loaded.
     */
    int compileProgram();

    void draw( const glm::mat4 &projection, const std::map<uint32_t, Text2D*> &text_2d_array );
};

}
}
}
}

#endif // GRAPHICS_GLES2_INTERNAL_FONT_SYSTEM_H
