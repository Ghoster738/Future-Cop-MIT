#ifndef GRAPHICS_2D_TEXT_BUFFER_H
#define GRAPHICS_2D_TEXT_BUFFER_H

#include "../Data/Mission/FontResource.h"
#include "../Data/Accessor.h"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace Graphics {

class Environment;

class Text2DBuffer {
protected:
    Text2DBuffer();

    glm::vec2 start, end;
public:
    enum CenterMode {
        LEFT,
        MIDDLE,
        RIGHT
    };

    struct Font {
        uint32_t resource_id;
        float    scale;

        Font( uint32_t p_resource_id, float p_scale ) : resource_id( p_resource_id ), scale ( p_scale ) {}
        Font( uint32_t p_resource_id ) : Font( p_resource_id, 1 ) {}
        Font() : Font( 1 ) {}
        Font( const Font &font ) : Font( font.resource_id, font.scale ) {}
    };
    
    virtual ~Text2DBuffer();

    virtual std::vector<std::string> splitText( const Font &font, const std::string &unsplit_text, float line_length ) const = 0;

    virtual bool selectFont( Font &font, unsigned minium_height, unsigned maxiuim_height ) const = 0;
    virtual bool scaleFont( Font &font, unsigned height ) const = 0;
    virtual float getLineLength( const Font &font, const std::string &text ) const = 0;

    virtual int setFont( const Font &font ) = 0;
    virtual int setPosition( const glm::vec2 &position ) = 0;
    virtual int setColor( const glm::vec4 &color ) = 0;
    virtual int setCenterMode( enum CenterMode ) = 0;
    virtual int print( const std::string &text ) = 0;

    virtual void beginBox() = 0;
    virtual glm::vec2 getBoxStart() const = 0;
    virtual glm::vec2 getBoxEnd() const = 0;

    /**
     * Restart the whole font buffer clearing the data.
     * @return How many font buffers were NOT cleared successfully, or a negative number indicating an error.
     *   Zero means everything worked perfectly.
     */
    virtual int reset() = 0;
};

}

#endif // GRAPHICS_2D_TEXT_BUFFER_H
