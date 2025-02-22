#ifndef GRAPHICS_IMAGE_BASE_H
#define GRAPHICS_IMAGE_BASE_H

#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

namespace Graphics {

class ImageBase {
public:
    glm::vec2 positions[2];
    glm::vec4 color;
    bool      is_visable;

    virtual ~ImageBase() = 0;

    float getAspectRatio() const;

    /**
     * @warning Calling this is required.
     */
    virtual void update() = 0;
};

}

#endif // GRAPHICS_IMAGE_BASE_H
