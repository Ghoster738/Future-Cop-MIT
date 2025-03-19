#ifndef GRAPHICS_SDL2_SOFTWARE_INTERNAL_EXTERNAL_IMAGE_DRAW_2D_H
#define GRAPHICS_SDL2_SOFTWARE_INTERNAL_EXTERNAL_IMAGE_DRAW_2D_H

#include <set>

namespace Graphics::SDL2::Software {

class Environment;
class ExternalImage;

namespace Internal {

class ExternalImageDraw2D {
public:
    std::set<Software::ExternalImage*> opaque_images;
    std::set<Software::ExternalImage*> images;

    void drawOpaque(Software::Environment *enviornment_r);
    void drawSemiTransparent(Software::Environment *enviornment_r);
};

}
}

#endif // GRAPHICS_SDL2_SOFTWARE_INTERNAL_EXTERNAL_IMAGE_DRAW_2D_H
