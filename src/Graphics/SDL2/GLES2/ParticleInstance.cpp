#include "ParticleInstance.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

Graphics::ParticleInstance* Environment::allocateParticleInstance() {
    auto particle_p = new Graphics::SDL2::GLES2::ParticleInstance();

    particle_p->environment_r = this;
    particle_p->particle_r = nullptr;

    return particle_p;
}

ParticleInstance::~ParticleInstance() {
    this->environment_r->particle_draw_routine.removeInstanceData(this);
}

void ParticleInstance::setParticleID( uint16_t particle_id ) {
    auto new_particle_r = this->environment_r->particle_draw_routine.containsParticle( particle_id );

    if(new_particle_r == nullptr && particle_id != 0)
        return;

    this->particle_r = new_particle_r;
    this->particle_index = 0;
}

uint16_t ParticleInstance::getParticleID() const {
    if(this->particle_r == nullptr)
        return 0;

    return this->particle_r->getID();
}

bool ParticleInstance::setParticleIndex( uint16_t index ) {
    if(this->particle_r == nullptr)
        return false;

    if(this->particle_r->getTextures().size() < index)
        return false;

    this->particle_index = index;

    return true;
}

uint16_t ParticleInstance::getParticleIndex() const {
    return this->particle_index;
}

void ParticleInstance::update() {
    Internal::ParticleDraw::ParticleInstanceData particle_instance_data;

    particle_instance_data.position    = this->position;
    particle_instance_data.color       = this->color;
    particle_instance_data.span        = this->span;
    particle_instance_data.is_addition = this->is_addition;
    particle_instance_data.particle_r  = this->particle_r;
    particle_instance_data.image_index = this->particle_index;

    this->environment_r->particle_draw_routine.updateInstanceData(this, particle_instance_data);
}

}
}
}
