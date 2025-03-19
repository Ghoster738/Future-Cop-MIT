#include "Image.h"

#include "Environment.h"

namespace Graphics::SDL2::Software {

Graphics::Image* Environment::allocateImage() {
    Software::Image* image_p = new Image();

    // ImageBase
    image_p->positions[0]      = glm::vec2( 00.0, 00.0);
    image_p->positions[1]      = glm::vec2( 16.0, 16.0);
    image_p->color             = glm::vec4( 1.0,  1.0,  1.0,  1.0);
    image_p->is_visable        = false;

    // Image
    image_p->cbmp_id           = 0;
    image_p->texture_coords[0] = glm::vec2( 0.0,  0.0);
    image_p->texture_coords[1] = glm::vec2( 1.0,  1.0);

    // Software Info
    image_p->environment_r  = this;

    image_p->internal.color             = image_p->color;
    image_p->internal.positions[0]      = image_p->positions[0];
    image_p->internal.positions[1]      = image_p->positions[1];
    image_p->internal.texture_coords[0] = image_p->texture_coords[0];
    image_p->internal.texture_coords[1] = image_p->texture_coords[1];
    image_p->internal.is_visable        = image_p->is_visable;

    image_p->internal.cbmp_index        = 0;

    return image_p;
}

Image::~Image() {
    this->environment_r->images.erase( this );
}

void Image::update() {
    this->internal.color             = this->color;
    this->internal.positions[0]      = this->positions[0];
    this->internal.positions[1]      = this->positions[1];
    this->internal.texture_coords[0] = this->texture_coords[0];
    this->internal.texture_coords[1] = this->texture_coords[1];
    this->internal.is_visable        = this->is_visable;

    this->internal.cbmp_index = 0;

    if(this->environment_r == nullptr)
        return;

    for(auto it = this->environment_r->textures.cbegin(); it != this->environment_r->textures.cend(); it++) {
        if((*it).resource_id == this->cbmp_id) {
            this->internal.cbmp_index = it - this->environment_r->textures.begin();
            break;
        }
    }

    this->environment_r->images.insert( this );
}

}
