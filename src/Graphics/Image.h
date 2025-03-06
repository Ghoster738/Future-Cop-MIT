#ifndef GRAPHICS_IMAGE_H
#define GRAPHICS_IMAGE_H

#include "ImageBase.h"

#include <cstdint>

namespace Graphics {

class Image : public ImageBase {
public:
    uint32_t  cbmp_id; // The texture cbmp_id to use.
    glm::vec2 texture_coords[2]; // The texture coordinates to be used in a square.

    /**
     * Update the positions, color, visability, cbmp_id, and texture_coords to the graphics renderer.
     * @warning Calling this is required.
     */
    virtual void update() = 0;
};

}

#endif // GRAPHICS_IMAGE_H
