#ifndef GRAPHICS_SDL2_SOFTWARE_IMAGE_H
#define GRAPHICS_SDL2_SOFTWARE_IMAGE_H

#include "../../Image.h"

#include "Environment.h"

namespace Graphics::SDL2::Software {

class Image : public Graphics::Image {
public:
    Environment *environment_r;

    virtual ~Image();

    virtual void update();
};

}

#endif // GRAPHICS_SDL2_SOFTWARE_IMAGE_H
