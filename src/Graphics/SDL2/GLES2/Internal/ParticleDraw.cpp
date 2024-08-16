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

int ParticleDraw::inputParticles(const Data::Mission::PYRResource& particle_data, std::map<uint32_t, Internal::Texture2D*>& textures) {
    Utilities::Image2D *image_p = particle_data.generatePalettlessAtlas(this->altas_particles);

    if(image_p == nullptr)
        return 0;

    this->particle_atlas_id = 0;

    for(uint32_t i = 1; i < 0x1000; i++) {
        if(textures.find(i) == textures.end()) {
            this->particle_atlas_id = i;
            break;
        }
    }

    if(this->particle_atlas_id == 0) {
        this->altas_particles.clear();
        return -1;
    }

    textures[ this->particle_atlas_id ] = new SDL2::GLES2::Internal::Texture2D;
    textures[ this->particle_atlas_id ]->setFilters( 0, GL_NEAREST, GL_LINEAR );
    textures[ this->particle_atlas_id ]->setImage( 0, 0, GL_RGBA, image_p->getWidth(), image_p->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image_p->getDirectGridData() );

    delete image_p;

    return 1;
}

void ParticleDraw::draw(Graphics::SDL2::GLES2::Camera& camera) {}

}
}
}
}
