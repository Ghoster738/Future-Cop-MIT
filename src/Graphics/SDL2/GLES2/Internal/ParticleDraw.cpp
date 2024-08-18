#include "ParticleDraw.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {
namespace Internal {

ParticleDraw::ParticleDraw() : particle_atlas_id(0), scale() {}

ParticleDraw::~ParticleDraw() {}

const Data::Mission::PYRResource::AtlasParticle * ParticleDraw::containsParticle( uint32_t pyr_identifier ) const {
    for(const auto &particle : altas_particles) {
        if(particle.getID() == pyr_identifier)
            return &particle;
    }

    return nullptr;
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

    particle_instances.clear();

    return 1;
}

void ParticleDraw::draw(Graphics::SDL2::GLES2::Camera& camera) {
    const uint8_t QUAD_TABLE[2][3] = { {3, 2, 1}, {1, 0, 3}};

    DynamicTriangleDraw::Triangle *draw_triangles_r;
    glm::mat4 camera_3D_model_transform = glm::mat4(1.0f);
    glm::mat4 view;

    camera.getView3D( view );

    const auto camera_position = camera.getPosition();
    const auto camera_right = glm::vec3(view[0][0], view[1][0], view[2][0]);
    const auto camera_up    = glm::vec3(view[0][1], view[1][1], view[2][1]);


    for(const auto &particle : particle_instances) {
        const ParticleInstanceData &instance_data = particle.second;

        if(instance_data.particle_r == nullptr)
            continue;

        const auto number_of_triangles = camera.transparent_triangles.getTriangles( 2, &draw_triangles_r );

        if(number_of_triangles == 0)
            break;

        const Data::Mission::PYRResource::AtlasParticle &altas_particle = *instance_data.particle_r;
        const Data::Mission::PYRResource::AtlasParticle::Texture &current_texture = altas_particle.getTextures().at(instance_data.image_index);

        glm::vec2 l = glm::vec2(current_texture.location) * scale;
        glm::vec2 u = l + glm::vec2(current_texture.size) * scale;

        glm::vec2 coords[4] = { {l.x, l.y}, {u.x, l.y}, {u.x, u.y}, {l.x, u.y} };

        glm::vec2 ql(-0.5);
        glm::vec2 qu = (glm::vec2(current_texture.size) / glm::vec2(altas_particle.getSpriteSize())) - ql;

        ql += (glm::vec2(current_texture.offset_from_size) / glm::vec2(altas_particle.getSpriteSize()));
        qu += (glm::vec2(current_texture.offset_from_size) / glm::vec2(altas_particle.getSpriteSize()));

        const glm::vec2 QUAD[4] = {{ql.x, qu.y}, {qu.x, qu.y}, {qu.x, ql.y}, {ql.x, ql.y}};

        size_t index = 0;

        for(int x = 0; x < 3; x++) {
            draw_triangles_r[ index ].vertices[x].position   = instance_data.position;
            draw_triangles_r[ index ].vertices[x].normal     = glm::vec3(0, 1, 0);
            draw_triangles_r[ index ].vertices[x].color      = instance_data.color;
            draw_triangles_r[ index ].vertices[x].vertex_metadata = glm::i16vec2(0, 0);
        }

        draw_triangles_r[ index ].setup( this->particle_atlas_id, camera_position, DynamicTriangleDraw::PolygonType::MIX );
        draw_triangles_r[ index ] = draw_triangles_r[ index ].addTriangle( camera_position, camera_3D_model_transform );

        draw_triangles_r[ index + 1 ] = draw_triangles_r[ index ];

        for(int t = 0; t < 2; t++) {
            for(int x = 0; x < 3; x++) {
                draw_triangles_r[ index ].vertices[x].position += (camera_right * QUAD[QUAD_TABLE[t][x]].x) + (camera_up * QUAD[QUAD_TABLE[t][x]].y);

                draw_triangles_r[ index ].vertices[x].coordinate = coords[QUAD_TABLE[t][x]];
            }
            index++; index = std::min(number_of_triangles - 1, index);
        }
    }
}

void ParticleDraw::updateInstanceData(const ParticleInstance *const particle_instance_r, const ParticleInstanceData& particle_instance_data) {
    particle_instances[particle_instance_r] = particle_instance_data;
}

void ParticleDraw::removeInstanceData(const ParticleInstance *const particle_instance_r) {
    auto search = particle_instances.find( particle_instance_r );

    if(search != particle_instances.end())
        particle_instances.erase(search);
}

}
}
}
}
