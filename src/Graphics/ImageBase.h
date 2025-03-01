#ifndef GRAPHICS_IMAGE_BASE_H
#define GRAPHICS_IMAGE_BASE_H

#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

namespace Graphics {

/**
 * @note This class is abstract.
 * Image and ImageExternal inheriets from this class.
 */
class ImageBase {
public:
    glm::vec2 positions[2]; // Screen coordinates
    glm::vec4 color; // The color of the image.
    bool      is_visable; // Determines whether the image is visable.

    virtual ~ImageBase() = 0;

    /**
     * This is to update the ImageBase to the graphics.
     * @warning Calling this is required.
     */
    virtual void update() = 0;
};

}

#endif // GRAPHICS_IMAGE_BASE_H
