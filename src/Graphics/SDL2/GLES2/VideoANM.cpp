#include "VideoANM.h"

namespace {
const Utilities::PixelFormatColor_R8G8B8A8 RGB_ALPHA_COLOR_FORMAT;
}

namespace Graphics::SDL2::GLES2 {

Graphics::VideoANM* Environment::allocateVideoANM(uint32_t track_offset) {
    auto *image_p = new Graphics::SDL2::GLES2::VideoANM();

    // ImageBase
    image_p->positions[0]      = glm::vec2(-1.0, -1.0);
    image_p->positions[1]      = glm::vec2( 1.0,  1.0);
    image_p->color             = glm::vec4( 1.0,  1.0,  1.0,  1.0);
    image_p->is_visable        = false;

    // OpenGL Info
    image_p->environment_r = this;

    return image_p;
}

VideoANM::~VideoANM() {
    //if(this->texture_2d_p != nullptr)
        //delete this->texture_2d_p;

    //this->environment_r->draw_2d_routine.removeExternalImageData(this);
}

void VideoANM::repeat() {}
void VideoANM::play(){}
void VideoANM::stop() {}
void VideoANM::pause() {}

void VideoANM::update() {
    Internal::Draw2D::ExternalImageData external_image_data;

    external_image_data.positions[0] = this->positions[0];
    external_image_data.positions[1] = this->positions[1];
    external_image_data.color        = this->color;
    external_image_data.visable      = this->is_visable;

    //this->environment_r->draw_2d_routine.updateExternalImageData(this, external_image_data);
}

}
