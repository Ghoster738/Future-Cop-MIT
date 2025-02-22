#include "Image.h"
#include "Internal/Draw2D.h"

namespace Graphics::SDL2::GLES2 {

Graphics::Image* Environment::allocateImage() {
    auto image_p = new Graphics::SDL2::GLES2::Image();

    // ImageBase
    image_p->positions[0]      = glm::vec2(-1.0, -1.0);
    image_p->positions[1]      = glm::vec2( 1.0,  1.0);
    image_p->color             = glm::vec4( 1.0,  1.0,  1.0,  1.0);
    image_p->is_visable        = false;

    // Image
    image_p->cbmp_id           = 1;
    image_p->texture_coords[0] = glm::vec2( 0.0,  0.0);
    image_p->texture_coords[1] = glm::vec2( 1.0,  1.0);

    // OpenGL Info
    image_p->environment_r  = this;
    image_p->cbmp_texture_r = nullptr;
    image_p->cbmp_last_id   = 0;

    return image_p;
}

Image::~Image() {
    if(this->cbmp_texture_r != nullptr)
        this->environment_r->draw_2d_routine.removeImageData(this->cbmp_texture_r, this);
}

void Image::update() {
    Internal::Draw2D::ImageData image_data;

    image_data.positions[0]      = this->positions[0];
    image_data.positions[1]      = this->positions[1];
    image_data.color             = this->color;
    image_data.visable           = this->is_visable;
    image_data.texture_coords[0] = this->texture_coords[0];
    image_data.texture_coords[1] = this->texture_coords[1];


    if(this->cbmp_texture_r == nullptr) {
        auto internal_texture_it = this->environment_r->textures.find( this->cbmp_id );

        if(internal_texture_it != this->environment_r->textures.end())
            this->cbmp_texture_r = internal_texture_it->second;
    }
    else if(this->cbmp_last_id != this->cbmp_id) {
        auto internal_texture_it = this->environment_r->textures.find( this->cbmp_id );

        if(internal_texture_it != this->environment_r->textures.end()) {
            this->environment_r->draw_2d_routine.removeImageData(this->cbmp_texture_r, this);
            this->cbmp_texture_r = internal_texture_it->second;
        }
    }

    this->cbmp_last_id = this->cbmp_id;

    if(this->cbmp_texture_r != nullptr)
        this->environment_r->draw_2d_routine.updateImageData(this->cbmp_texture_r, this, image_data);
}

}
