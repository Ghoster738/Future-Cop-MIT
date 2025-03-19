#ifndef GRAPHICS_SDL2_SOFTWARE_IMAGE_H
#define GRAPHICS_SDL2_SOFTWARE_IMAGE_H

#include "../../Image.h"

namespace Graphics::SDL2::Software {

class Environment;

class Image : public Graphics::Image {
public:
    Environment *environment_r;

    struct {
        glm::vec4 color; // The color of the image.
        glm::vec2 positions[2]; // Screen coordinates
        glm::vec2 texture_coords[2];
        uint32_t  cbmp_index;
        bool      is_visable; // Determines whether the image is visable.
    } internal;

    virtual ~Image();

    virtual void update();
};

}

#endif // GRAPHICS_SDL2_SOFTWARE_IMAGE_H
