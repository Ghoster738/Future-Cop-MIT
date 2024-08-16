#include "ParticleDraw.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {
namespace Internal {

ParticleDraw::ParticleDraw() : particle_atlas_id(0) {}

ParticleDraw::~ParticleDraw() {}

bool ParticleDraw::containsParticle( uint32_t pyr_identifier ) const {
    for(const auto &particle : altas_particles) {
        if(particle.getID() == pyr_identifier)
            return true;
    }

    return false;
}

int ParticleDraw::inputParticles(const Data::Mission::PYRResource& particle_data, std::map<uint32_t, Internal::Texture2D*>& textures) {}

void ParticleDraw::draw(Graphics::SDL2::GLES2::Camera& camera) {}

}
}
}
}
