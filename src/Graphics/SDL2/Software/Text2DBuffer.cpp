#include "Text2DBuffer.h"

#include "Environment.h"

Graphics::Text2DBuffer* Graphics::SDL2::Software::Environment::allocateText2DBuffer() {
    return new Graphics::SDL2::Software::Text2DBuffer(*this);
}

namespace Graphics::SDL2::Software {

Text2DBuffer::Text2DBuffer( Graphics::SDL2::Software::Environment &gl_environment ) :
    Graphics::Text2DBuffer() {}

Text2DBuffer::~Text2DBuffer() {}

std::vector<std::string> Text2DBuffer::splitText( const Font &font, const std::string &unsplit_text, float line_length ) const {
    return std::vector<std::string>();
}

bool Text2DBuffer::selectFont( Font &font, unsigned minium_height, unsigned maxiuim_height ) const { return false; }
bool Text2DBuffer::scaleFont( Font &font, unsigned height ) const { return false; }
float Text2DBuffer::getLineLength( const Font &font, const std::string &text ) const { return 0.0f; }

int Text2DBuffer::setFont( const Font &font ) { return -1; }
int Text2DBuffer::setPosition( const glm::vec2 &position ) { return -1; }
int Text2DBuffer::setColor( const glm::vec4 &color ) { return -1; }
int Text2DBuffer::setCenterMode( enum CenterMode ) { return -1; }
int Text2DBuffer::print( const std::string &text ) { return -1; }

void Text2DBuffer::beginBox() {}
glm::vec2 Text2DBuffer::getBoxStart() const { return glm::vec2(0, 0); }
glm::vec2 Text2DBuffer::getBoxEnd() const { return glm::vec2(0, 0); }

int Text2DBuffer::reset() { return -1; }

}
