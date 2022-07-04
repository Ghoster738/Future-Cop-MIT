#ifndef GRAPHICS_2D_TEXT_BUFFER_H
#define GRAPHICS_2D_TEXT_BUFFER_H

#include "../Utilities/DataTypes.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace Graphics {

class Environment;

class Text2DBuffer {
protected:
    void *text_2d_buffer_internal_data_p;
public:
    Text2DBuffer( unsigned int buffer_size_per_font_KiB );
    virtual ~Text2DBuffer();

    /**
     * Load the font library from the environment.
     * @note Make sure that the Environment has successfully loaded the font.
     * @param environment This is the environment to get the font from.
     * @return 1 for success, or 0 for the font library not existing in the environment.
     */
    bool loadFontLibrary( Environment &environment );

    int setFont( unsigned index );
    int setPosition( const glm::vec2 &position );
    int setColor( const glm::vec4 &color );
    int print( const std::string &text );

    /**
     * Restart the whole font buffer clearing the data.
     * @return How many font buffers were NOT cleared successfully, or a negative number indicating an error.
     *   Zero means everything worked perfectly.
     */
    int reset();

    /**
     * This gets the Graphics API variables for use in the internal code for the Environment.
     */
    void* getInternalData() { return text_2d_buffer_internal_data_p; }
};

}

#endif // GRAPHICS_2D_TEXT_BUFFER_H
