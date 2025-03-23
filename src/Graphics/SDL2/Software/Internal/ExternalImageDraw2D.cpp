#include "ExternalImageDraw2D.h"


#include "../Environment.h"
#include "../ExternalImage.h"

namespace Graphics::SDL2::Software::Internal {

void ExternalImageDraw2D::drawOpaque(Window::RenderingRect &rendering_rect) const {
    for( auto i : this->opaque_images ) {
        if(!i->internal.is_visable)
            continue;

        if(i->internal.positions[1].x < i->internal.positions[0].x)
            std::swap(i->internal.positions[0].x, i->internal.positions[1].x);
        if(i->internal.positions[1].y < i->internal.positions[0].y)
            std::swap(i->internal.positions[0].y, i->internal.positions[1].y);

        glm::vec2 pos_0 = i->internal.positions[0] * rendering_rect.env_r->window_p->inv_factor;
        glm::vec2 pos_1 = i->internal.positions[1] * rendering_rect.env_r->window_p->inv_factor;
        glm::vec2 scale = pos_1 - pos_0;

        glm::i32vec2 screen_pos_0 = glm::clamp(glm::i32vec2( pos_0 ), glm::i32vec2(0, 0), glm::i32vec2(rendering_rect.env_r->window_p->destination_buffer.getWidth(), rendering_rect.env_r->window_p->destination_buffer.getHeight()) );

        glm::i32vec2 screen_pos_1 = glm::clamp(glm::i32vec2( pos_1 ), glm::i32vec2(0, 0), glm::i32vec2(rendering_rect.env_r->window_p->destination_buffer.getWidth(), rendering_rect.env_r->window_p->destination_buffer.getHeight()) );

        Window::DifferredPixel default_pixel;
        default_pixel.colors[3] = 0;
        default_pixel.depth = 0;
        default_pixel.depth--;

        for(auto y = screen_pos_0.y; y != screen_pos_1.y; y++) {
            auto v = i->image_2d.getHeight() * ((y - pos_0.y) / scale.y);

            for(auto x = screen_pos_0.x; x != screen_pos_1.x; x++) {
                auto u = i->image_2d.getWidth() * ((x - pos_0.x) / scale.x);

                auto pixel = i->image_2d.getRef(u, v);

                Window::DifferredPixel source_pixel = default_pixel;

                source_pixel.colors[0] = (i->internal.color.r * pixel[0]);
                source_pixel.colors[1] = (i->internal.color.g * pixel[1]);
                source_pixel.colors[2] = (i->internal.color.b * pixel[2]);

                rendering_rect.differred_buffer.setValue(x, y, source_pixel);
            }
        }
    }
}

void ExternalImageDraw2D::draw(Window::RenderingRect &rendering_rect) const {
    // TODO Handle semi-transparancy.
}

}
