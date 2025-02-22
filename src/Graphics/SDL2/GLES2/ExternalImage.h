#ifndef GRAPHICS_SDL2_GLES2_EXTERNAL_IMAGE_H
#define GRAPHICS_SDL2_GLES2_EXTERNAL_IMAGE_H

#include "../../ExternalImage.h"

#include "Environment.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

class ExternalImage : public Graphics::ExternalImage {
public:
    Environment *environment_r;
    Internal::Texture2D *texture_2d_p;

    virtual ~ExternalImage();

    virtual void update();

    virtual void upload();
};

}
}
}

#endif // GRAPHICS_SDL2_GLES2_EXTERNAL_IMAGE_H
