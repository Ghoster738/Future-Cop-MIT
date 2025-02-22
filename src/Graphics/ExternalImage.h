#ifndef GRAPHICS_EXTERNAL_IMAGE_H
#define GRAPHICS_EXTERNAL_IMAGE_H

#include "ImageBase.h"
#include "../Utilities/Image2D.h"

namespace Graphics {

class ExternalImage : public ImageBase {
public:
    const Utilities::Image2D *image_2d_r;

    /**
     * @warning Calling this is required.
     */
    virtual void update() = 0;

    /**
     * @warning Calling this is required.
     */
    virtual void upload() = 0;
};

}

#endif // GRAPHICS_EXTERNAL_IMAGE_H
