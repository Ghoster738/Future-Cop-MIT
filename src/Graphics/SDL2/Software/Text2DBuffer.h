#ifndef GRAPHICS_SDL2_SOFTWARE_TEXT_2D_BUFFER_H
#define GRAPHICS_SDL2_SOFTWARE_TEXT_2D_BUFFER_H

#include "../../Text2DBuffer.h"

#include "Environment.h"

namespace Graphics::SDL2::Software {

class Text2DBuffer : public Graphics::Text2DBuffer {
public:
    Software::Environment *environment_r;

    CenterMode center_mode;
    Font current_font;
    glm::i32vec2 position;
    glm::u8vec4  color;

    Text2DBuffer( Software::Environment &environment );
    virtual ~Text2DBuffer();

    virtual std::vector<std::string> splitText( const Font &font, const std::string &unsplit_text, float line_length ) const;

    virtual bool selectFont( Font &font, unsigned minium_height, unsigned maxiuim_height ) const;
    virtual bool scaleFont( Font &font, unsigned height ) const;
    virtual float getLineLength( const Font &font, const std::string &text ) const;

    virtual int setFont( const Font &font );
    virtual int setPosition( const glm::vec2 &position );
    virtual int setColor( const glm::vec4 &color );
    virtual int setCenterMode( enum CenterMode );
    virtual int print( const std::string &text );

    virtual void beginBox();
    virtual glm::vec2 getBoxStart() const;
    virtual glm::vec2 getBoxEnd() const;

    virtual int reset();
};

}

#endif // GRAPHICS_SDL2_SOFTWARE_TEXT_2D_BUFFER_H
