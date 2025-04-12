#include "QuadInstance.h"

namespace Graphics::SDL2::GLES2 {

Graphics::QuadInstance* Environment::allocateQuadInstance() {
    auto quadrilateral_p = new Graphics::SDL2::GLES2::QuadInstance();

    quadrilateral_p->environment_r = this;
    quadrilateral_p->texture_info_r = nullptr;

    return quadrilateral_p;
}

QuadInstance::~QuadInstance() {
    //this->environment_r->particle_draw_routine.removeQuadData(this);
}

void QuadInstance::setQuadID( uint8_t dcs_id ) {
    auto new_texture_info_r = nullptr;
    //auto new_texture_info_r = this->environment_r->particle_draw_routine.containsDCSImage( dcs_id );

    if(new_texture_info_r == nullptr && dcs_id != 0)
        return;

    this->texture_info_r = new_texture_info_r;
}

uint8_t QuadInstance::getQuadID() const {
    if(this->texture_info_r == nullptr)
        return 0;

    return this->texture_info_r->id;
}

void QuadInstance::update() {
}

}
