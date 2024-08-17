#include "ParticleDraw.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {
namespace Internal {

ParticleDraw::ParticleDraw() : particle_atlas_id(0), scale() {}

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

    this->scale = glm::vec2(1. / image_p->getWidth(), 1. / image_p->getHeight());

    textures[ this->particle_atlas_id ] = new SDL2::GLES2::Internal::Texture2D;
    textures[ this->particle_atlas_id ]->setFilters( 0, GL_NEAREST, GL_LINEAR );
    textures[ this->particle_atlas_id ]->setImage( 0, 0, GL_RGBA, image_p->getWidth(), image_p->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image_p->getDirectGridData() );

    delete image_p;

    return 1;
}

void ParticleDraw::draw(Graphics::SDL2::GLES2::Camera& camera) {
    DynamicTriangleDraw::Triangle *draw_triangles_r;
    glm::mat4 camera_3D_model_transform = glm::mat4(1.0f);
    glm::mat4 view;

    camera.getView3D( view );

    const auto camera_position = camera.getPosition();
    const auto camera_right = glm::vec3(view[0][0], view[1][0], view[2][0]);
    const auto camera_up    = glm::vec3(view[0][1], view[1][1], view[2][1]);

    float x = 0;

    for(const auto &particle : altas_particles) {
        const auto number_of_triangles = camera.transparent_triangles.getTriangles( 2, &draw_triangles_r );

        glm::vec2 l = glm::vec2(particle.getTextures()[0].location) * scale;
        glm::vec2 u = l + glm::vec2(particle.getTextures()[0].size) * scale;

        glm::vec2 quad[4] = { {l.x, l.y}, {u.x, l.y}, {u.x, u.y}, {l.x, u.y} };

        DynamicTriangleDraw::Triangle::addBillboard(
            draw_triangles_r, number_of_triangles,
            camera_position, camera_3D_model_transform, camera_right, camera_up,
            {x, 3, 0}, {0.5,0.5,0.5}, 2.0f * particle.getTextures()[0].size.x / particle.getSpriteSize(),
            DynamicTriangleDraw::PolygonType::ADDITION, particle_atlas_id, quad
        );

        x += 5;
    }
}

}
}
}
}
