#ifndef GRAPHICS_2D_TEXT_BUFFER_INTERNAL_H
#define GRAPHICS_2D_TEXT_BUFFER_INTERNAL_H

#include "Internal/GLES2.h"
#include "Internal/FontSystem.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

class Text2DBuffer : public Graphics::Text2DBuffer {
public:
    Internal::FontSystem *font_system_r;
    std::vector<Internal::FontSystem::Text2D*> text_data;
    Internal::FontSystem::Text2D *current_text_2D_r; // This merely references the text_data vector.
    unsigned int buffer_size_per_font_KiB; // This is the memory size of the pages.
    unsigned int text_2D_expand_factor; // The amount of characters the text 2D expands
    
    Text2DBuffer( Environment &env_r );
    virtual ~Text2DBuffer();
    
    /**
     * Load the font library from the environment.
     * @note Make sure that the Environment has successfully loaded the font.
     * @param environment This is the environment to get the font from.
     * @return 1 for success, or 0 for the font library not existing in the environment.
     */
    bool loadFontLibrary( Environment &environment );
    
    virtual int setFont( unsigned index );
    virtual int setPosition( const glm::vec2 &position );
    virtual int setColor( const glm::vec4 &color );
    virtual int print( const std::string &text );

    /**
     * Restart the whole font buffer clearing the data.
     * @return How many font buffers were NOT cleared successfully, or a negative number indicating an error.
     *   Zero means everything worked perfectly.
     */
    virtual int reset();
};

}
}
}
#endif // GRAPHICS_2D_TEXT_BUFFER_H
