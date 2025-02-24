#ifndef GRAPHICS_VIDEO_ANM_FRAME_H
#define GRAPHICS_VIDEO_ANM_FRAME_H

#include "ExternalImage.h"

namespace Graphics {

class ANMFrame : public ExternalImage {
public:
    ANMFrame(const Utilities::PixelFormatColor &color) : ExternalImage(color) {}

    virtual bool nextFrame() = 0;
    virtual bool setFrameIndex(unsigned index) = 0;
    virtual unsigned getFrameAmount() const = 0;
};

}

#endif // GRAPHICS_VIDEO_ANM_FRAME_H
