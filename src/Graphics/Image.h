#ifndef GRAPHICS_IMAGE_H
#define GRAPHICS_IMAGE_H

#include "ImageBase.h"

#include <cstdint>

namespace Graphics {

class Image : public ImageBase {
public:
    uint32_t  cbmp_id;
    glm::vec2 texture_coords[2];

    /**
     * @warning Calling this is required.
     */
    virtual void update() = 0;
};

}

#endif // GRAPHICS_IMAGE_H
