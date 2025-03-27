#ifndef GRAPHICS_SDL2_SOFTWARE_EXTERNAL_IMAGE_H
#define GRAPHICS_SDL2_SOFTWARE_EXTERNAL_IMAGE_H

#include "../../ExternalImage.h"

namespace Graphics::SDL2::Software {

class Environment;

class ExternalImage : public Graphics::ExternalImage {
public:
    Environment *environment_r;

    struct {
        glm::vec4 color; // The color of the image.
        glm::vec2 positions[2]; // Screen coordinates
        bool      is_visable; // Determines whether the image is visable.
    } internal;

    bool has_alpha_channel;

    ExternalImage(const Utilities::PixelFormatColor &color);

    virtual ~ExternalImage();

    virtual void update();

    virtual void upload();
};

}

#endif // GRAPHICS_SDL2_SOFTWARE_EXTERNAL_IMAGE_H
