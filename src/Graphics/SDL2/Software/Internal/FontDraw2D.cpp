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

bool FontDraw2D::getGlyphBox(size_t glyph_added_count, glm::vec2 &start, glm::vec2 &end) const {
    if(this->current_glyph_amount < glyph_added_count)
        return false;

    glm::ivec2 positions[2];

    for(size_t g = current_glyph_amount - glyph_added_count; g != current_glyph_amount; g++) {
        positions[0] = this->glyphs[g].position;
        positions[1] = this->glyphs[g].position + glm::ivec2(this->glyphs[g].glyph_texture_r->getWidth() * this->glyphs[g].scale, this->glyphs[g].glyph_texture_r->getHeight() * this->glyphs[g].scale);

        if(positions[0].x < start.x)
            start.x = positions[0].x;
        if(positions[0].y < start.y)
            start.y = positions[0].y;
        if(positions[1].x < start.x)
            start.x = positions[1].x;
        if(positions[1].y < start.y)
            start.y = positions[1].y;

        if(positions[0].x > end.x)
            end.x = positions[0].x;
        if(positions[0].y > end.y)
            end.y = positions[0].y;
        if(positions[1].x > end.x)
            end.x = positions[1].x;
        if(positions[1].y > end.y)
            end.y = positions[1].y;
    }

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

            for(auto y = glyph_texture_p->getHeight(); y != 0; y--) {
                for(auto x = glyph_texture_p->getWidth(); x != 0; x--) {
                    auto pixel = font_r->getImage()->readPixel(glyph_r->left + (x - 1), glyph_r->top + (y - 1));

                    glyph_texture_p->setValue((x - 1), (y - 1), 0xff * pixel.red);
                }
            }

            this->resource_id_to_font[font_r->getResourceID()]->font_glyphs[g - 1] = glyph_texture_p;
        }

    }

    return fonts.size() != 0;
}

void FontDraw2D::drawOpaque(Window::RenderingRect &rendering_rect) const {
    Window::DifferredPixel default_pixel;
    default_pixel.colors[3] = 0;
    default_pixel.depth = 0;
    default_pixel.depth--;

    for(size_t g = 0; g < this->current_glyph_amount; g++) {
        const auto &glyph = this->glyphs[g];

        if(glyph.color[3] != 0xff)
            continue;

        default_pixel.colors[0] = glyph.color[0];
        default_pixel.colors[1] = glyph.color[1];
        default_pixel.colors[2] = glyph.color[2];

        if(glyph.scale == 1.f) {
            for(auto rev_y = glyph.glyph_texture_r->getHeight(); rev_y != 0; rev_y--) {
                auto y = glyph.glyph_texture_r->getHeight() - rev_y;

                for(auto rev_x = glyph.glyph_texture_r->getWidth(); rev_x != 0; rev_x--) {
                    auto x = glyph.glyph_texture_r->getWidth() - rev_x;

                    auto pixel = glyph.glyph_texture_r->getValue(x, y);

                    if(pixel != 0)
                        rendering_rect.differred_buffer.setValue(
                            (glyph.position.x + x) - rendering_rect.area.start_x,
                            (glyph.position.y + y) - rendering_rect.area.start_y,
                            default_pixel);
                }
            }
        }
        else {
            unsigned scale_x = glyph.scale * glyph.glyph_texture_r->getWidth();
            unsigned scale_y = glyph.scale * glyph.glyph_texture_r->getHeight();

            for(unsigned rev_y = scale_y; rev_y != 0; rev_y--) {
                auto y = scale_y - rev_y;

                for(unsigned rev_x = scale_x; rev_x != 0; rev_x--) {
                    auto x = scale_x - rev_x;

                    auto pixel = glyph.glyph_texture_r->getValue(1 / glyph.scale * x, 1 / glyph.scale * y);

                    if(pixel != 0)
                        rendering_rect.differred_buffer.setValue(
                            (glyph.position.x + x) - rendering_rect.area.start_x,
                            (glyph.position.y + y) - rendering_rect.area.start_y,
                            default_pixel);
                }
            }
        }
    }
}

