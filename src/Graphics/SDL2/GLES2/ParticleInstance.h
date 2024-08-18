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

    virtual void setPosition( const glm::vec3 &position );
    virtual glm::vec3 getPosition() const;

    virtual void setColor( const glm::vec4 &color );
    virtual glm::vec4 getColor() const;

    virtual void setSpan( float span );
    virtual float getSpan() const;

    virtual void setParticleID( uint16_t particle_id );
    virtual uint16_t getParticleID() const = 0;

    virtual bool setParticleIndex( uint16_t index );
    virtual uint16_t getParticleIndex() const;
};

}
}
}

#endif
