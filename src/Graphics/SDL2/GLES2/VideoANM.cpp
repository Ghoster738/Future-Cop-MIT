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
    image_p->image_index = 0;
    image_p->factor = 0.0;
    image_p->seconds_to_next_frame = image_p->factor;

    return image_p;
}

VideoANM::~VideoANM() {
    //if(this->texture_2d_p != nullptr)
        //delete this->texture_2d_p;

    //this->environment_r->draw_2d_routine.removeExternalImageData(this);
}

void VideoANM::play() {
    this->factor = 0.125;
    update();
}

void VideoANM::stop() {
    this->image_index = 0;
    this->seconds_to_next_frame = this->factor;
    this->factor = 0.0;
    update();
}

void VideoANM::pause() {
    this->factor = 0.0;
    update();
}

void VideoANM::update() {
    Internal::Draw2D::DynamicImageData dynamic_image_data;

    dynamic_image_data.positions[0] = this->positions[0];
    dynamic_image_data.positions[1] = this->positions[1];
    dynamic_image_data.color        = this->color;
    dynamic_image_data.visable      = this->is_visable;

    //this->environment_r->draw_2d_routine.updateDynamicImageData(this, dynamic_image_data);
}

}
