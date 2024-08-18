#include "ParticleInstance.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

Graphics::ParticleInstance* Environment::allocateParticleInstance(uint16_t particle_id, uint16_t index) {}

ParticleInstance::~ParticleInstance() {}

void ParticleInstance::setParticleID( uint16_t particle_id ) {}
uint16_t ParticleInstance::getParticleID() const {}

bool ParticleInstance::setParticleIndex( uint16_t index ) {}
uint16_t ParticleInstance::getParticleIndex() const {}

void ParticleInstance::update() {}

}
}
}
