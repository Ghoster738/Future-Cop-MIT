#ifndef GRAPHICS_PARTICLE_INSTANCE_H
#define GRAPHICS_PARTICLE_INSTANCE_H

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cstdint>

namespace Graphics {

class ParticleInstance {
public:
    glm::vec3 position;
    glm::vec4 color;
    float span;
    bool is_addition;

    virtual ~ParticleInstance() = 0;

    /**
     * This sets the particle id of the graphic that this particle uses.
     * @param particle_id the new id for the particle.
     */
    virtual void setParticleID( uint16_t particle_id ) = 0;

    /**
     * This gets the particle id of the graphic that this particle uses.
     * @return the id for the particle.
     */
    virtual uint16_t getParticleID() const = 0;

    /**
     * This sets the particle index of the graphic that this particle uses.
     * @warning This method has no effect if the given index is out of bounds.
     * @param index the index for the particle.
     * @return true if the index could be successfully set.
     */
    virtual bool setParticleIndex( uint16_t index ) = 0;

    /**
     * This gets the particle index of the graphic that this particle uses.
     * @return the index of the particle.
     */
    virtual uint16_t getParticleIndex() const = 0;

    /**
     *
     * @warning Calling this is required for the particle to fully display.
     */
    virtual void update() = 0;
};

}

#endif
