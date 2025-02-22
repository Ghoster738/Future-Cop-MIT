#ifndef GRAPHICS_SDL2_GLES2_IMAGE_H
#define GRAPHICS_SDL2_GLES2_IMAGE_H

#include "../../Image.h"

#include "Environment.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

class Image : public Graphics::Image {
public:
    Environment *environment_r;
    const Internal::Texture2D *cbmp_texture_r;
    uint32_t                   cbmp_last_id;

    virtual ~Image();

    virtual void update();
};

}
}
}

#endif // GRAPHICS_SDL2_GLES2_IMAGE_H
