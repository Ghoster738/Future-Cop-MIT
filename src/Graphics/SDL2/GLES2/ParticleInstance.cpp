#include "ParticleInstance.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

Graphics::ParticleInstance* Environment::allocateParticleInstance(uint16_t particle_id, uint16_t index) {}

ParticleInstance::~ParticleInstance() {}

void ParticleInstance::setPosition( const glm::vec3 &position ) {}
glm::vec3 ParticleInstance::getPosition() const {}

void ParticleInstance::setColor( const glm::vec4 &color ) {}
glm::vec4 ParticleInstance::getColor() const {}

void ParticleInstance::setSpan( float span ) {}
float ParticleInstance::getSpan() const {}

void ParticleInstance::setParticleID( uint16_t particle_id ) {}
uint16_t ParticleInstance::getParticleID() const {}

bool ParticleInstance::setParticleIndex( uint16_t index ) {}
uint16_t ParticleInstance::getParticleIndex() const {}

}
}
}
