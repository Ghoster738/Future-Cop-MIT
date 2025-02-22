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

    // OpenGL Info
    image_p->environment_r = this;
    image_p->texture_2d_p  = nullptr;

    return image_p;
}

ExternalImage::~ExternalImage() {
    if(this->texture_2d_p != nullptr)
        delete this->texture_2d_p;

    this->environment_r->draw_2d_routine.removeExternalImageData(this);
}

void ExternalImage::update() {
    Internal::Draw2D::ExternalImageData external_image_data;

    if(this->image_2d_r == nullptr)
        return;

    external_image_data.positions[0] = this->positions[0];
    external_image_data.positions[1] = this->positions[1];
    external_image_data.color        = this->color;
    external_image_data.visable      = this->is_visable;
    external_image_data.texture_2d   = this->texture_2d_p;

    this->environment_r->draw_2d_routine.updateExternalImageData(this, external_image_data);
}

void ExternalImage::upload() {
    if(this->image_2d_r == nullptr)
        return;

    if(this->texture_2d_p != nullptr)
        delete this->texture_2d_p;
    this->texture_2d_p = new Internal::Texture2D;

    //TODO Add type safety!

    this->texture_2d_p->setFilters( 0, GL_NEAREST, GL_LINEAR );
    this->texture_2d_p->setImage(0, 0, GL_RGB, this->image_2d_r->getWidth(), this->image_2d_r->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, this->image_2d_r->getDirectGridData() );
}

}
}
}
