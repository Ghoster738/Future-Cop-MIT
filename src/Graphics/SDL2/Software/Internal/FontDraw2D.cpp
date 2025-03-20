#include "FontDraw2D.h"

#include "../../../../Data/Accessor.h"
#include "../../../../Data/Mission/FontResource.h"

namespace Graphics::SDL2::Software::Internal {

FontDraw2D::~FontDraw2D() {
    for( auto font : this->resource_id_to_font ) {
        for( auto glyph : font.second->font_glyphs ) {
            delete glyph.second;
        }
    }
}

void FontDraw2D::allocateGlyph(size_t num_glyphs) {
    this->glyphs.resize(num_glyphs);
}

bool FontDraw2D::addGlyph(const Glyph &glyph) {
    if(this->glyphs.size() == current_glyph_amount)
        return false;

    this->glyphs[this->current_glyph_amount] = glyph;
    this->current_glyph_amount++;

    return true;
}

void FontDraw2D::clear() {
    this->current_glyph_amount = 0;
}

bool FontDraw2D::load( const Data::Accessor &accessor ) {
    for( auto font : this->resource_id_to_font ) {
        for( auto glyph : font.second->font_glyphs ) {
            delete glyph.second;
        }
    }
    this->resource_id_to_font.clear();

    std::vector<const Data::Mission::FontResource*> fonts = accessor.getAllConstFNT();
    for(const Data::Mission::FontResource* font_r : fonts ) {
        this->resource_id_to_font[font_r->getResourceID()];
        //TODO Load Glyphs.
    }

    return fonts.size() != 0;
}

void FontDraw2D::drawOpaque(Software::Environment *enviornment_r) {}

void FontDraw2D::draw(Software::Environment *enviornment_r) {}

}
