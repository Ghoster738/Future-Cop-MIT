#include "QuadInstance.h"

namespace Graphics::SDL2::GLES2 {

Graphics::QuadInstance* Environment::allocateQuadInstance() {
    auto quadrilateral_p = new Graphics::SDL2::GLES2::QuadInstance();

    quadrilateral_p->environment_r = this;
    quadrilateral_p->texture_info_r = nullptr;

    return quadrilateral_p;
}

QuadInstance::~QuadInstance() {
    this->environment_r->particle_draw_routine.removeQuadData(this);
}

void QuadInstance::setQuadID( uint8_t dcs_id ) {
    auto new_texture_info_r = this->environment_r->particle_draw_routine.containsDCSImage( dcs_id );

    if(new_texture_info_r == nullptr)
        return;

    this->texture_info_r = new_texture_info_r;
}

uint8_t QuadInstance::getQuadID() const {
    if(this->texture_info_r == nullptr)
        return 0;

    return this->texture_info_r->id;
}

void QuadInstance::update() {
    Internal::ParticleDraw::QuadInstanceData instance_data;

    float span = glm::max(this->span.x, this->span.y);

    instance_data.rotation    = this->rotation;
    instance_data.position    = this->position;
    instance_data.color       = this->color;
    instance_data.image_r     = this->texture_info_r;
    instance_data.min         = { this->position.x - span, this->position.z - span };
    instance_data.max         = { this->position.x + span, this->position.z + span };
    instance_data.span        = this->span;
    instance_data.is_opaque   = this->is_opaque;
    instance_data.is_addition = this->is_addition;

    this->environment_r->particle_draw_routine.updateQuadData(this, instance_data);
}

}
