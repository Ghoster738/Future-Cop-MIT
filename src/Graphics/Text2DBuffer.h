#ifndef GRAPHICS_2D_TEXT_BUFFER_H
#define GRAPHICS_2D_TEXT_BUFFER_H

#include "../Utilities/DataTypes.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include "../Data/Mission/FontResource.h"

namespace Graphics {

class Environment;

class Text2DBuffer {
protected:
    Text2DBuffer();
public:
    static Graphics::Text2DBuffer* alloc( Environment &env_r );
    
    virtual ~Text2DBuffer();
    
    static int loadFonts( Environment &env_r, const std::vector<Data::Mission::IFF*> &data );

    virtual int setFont( uint32_t resource_id ) = 0;
    virtual int setPosition( const glm::vec2 &position ) = 0;
    virtual int setColor( const glm::vec4 &color ) = 0;
    virtual int print( const std::string &text ) = 0;

    /**
     * Restart the whole font buffer clearing the data.
     * @return How many font buffers were NOT cleared successfully, or a negative number indicating an error.
     *   Zero means everything worked perfectly.
     */
    virtual int reset() = 0;
};

}

#endif // GRAPHICS_2D_TEXT_BUFFER_H