void FontDraw2D::draw(Window::RenderingRect &rendering_rect) const {
    Window::DifferredPixel default_pixel;
    Window::DifferredPixel original_pixel;
    Window::DifferredPixel new_pixel;
    float alpha_value, inv_alpha_value;
    default_pixel.colors[3] = 0;
    default_pixel.depth = 0;
    default_pixel.depth--;

    for(size_t g = 0; g < this->current_glyph_amount; g++) {
        const auto &glyph = this->glyphs[g];

        if(glyph.color[3] == 0xff)
            continue;

        alpha_value     = 1.f / 255.f * glyph.color[3];
        inv_alpha_value = 1.f - alpha_value;

        default_pixel.colors[0] = glyph.color[0];
        default_pixel.colors[1] = glyph.color[1];
        default_pixel.colors[2] = glyph.color[2];

        if(glyph.scale == 1.f) {
            for(auto rev_y = glyph.glyph_texture_r->getHeight(); rev_y != 0; rev_y--) {
                auto y = glyph.glyph_texture_r->getHeight() - rev_y;

                for(auto rev_x = glyph.glyph_texture_r->getWidth(); rev_x != 0; rev_x--) {
                    auto x = glyph.glyph_texture_r->getWidth() - rev_x;

                    auto pixel = glyph.glyph_texture_r->getValue(x, y);

                    if(pixel == 0)
                        continue;

                    original_pixel = rendering_rect.differred_buffer.getValue(
                        (glyph.position.x + x) - rendering_rect.area.start_x,
                        (glyph.position.y + y) - rendering_rect.area.start_y);

                    new_pixel = default_pixel;

                    new_pixel.colors[0] = default_pixel.colors[0] * alpha_value + original_pixel.colors[0] * inv_alpha_value;
                    new_pixel.colors[1] = default_pixel.colors[1] * alpha_value + original_pixel.colors[1] * inv_alpha_value;
                    new_pixel.colors[2] = default_pixel.colors[2] * alpha_value + original_pixel.colors[2] * inv_alpha_value;

                    rendering_rect.differred_buffer.setValue(
                        (glyph.position.x + x) - rendering_rect.area.start_x,
                        (glyph.position.y + y) - rendering_rect.area.start_y,
                        new_pixel);
                }
            }
        }
        else {
            unsigned scale_x = glyph.scale * glyph.glyph_texture_r->getWidth();
            unsigned scale_y = glyph.scale * glyph.glyph_texture_r->getHeight();

            for(unsigned rev_y = scale_y; rev_y != 0; rev_y--) {
                auto y = scale_y - rev_y;

                for(unsigned rev_x = scale_x; rev_x != 0; rev_x--) {
                    auto x = scale_x - rev_x;

                    auto pixel = glyph.glyph_texture_r->getValue(1 / glyph.scale * x, 1 / glyph.scale * y);

                    if(pixel == 0)
                        continue;

                    original_pixel = rendering_rect.differred_buffer.getValue(
                            (glyph.position.x + x) - rendering_rect.area.start_x,
                            (glyph.position.y + y) - rendering_rect.area.start_y);

                    new_pixel = default_pixel;

                    new_pixel.colors[0] = default_pixel.colors[0] * alpha_value + original_pixel.colors[0] * inv_alpha_value;
                    new_pixel.colors[1] = default_pixel.colors[1] * alpha_value + original_pixel.colors[1] * inv_alpha_value;
                    new_pixel.colors[2] = default_pixel.colors[2] * alpha_value + original_pixel.colors[2] * inv_alpha_value;

                    rendering_rect.differred_buffer.setValue(
                            (glyph.position.x + x) - rendering_rect.area.start_x,
                            (glyph.position.y + y) - rendering_rect.area.start_y,
                            new_pixel);
                }
            }
        }
    }
}

}
