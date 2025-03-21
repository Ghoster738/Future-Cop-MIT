#ifndef GRAPHICS_SDL2_SOFTWARE_INTERNAL_FONT_DRAW_2D_H
#define GRAPHICS_SDL2_SOFTWARE_INTERNAL_FONT_DRAW_2D_H

#include "../../../../Utilities/GridBase2D.h"

#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

#include <map>
#include <vector>
#include <unordered_set>

namespace Data {

class Accessor;

namespace Mission {

class FontResource;

}

}

namespace Graphics::SDL2::Software {

class Environment;
class Text2DBuffer;

namespace Internal {

class FontDraw2D {
public:
    struct FontGraphics {
        const Data::Mission::FontResource* font_r;
        std::map<uint32_t, Utilities::GridBase2D<uint8_t>*> font_glyphs;
    };

    struct Glyph {
        Utilities::GridBase2D<uint8_t> *glyph_texture_r;
        glm::i32vec2 position;
        float        scale;
        glm::u8vec4  color;
    };

    std::map<uint32_t, FontGraphics*> resource_id_to_font;
    std::unordered_set<Software::Text2DBuffer*> text_2d_buffers;

private:
    size_t current_glyph_amount;
    std::vector<Glyph> glyphs;

public:
    virtual ~FontDraw2D();

    void allocateGlyph(size_t num_glyphs);
    bool addGlyph(const Glyph &glyph);
    bool adjustGlyphs(size_t glyph_added_count, int32_t move_by_x);
    void clear();

    bool load( const Data::Accessor &accessor );

    void drawOpaque(Software::Environment *enviornment_r);
    void draw(Software::Environment *enviornment_r);
};

}
}

#endif // GRAPHICS_SDL2_SOFTWARE_INTERNAL_FONT_DRAW_2D_H
