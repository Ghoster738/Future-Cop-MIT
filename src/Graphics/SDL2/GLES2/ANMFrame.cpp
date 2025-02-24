#include "ANMFrame.h"

#include "../../../Data/Mission/ANMResource.h"

namespace {
const Utilities::PixelFormatColor_R8G8B8 RGB_COLOR_FORMAT;
}

namespace Graphics::SDL2::GLES2 {

Graphics::ANMFrame* Environment::allocateVideoANM(uint32_t track_offset) {
    auto search = this->anm_resources.find( track_offset );

    if(search == this->anm_resources.end())
        return NULL;

    auto *image_p = new Graphics::SDL2::GLES2::ANMFrame(*search->second, RGB_COLOR_FORMAT);

    // ImageBase
    image_p->positions[0]      = glm::vec2(-1.0, -1.0);
    image_p->positions[1]      = glm::vec2( 1.0,  1.0);
    image_p->color             = glm::vec4( 1.0,  1.0,  1.0,  1.0);
    image_p->is_visable        = false;

    // External Image
    image_p->image_2d.setDimensions(Data::Mission::ANMResource::Video::WIDTH, Data::Mission::ANMResource::Video::HEIGHT);

    image_p->video.gotoFrame(0);
    const Utilities::ImagePalette2D *image_r = image_p->video.getImage();
    image_p->image_2d.inscribeSubImage( 0, 0, *image_r );
    image_p->total_frames = search->second->getTotalFrames();

    // OpenGL Info
    image_p->environment_r = this;

    return image_p;
}

ANMFrame::ANMFrame(const Data::Mission::ANMResource &resource, const Utilities::PixelFormatColor &color) : Graphics::ANMFrame(color), video(&resource) {}

ANMFrame::~ANMFrame() {
    this->environment_r->draw_2d_routine.removeDynamicImageData(this);
}

bool ANMFrame::nextFrame() {
    bool status = video.nextFrame();
    video.setImage(this->image_2d);
    upload();

    return status;
}

bool ANMFrame::setFrameIndex(unsigned index) {
    bool status = video.gotoFrame(index);
    video.setImage(this->image_2d);
    upload();

    return status;
}

unsigned ANMFrame::getFrameAmount() const {
    return this->total_frames;
}

void ANMFrame::update() {
    Internal::Draw2D::DynamicImageData dynamic_image_data;

    dynamic_image_data.positions[0] = this->positions[0];
    dynamic_image_data.positions[1] = this->positions[1];
    dynamic_image_data.color        = this->color;
    dynamic_image_data.visable      = this->is_visable;

    this->environment_r->draw_2d_routine.updateDynamicImageData(this, dynamic_image_data);
}

void ANMFrame::upload() {
    this->environment_r->draw_2d_routine.uploadDynamicImageData(this, this->image_2d, GL_RGB);
}
}
