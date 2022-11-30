#ifndef GRAPHICS_MODEL_INSTANCE_H
#define GRAPHICS_MODEL_INSTANCE_H

#include "Environment.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Graphics {

class ModelInstance {
protected:
    // These units are for the 3D environment.
    glm::vec3 position;
    glm::quat rotation; // This value is a quaterion.
    glm::vec2 texture_offset;
    
    float timeline;
    
    ModelInstance(
        const glm::vec3 &pos,
        const glm::quat &rot,
        const glm::vec2 &offset) :
            position( pos ),
            rotation( rot ),
            texture_offset( offset ),
            timeline( 0.0f ) {}
public:
    /**
     * This method is to be called only in Environment, because this class is responsiable for handling the position and rotation of the model.
     * @param env_r this is the environment that will be attached to this model instance.
     * @param index_obj this is the index to the object.
     * @param position the 3D position of the model.
     * @param rotation the rotation of the model. Warning, this is a quaterion, and it should be manually normalized.
     * @param texture_offset This is the texture offset. It is used to change the "color" of the models.
     * @return a valid pointer to model instance.
     */
    static Graphics::ModelInstance* alloc( Environment &env_r,
                                           int index_obj,
                                           const glm::vec3 &position,
                                           const glm::quat &rotation = glm::quat(),
                                           const glm::vec2 &texture_offset = glm::vec2( 0, 0 ) );

    /**
     * @warning Be sure that this is attached to an environement before deletion.
     */
    virtual ~ModelInstance();

    /**
     * This sets the position of the model.
     * @param position the 3D position of the model.
     */
    virtual void setPosition( const glm::vec3 &position );

    /**
     * This sets the  the rotation of the model.
     * @param rotation the rotation of the model. Warning this is a quaterion, and it should be manually normalized.
     */
    virtual void setRotation( const glm::quat &rotation );

    /**
     * This sets the offset of the texture.
     * @param texture_offset the rotation of the model.
     */
    virtual void setTextureOffset( const glm::vec2 &texture_offset );

    /**
     * This sets the time of the instance.
     * @param timeline set the timeline of the instance.
     */
    virtual void setTimeline( float timeline ) { this->timeline = timeline; }

    /**
     * This gets the position of the model.
     * @return position the 3D position of the model to be overwritten.
     */
    glm::vec3 getPosition() const;

    /**
     * This gets the the rotation of the model.
     * @return rotation the rotation of the model to be overwritten.
     */
    glm::quat getRotation() const;

    /**
     * This gets the 2D offset of the texture.
     * @return texture_offset the offset to the texture.
     */
    glm::vec2 getTextureOffset() const;
    
    /**
     * Get the regular bounding sphere of the model instance.
     */
    virtual bool getBoundingSphere( glm::vec3 &position, float &radius ) const = 0;

    float getTimeline() const { return timeline; }
};

}

#endif // GRAPHICS_MODEL_INSTANCE_H
