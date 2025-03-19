#ifndef GRAPHICS_SDL2_SOFTWARE_EXTERNAL_IMAGE_H
#define GRAPHICS_SDL2_SOFTWARE_EXTERNAL_IMAGE_H

#include "../../ExternalImage.h"

namespace Graphics::SDL2::Software {

class Environment;

class ExternalImage : public Graphics::ExternalImage {
public:
    Environment *environment_r;

    ExternalImage(const Utilities::PixelFormatColor &color);

    virtual ~ExternalImage();

    virtual void update();

    virtual void upload();
};

}

#endif // GRAPHICS_SDL2_SOFTWARE_EXTERNAL_IMAGE_H
