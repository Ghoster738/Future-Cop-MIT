#ifndef GRAPHICS_GLES2_INTERNAL_FONT_SYSTEM_H
#define GRAPHICS_GLES2_INTERNAL_FONT_SYSTEM_H

#include "../../../../Data/Mission/FontResource.h"
#include "../../../Camera.h"
#include "Program.h"
#include "VertexAttributeArray.h"
#include "Texture2D.h"

namespace Graphics::SDL2::GLES2::Internal {

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

        glm::vec2 pen_span_min, pen_span_max;
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
        void setPenColor( const uint8_t *pen_color_r);

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
        int addText( const std::string &text, float scale, char centering = 'l' );

        glm::vec2 addedTextStart() const { return pen_span_min; }

        glm::vec2 addedTextEnd() const { return pen_span_max; }

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

    std::vector<Font> font_bank;
public:
    /**
     */
    FontSystem( const std::vector<const Data::Mission::FontResource*> &font_resources );

    /**
     * Delete everything.
     */
    ~FontSystem();
    
    std::map<uint32_t, Text2D*> getText2D();
    
    /**
     * @return the number of fonts in the class.
     */
    int getNumFonts() const;

    /**
     * This gets a font from the class.
     */
    Font* accessFont( unsigned int index );

    void draw( const glm::mat4 &projection, GLuint texture_uniform_id, VertexAttributeArray &vertex_array, const std::map<uint32_t, Graphics::SDL2::GLES2::Internal::FontSystem::Text2D*> &text_2d_array );
};

}

#endif // GRAPHICS_GLES2_INTERNAL_FONT_SYSTEM_H
