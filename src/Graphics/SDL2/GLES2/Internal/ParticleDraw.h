#ifndef GRAPHICS_GLES2_INTERNAL_PARTICLE_DRAW_H
#define GRAPHICS_GLES2_INTERNAL_PARTICLE_DRAW_H

#include "../../../../Data/Mission/PYRResource.h"
#include "../Camera.h"

#include <cstdint>
#include <vector>

namespace Graphics {
namespace SDL2 {
namespace GLES2 {
namespace Internal {

class ParticleDraw {
private:
    uint32_t particle_atlas_id;
    std::vector<Data::Mission::PYRResource::AtlasParticle> altas_particles;

public:
    ParticleDraw();
    virtual ~ParticleDraw();

    /**
     * This checks for the existence of the particle inside this draw routine.
     * @param pyr_identifier The id of the pyr to find.
     * @return True if the particle exists in the class.
     */
    bool containsParticle( uint32_t pyr_identifier ) const;

    /**
     * This loads up the particle data.
     * @param particle_data The "particle" data from either the global resource from the PC versions or the mission file from the PS1 version.
     * @param textures This is the texture storage, the same one used for dynamic triangle renderer. @warning If this method successful this parameter will gain another texture used by this resource.
     * @return 1 for success less than zero for error.
     */
    int inputParticles(const Data::Mission::PYRResource& particle_data, std::map<uint32_t, Internal::Texture2D*>& textures);

    /**
     * Draw all the particles that are being rendered.
     * @param camera This parameter will get the particles that would be rendered with this method.
     */
    void draw(Graphics::SDL2::GLES2::Camera& camera);
};

}
}
}
}

#endif // GRAPHICS_GLES2_INTERNAL_PARTICLE_DRAW_H
