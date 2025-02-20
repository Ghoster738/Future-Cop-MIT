#ifndef GRAPHICS_IMAGE_H
#define GRAPHICS_IMAGE_H

#include "ImageBase.h"

namespace Graphics {

class Image : public ImageBase {
public:
    glm::vec2 texture_coords[2];

    /**
     * @warning Calling this is required.
     */
    virtual void update() = 0;
};

}

#endif // GRAPHICS_IMAGE_H
