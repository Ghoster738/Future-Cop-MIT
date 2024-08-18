#ifndef GRAPHICS_SDL2_GLES2_PARTICLE_INSTANCE_H
#define GRAPHICS_SDL2_GLES2_PARTICLE_INSTANCE_H

#include "../../ParticleInstance.h"

#include "Environment.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

class ParticleInstance : public Graphics::ParticleInstance {
public:
    Environment *environment_r;

    virtual ~ParticleInstance();

    virtual void setParticleID( uint16_t particle_id );
    virtual uint16_t getParticleID() const;

    virtual bool setParticleIndex( uint16_t index );
    virtual uint16_t getParticleIndex() const;

    virtual void update();
};

}
}
}

#endif
