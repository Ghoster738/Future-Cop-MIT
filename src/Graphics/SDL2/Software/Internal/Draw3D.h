#ifndef GRAPHICS_SDL2_SOFTWARE_INTERNAL_DRAW_3D_H
#define GRAPHICS_SDL2_SOFTWARE_INTERNAL_DRAW_3D_H

#include "../Window.h"

#include "WorldDraw3D.h"

namespace Data {

class Accessor;

}

namespace Graphics::SDL2::Software::Internal {

class Draw3D {
public:
    WorldDraw3D world_draw_3d;

    bool load( const Data::Accessor &accessor );

    void draw(Window::RenderingRect &rendering_rect) const;
};

}

#endif // GRAPHICS_SDL2_SOFTWARE_INTERNAL_DRAW_3D_H
