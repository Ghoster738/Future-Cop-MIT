#include "ExternalImage.h"

#include "Environment.h"

namespace Graphics::SDL2::Software {

Graphics::ExternalImage* Environment::allocateExternalImage(bool has_alpha) {
    Software::ExternalImage *image_p;

    if(has_alpha) {
        image_p = new Graphics::SDL2::Software::ExternalImage(Utilities::PixelFormatColor_R8G8B8A8::linear);
        image_p->has_alpha_channel = true;
        this->external_image_draw_2d.images.insert( image_p );
    }
    else {
        image_p = new Graphics::SDL2::Software::ExternalImage(Utilities::PixelFormatColor_R8G8B8::linear);
        image_p->has_alpha_channel = false;
        this->external_image_draw_2d.opaque_images.insert( image_p );
    }

    // ImageBase
    image_p->color        = glm::vec4( 1.0,  1.0,  1.0,  1.0);
    image_p->positions[0] = glm::vec2(  0.0,   0.0);
    image_p->positions[1] = glm::vec2( 16.0,  16.0);
    image_p->is_visable   = false;

    image_p->internal.color        = image_p->color;
    image_p->internal.positions[0] = image_p->positions[0];
    image_p->internal.positions[1] = image_p->positions[1];
    image_p->internal.is_visable   = image_p->is_visable;

    // OpenGL Info
    image_p->environment_r = this;

    return image_p;
}

ExternalImage::ExternalImage(const Utilities::PixelFormatColor &color) : Graphics::ExternalImage(color) {}

ExternalImage::~ExternalImage() {}

void ExternalImage::update() {
    this->internal.color        = this->color;
    this->internal.positions[0] = this->positions[0];
    this->internal.positions[1] = this->positions[1];
    this->internal.is_visable   = this->is_visable;

    if(!this->has_alpha_channel) {
        if(this->color.a != this->internal.color.a) {
            if(this->color.a < 1.0f) {
                this->environment_r->external_image_draw_2d.opaque_images.erase( this );
                this->environment_r->external_image_draw_2d.images.insert( this );
            }
            else {
                this->environment_r->external_image_draw_2d.images.erase( this );
                this->environment_r->external_image_draw_2d.opaque_images.insert( this );
            }
        }
    }
    else
        this->environment_r->external_image_draw_2d.images.insert( this );
}

void ExternalImage::upload() {}

}
