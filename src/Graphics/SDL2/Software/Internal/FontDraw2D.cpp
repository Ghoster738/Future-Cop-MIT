#include "FontDraw2D.h"

#include "../../../../Data/Accessor.h"
#include "../../../../Data/Mission/FontResource.h"

#include "../Environment.h"

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
    this->current_glyph_amount = 0;
    this->glyphs.resize(num_glyphs);
}

bool FontDraw2D::adjustGlyphs(size_t glyph_added_count, int32_t move_by_x) {
    if(this->current_glyph_amount < glyph_added_count)
        return false;

    for(size_t g = current_glyph_amount - glyph_added_count; g != current_glyph_amount; g++) {
        this->glyphs[g].position.x += move_by_x;
    }
    return true;
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

            for(auto y = glyph_r->height; y != 0; y--) {
                for(auto x = glyph_r->width; x != 0; x--) {
                    auto pixel = font_r->getImage()->readPixel(glyph_r->left + x, glyph_r->top + y);

                    glyph_texture_p->setValue(x, y, 0xff * pixel.red);
                }
            }

            this->resource_id_to_font[font_r->getResourceID()]->font_glyphs[g - 1] = glyph_texture_p;
        }

    }

    return fonts.size() != 0;
}

void FontDraw2D::drawOpaque(Software::Environment *env_r) {
    Window::DifferredPixel default_pixel;
    default_pixel.colors[3] = 0;
    default_pixel.depth = 0;
    default_pixel.depth--;

    for(size_t g = 0; g < this->current_glyph_amount; g++) {
        const auto &glyph = this->glyphs[g];

        default_pixel.colors[0] = glyph.color[0];
        default_pixel.colors[1] = glyph.color[1];
        default_pixel.colors[2] = glyph.color[2];

        for(auto y = glyph.glyph_texture_r->getHeight(); y != 0; y--) {
            for(auto x = glyph.glyph_texture_r->getWidth(); x != 0; x--) {
                auto pixel = glyph.glyph_texture_r->getValue(x, y);

                if(pixel != 0)
                    env_r->window_p->differred_buffer.setValue(glyph.position.x + x, glyph.position.y + y, default_pixel);
            }
        }
    }
}

void FontDraw2D::draw(Software::Environment *enviornment_r) {}

}
