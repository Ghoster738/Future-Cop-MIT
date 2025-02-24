#ifndef GRAPHICS_SDL2_GLES2_VIDEO_ANM_H
#define GRAPHICS_SDL2_GLES2_VIDEO_ANM_H

#include "../../VideoANM.h"

#include "Environment.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

class VideoANM : public Graphics::VideoANM {
public:
    Environment *environment_r;
    unsigned image_index;
    float factor;
    float seconds_to_next_frame;

    virtual ~VideoANM();

    virtual void play();
    virtual void stop();
    virtual void pause();

    virtual void update();
};

}
}
}

#endif // GRAPHICS_SDL2_GLES2_VIDEO_ANM_H
