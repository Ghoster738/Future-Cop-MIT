#ifndef GRAPHICS_2D_TEXT_BUFFER_INTERNAL_H
#define GRAPHICS_2D_TEXT_BUFFER_INTERNAL_H

#include "../../Text2DBuffer.h"

// Implementation includes.
#include "Internal/GLES2.h"
#include "Internal/FontSystem.h"

namespace Graphics {

namespace SDL2 {
namespace GLES2 {

class Environment;

class Text2DBuffer : public Graphics::Text2DBuffer {
private:
    Graphics::SDL2::GLES2::Environment *gl_environment_r;
public:
    std::map<uint32_t, Internal::FontSystem::Text2D*> text_data_p;
    Internal::FontSystem::Text2D *current_text_2D_r; // This merely references the text_data vector.
    unsigned int buffer_size_per_font_KiB; // This is the memory size of the pages.
    unsigned int text_2D_expand_factor; // The amount of characters the text 2D expands
    
    Text2DBuffer( Graphics::Environment &env_r );
    virtual ~Text2DBuffer();
    
    /**
     * This is used to setup the fonts. It does not account for the playstation layouts.
     * @param env_r The environment that stores the data types.
     * @param data These are the IFF files that hold resources.
     * @return It will return 1 for success or a negative number stating how many "fonts" failed to load.
     */
    static int loadFonts( Graphics::Environment &env_r, const std::vector<Data::Mission::IFF*> &data );
    
    void draw( const glm::mat4 &projection ) const;
    
    virtual int setFont( uint32_t resource_id );
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
