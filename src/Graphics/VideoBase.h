#ifndef GRAPHICS_VIDEO_BASE_H
#define GRAPHICS_VIDEO_BASE_H

#include "ImageBase.h"

namespace Graphics {

class VideoBase : public ImageBase {
public:
    virtual void repeat() = 0;
    virtual void play() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;

    /**
     * @warning Calling this is required.
     */
    virtual void update() = 0;
};

}

#endif // GRAPHICS_VIDEO_BASE_H
