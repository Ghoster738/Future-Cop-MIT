#ifndef GRAPHICS_EXTERNAL_IMAGE_H
#define GRAPHICS_EXTERNAL_IMAGE_H

#include "ImageBase.h"
#include "../Utilities/Image2D.h"

namespace Graphics {

class ExternalImage : public ImageBase {
public:
    /**
     * This is the image that this class renders.
     */
    Utilities::Image2D image_2d;

    /**
     * @warning This method is for internal use only. Use allocateExternalImage in Environment to allocate this class.
     */
    ExternalImage(const Utilities::PixelFormatColor &color) : image_2d(1, 1, color) {}

    /**
     * Update the positions, color and visability to the graphics renderer.
     * @warning Calling this is required.
     */
    virtual void update() = 0;

    /**
     * This "uploads" image_2d to the graphics renderer.
     * @warning Calling this is required.
     */
    virtual void upload() = 0;
};

}

#endif // GRAPHICS_EXTERNAL_IMAGE_H
