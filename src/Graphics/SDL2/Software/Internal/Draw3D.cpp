#include "Draw3D.h"

namespace Graphics::SDL2::Software::Internal {

bool Draw3D::load( const Data::Accessor &accessor ) {
    world_draw_3d.load(accessor);

    return true;
}

void Draw3D::draw(Window::RenderingRect &rendering_rect) const {
    world_draw_3d.drawOpaque(rendering_rect);

    world_draw_3d.draw(rendering_rect);
}

}
