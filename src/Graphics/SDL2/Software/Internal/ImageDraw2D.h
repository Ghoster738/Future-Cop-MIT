#ifndef GRAPHICS_SDL2_SOFTWARE_INTERNAL_IMAGE_DRAW_2D_H
#define GRAPHICS_SDL2_SOFTWARE_INTERNAL_IMAGE_DRAW_2D_H

#include "../Window.h"

#include <set>

namespace Graphics::SDL2::Software {

class Environment;
class Image;

namespace Internal {

class ImageDraw2D {
public:
    std::set<Software::Image*> images;

    void draw(Window::RenderingRect &rendering_rect);
};

}
}

#endif // GRAPHICS_SDL2_SOFTWARE_INTERNAL_IMAGE_DRAW_2D_H
