#include "ExternalImage.h"

namespace {
const Utilities::PixelFormatColor_R8G8B8   RGB_COLOR_FORMAT;
const Utilities::PixelFormatColor_R8G8B8A8 RGB_ALPHA_COLOR_FORMAT;
}

namespace Graphics::SDL2::GLES2 {

Graphics::ExternalImage* Environment::allocateExternalImage(bool has_alpha) {
    Graphics::SDL2::GLES2::ExternalImage *image_p;

    if(has_alpha) {
        image_p = new Graphics::SDL2::GLES2::ExternalImage(RGB_ALPHA_COLOR_FORMAT);
        image_p->image_gl_format = GL_RGBA;
    }
    else {
        image_p = new Graphics::SDL2::GLES2::ExternalImage(RGB_COLOR_FORMAT);
        image_p->image_gl_format = GL_RGB;
    }

    // ImageBase
    image_p->positions[0]      = glm::vec2(-1.0, -1.0);
    image_p->positions[1]      = glm::vec2( 1.0,  1.0);
    image_p->color             = glm::vec4( 1.0,  1.0,  1.0,  1.0);
    image_p->is_visable        = false;

    // OpenGL Info
    image_p->environment_r = this;

    return image_p;
}

ExternalImage::ExternalImage(const Utilities::PixelFormatColor &color) : Graphics::ExternalImage(color) {
}

ExternalImage::~ExternalImage() {
    this->environment_r->draw_2d_routine.removeDynamicImageData(this);
}

void ExternalImage::update() {
    Internal::Draw2D::DynamicImageData dynamic_image_data;

    dynamic_image_data.positions[0] = this->positions[0];
    dynamic_image_data.positions[1] = this->positions[1];
    dynamic_image_data.color        = this->color;
    dynamic_image_data.visable      = this->is_visable;

    this->environment_r->draw_2d_routine.updateDynamicImageData(this, dynamic_image_data);
}

void ExternalImage::upload() {
    this->environment_r->draw_2d_routine.uploadDynamicImageData(this, this->image_2d, this->image_gl_format);
}

}
