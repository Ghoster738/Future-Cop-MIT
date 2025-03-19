#include "ExternalImageDraw2D.h"


#include "../Environment.h"
#include "../ExternalImage.h"

namespace Graphics::SDL2::Software::Internal {

void ExternalImageDraw2D::drawOpaque(Software::Environment *env_r) {
    for( auto i : this->opaque_images ) {
        if(!i->internal.is_visable)
            continue;

        if(i->internal.positions[1].x < i->internal.positions[0].x)
            std::swap(i->internal.positions[0].x, i->internal.positions[1].x);
        if(i->internal.positions[1].y < i->internal.positions[0].y)
            std::swap(i->internal.positions[0].y, i->internal.positions[1].y);

        glm::vec2 pos_0 = i->internal.positions[0] * env_r->window_p->inv_factor;
        glm::vec2 pos_1 = i->internal.positions[1] * env_r->window_p->inv_factor;
        glm::vec2 scale = pos_1 - pos_0;

        glm::i32vec2 screen_pos_0 = glm::clamp(glm::i32vec2( pos_0 ), glm::i32vec2(0, 0), glm::i32vec2(env_r->window_p->destination_buffer.getWidth(), env_r->window_p->destination_buffer.getHeight()) );

        glm::i32vec2 screen_pos_1 = glm::clamp(glm::i32vec2( pos_1 ), glm::i32vec2(0, 0), glm::i32vec2(env_r->window_p->destination_buffer.getWidth(), env_r->window_p->destination_buffer.getHeight()) );

        Window::DifferredPixel default_pixel;
        default_pixel.colors[3] = 0;
        default_pixel.depth = 0;
        default_pixel.depth--;

        for(auto y = screen_pos_0.y; y != screen_pos_1.y; y++) {
            default_pixel.texture_coordinates[1] = i->image_2d.getHeight() * ((pos_1.y - y) / scale.y);

            for(auto x = screen_pos_0.x; x != screen_pos_1.x; x++) {
                default_pixel.texture_coordinates[0] = i->image_2d.getWidth() * ((pos_1.x - x) / scale.x);

                auto pixel = i->image_2d.readPixel(default_pixel.texture_coordinates[0], default_pixel.texture_coordinates[1]);

                Window::DifferredPixel source_pixel = default_pixel;

                source_pixel.colors[0] = 0xff * (i->internal.color.r * pixel.red);
                source_pixel.colors[1] = 0xff * (i->internal.color.g * pixel.green);
                source_pixel.colors[2] = 0xff * (i->internal.color.b * pixel.blue);

                env_r->window_p->differred_buffer.setValue(x, y, source_pixel);
            }
        }
    }
}

void ExternalImageDraw2D::drawSemiTransparent(Software::Environment *env_r) {}

}
