#ifndef GRAPHICS_SDL2_SOFTWARE_INTERNAL_EXTERNAL_IMAGE_DRAW_2D_H
#define GRAPHICS_SDL2_SOFTWARE_INTERNAL_EXTERNAL_IMAGE_DRAW_2D_H

#include "../Window.h"

#include <set>

namespace Graphics::SDL2::Software {

class Environment;
class ExternalImage;

namespace Internal {

class ExternalImageDraw2D {
public:
    std::set<Software::ExternalImage*> opaque_images;
    std::set<Software::ExternalImage*> images;

    void drawOpaque(Window::RenderingRect &rendering_rect) const;
    void draw(Window::RenderingRect &rendering_rect) const;
};

}
}

#endif // GRAPHICS_SDL2_SOFTWARE_INTERNAL_EXTERNAL_IMAGE_DRAW_2D_H
