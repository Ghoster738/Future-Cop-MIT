#include "FontDraw2D.h"

#include "../../../../Data/Accessor.h"
#include "../../../../Data/Mission/FontResource.h"

namespace Graphics::SDL2::Software::Internal {

FontDraw2D::~FontDraw2D() {
    for( auto font : this->resource_id_to_font ) {
        for( auto glyph : font.second->font_glyphs ) {
            delete glyph.second;
        }
        delete font.second;
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
        delete font.second;
    }
    this->resource_id_to_font.clear();

    std::vector<const Data::Mission::FontResource*> fonts = accessor.getAllConstFNT();
    for(const Data::Mission::FontResource* font_r : fonts ) {
        this->resource_id_to_font[font_r->getResourceID()] = new FontGraphics;
        this->resource_id_to_font[font_r->getResourceID()]->font_r = font_r;

        for(unsigned g = 0x100; g != 0; g--) {
            auto glyph_r = font_r->getGlyph( g - 1 );

            if( glyph_r == nullptr )
                continue;

            auto glyph_texture_p = new Utilities::GridBase2D<uint8_t>(glyph_r->width, glyph_r->height);

            for(unsigned y = glyph_r->height; y != 0; y--) {
                for(unsigned x = glyph_r->width; x != 0; x--) {
                    auto pixel = font_r->getImage()->readPixel(glyph_r->left + x, glyph_r->top + y);

                    glyph_texture_p->setValue(x, y, 0xff * pixel.red);
                }
            }

            this->resource_id_to_font[font_r->getResourceID()]->font_glyphs[g - 1] = glyph_texture_p;
        }

    }

    return fonts.size() != 0;
}

void FontDraw2D::drawOpaque(Software::Environment *enviornment_r) {}

void FontDraw2D::draw(Software::Environment *enviornment_r) {}

}
