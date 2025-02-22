#include "ExternalImage.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

Graphics::ExternalImage* Environment::allocateExternalImage() {
    auto image_p = new Graphics::SDL2::GLES2::ExternalImage();

    // ImageBase
    image_p->positions[0]      = glm::vec2(-1.0, -1.0);
    image_p->positions[1]      = glm::vec2( 1.0,  1.0);
    image_p->color             = glm::vec4( 1.0,  1.0,  1.0,  1.0);
    image_p->is_visable        = false;

    // ExternalImage
    image_p->image_2d_r = nullptr;

    return image_p;
}

ExternalImage::~ExternalImage() {
    this->environment_r->draw_2d_routine.removeExternalImageData(this);
}

void ExternalImage::update() {}


}
}
}
