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
        //image_p->image_2d = Utilities::Image2D(1, 1, RGB_COLOR_FORMAT);
        image_p->image_gl_format = GL_RGB;
    }

    // ImageBase
    image_p->positions[0]      = glm::vec2(-1.0, -1.0);
    image_p->positions[1]      = glm::vec2( 1.0,  1.0);
    image_p->color             = glm::vec4( 1.0,  1.0,  1.0,  1.0);
    image_p->is_visable        = false;

    // OpenGL Info
    image_p->environment_r = this;
    image_p->texture_2d_p  = nullptr;
    image_p->width         = image_p->image_2d.getWidth();
    image_p->height        = image_p->image_2d.getHeight();

    return image_p;
}

ExternalImage::ExternalImage(const Utilities::PixelFormatColor &color) : Graphics::ExternalImage(color) {
}

ExternalImage::~ExternalImage() {
    if(this->texture_2d_p != nullptr)
        delete this->texture_2d_p;

    this->environment_r->draw_2d_routine.removeExternalImageData(this);
}

void ExternalImage::update() {
    Internal::Draw2D::ExternalImageData external_image_data;

    external_image_data.positions[0] = this->positions[0];
    external_image_data.positions[1] = this->positions[1];
    external_image_data.color        = this->color;
    external_image_data.visable      = this->is_visable;
    external_image_data.texture_2d   = this->texture_2d_p;

    this->environment_r->draw_2d_routine.updateExternalImageData(this, external_image_data);
}

void ExternalImage::upload() {
    if(this->texture_2d_p == nullptr || (this->image_2d.getWidth() != this->width || this->image_2d.getHeight() != this->height)) {
        if(this->texture_2d_p != nullptr)
            delete this->texture_2d_p;

        this->texture_2d_p = new Internal::Texture2D;

        this->texture_2d_p->setFilters( 0, GL_NEAREST, GL_LINEAR);
        this->texture_2d_p->setImage(0, 0, image_gl_format, this->image_2d.getWidth(), this->image_2d.getHeight(), 0, image_gl_format, GL_UNSIGNED_BYTE, this->image_2d.getDirectGridData());

        this->width  = this->image_2d.getWidth();
        this->height = this->image_2d.getHeight();
    }
    else {
        this->texture_2d_p->updateImage(0, 0, this->image_2d.getWidth(), this->image_2d.getHeight(), image_gl_format, GL_UNSIGNED_BYTE, this->image_2d.getDirectGridData());
    }
}

}
