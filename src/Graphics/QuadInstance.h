#ifndef GRAPHICS_QUAD_INSTANCE_H
#define GRAPHICS_QUAD_INSTANCE_H

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Defines.h"

#include <cstdint>

namespace Graphics {

class QuadInstance {
public:
    glm::quat rotation;
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 span;
    RenderMode render_mode;

    virtual ~QuadInstance() {};

    /**
     * This sets the dcs quad id of the graphic that this quad uses.
     * @param dcs_id the new id for the quad.
     */
    virtual void setQuadID( uint8_t dcs_id ) = 0;

    /**
     * This gets the dcs id of the graphic that this quad uses.
     * @return the id for the quad.
     */
    virtual uint8_t getQuadID() const = 0;

    /**
     * This updates the quad to the renderer.
     * @warning Calling this is required for the quad to fully display.
     */
    virtual void update() = 0;
};

}

#endif // GRAPHICS_QUAD_INSTANCE_H
