#include "Text2DBuffer.h"

#include "Environment.h"

Graphics::Text2DBuffer* Graphics::SDL2::Software::Environment::allocateText2DBuffer() {
    return new Graphics::SDL2::Software::Text2DBuffer(*this);
}

namespace Graphics::SDL2::Software {

Text2DBuffer::Text2DBuffer( Software::Environment &environment ) :
    Graphics::Text2DBuffer(), environment_r(&environment), center_mode(CenterMode::LEFT) {}

Text2DBuffer::~Text2DBuffer() {}

std::vector<std::string> Text2DBuffer::splitText( const Font &font, const std::string &unsplit_text, float line_length ) const {
    return std::vector<std::string>( {unsplit_text} );
}

bool Text2DBuffer::selectFont( Font &font, unsigned minium_height, unsigned maxiuim_height ) const {
    if(this->environment_r == nullptr)
        return false;

    if(this->environment_r->font_draw_2d.resource_id_to_font.size() == 0)
        return false;

    auto first_thing = this->environment_r->font_draw_2d.resource_id_to_font.begin();

    if(first_thing == this->environment_r->font_draw_2d.resource_id_to_font.end())
        return false;

    font.resource_id = first_thing->second->font_r->getResourceID();
    font.scale       = 1.f;
    return true;

}
bool Text2DBuffer::scaleFont( Font &font, unsigned height ) const { return false; }

float Text2DBuffer::getLineLength( const Font &font, const std::string &text ) const {
    auto accessor = this->environment_r->font_draw_2d.resource_id_to_font.find( font.resource_id );
    std::string filtered_text;

    if( accessor == this->environment_r->font_draw_2d.resource_id_to_font.end() )
        return 0.0f;

    auto font_resource_r = (*accessor).second->font_r;

    if( font_resource_r == nullptr )
        return 0.0f;

    font_resource_r->filterText( text, &filtered_text );

    return font.scale * static_cast<float>(font_resource_r->getLineLength( filtered_text ));
}

int Text2DBuffer::setFont( const Font &font ) {
    this->current_font = font;

    return 1;
}

int Text2DBuffer::setPosition( const glm::vec2 &position ) {
    this->position = position;

    return 1;
}

int Text2DBuffer::setColor( const glm::vec4 &color ) {
    this->color = static_cast<float>(0xff) * color;

    return 1;
}

int Text2DBuffer::setCenterMode( enum CenterMode center_mode ) {
    this->center_mode = center_mode;

    return 1;
}
int Text2DBuffer::print( const std::string &text ) { return -1; }

void Text2DBuffer::beginBox() {
    this->start = glm::vec2( std::numeric_limits<float>::max());
    this->end   = glm::vec2(-std::numeric_limits<float>::max());
}

glm::vec2 Text2DBuffer::getBoxStart() const {
    return this->start;
}

glm::vec2 Text2DBuffer::getBoxEnd() const {
    return this->end;
}

int Text2DBuffer::reset() {
    if(this->environment_r == nullptr)
        return -1;

    this->environment_r->font_draw_2d.clear();

    return 1;
}

}
