#ifndef GRAPHICS_SDL2_SOFTWARE_INTERNAL_IMAGE_DRAW_2D_H
#define GRAPHICS_SDL2_SOFTWARE_INTERNAL_IMAGE_DRAW_2D_H

#include <set>

namespace Graphics::SDL2::Software {

class Environment;
class Image;

namespace Internal {

class ImageDraw2D {
public:
    std::set<Software::Image*> images;

    void draw(Software::Environment *enviornment_r);
};

}
}

#endif // GRAPHICS_SDL2_SOFTWARE_INTERNAL_IMAGE_DRAW_2D_H
