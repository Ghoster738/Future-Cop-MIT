#ifndef GRAPHICS_SDL2_GLES2_ANM_FRAME_H
#define GRAPHICS_SDL2_GLES2_ANM_FRAME_H

#include "../../ANMFrame.h"

#include "Environment.h"

#include "../../../Data/Mission/ANMResource.h"

namespace Graphics:: SDL2::GLES2 {

class ANMFrame : public Graphics::ANMFrame {
public:
    Data::Mission::ANMResource::Video video;
    Environment *environment_r;
    size_t total_frames;

    ANMFrame(const Data::Mission::ANMResource &resource, const Utilities::PixelFormatColor &color);

    virtual ~ANMFrame();

    virtual bool nextFrame();

    virtual bool setFrameIndex(unsigned index);

    virtual unsigned getFrameAmount() const;

    virtual void update();

    virtual void upload();
};

}

#endif // GRAPHICS_SDL2_GLES2_ANM_FRAME_H
