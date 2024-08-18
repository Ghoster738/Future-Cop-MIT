#ifndef GRAPHICS_PARTICLE_INSTANCE_H
#define GRAPHICS_PARTICLE_INSTANCE_H

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cstdint>

namespace Graphics {

class ParticleInstance {
public:

    virtual ~ParticleInstance() = 0;

    /**
     * This sets the position of the particle.
     * @param position the 3D position of the particle.
     */
    virtual void setPosition( const glm::vec3 &position ) = 0;

    /**
     * This gets the position of the particle.
     * @return the 3D position of the particle to be overwritten.
     */
    virtual glm::vec3 getPosition() const = 0;

    /**
     * This sets the color of the particle.
     * @param color the 4 channel color value for the particle.
     */
    virtual void setColor( const glm::vec4 &color ) = 0;

    /**
     * This gets the color of the particle.
     * @return the 4 channel color value for the particle.
     */
    virtual glm::vec4 getColor() const = 0;

    /**
     * This sets the span of the particle.
     * @param span the span of the particle.
     */
    virtual void setSpan( float span ) = 0;

    /**
     * This gets the span of the particle.
     * @return the span of the particle.
     */
    virtual float getSpan() const = 0;

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
};

}

#endif
