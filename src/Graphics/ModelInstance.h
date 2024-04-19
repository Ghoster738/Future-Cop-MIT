#ifndef GRAPHICS_SDL2_GLES2_MODEL_INSTANCE_H
#define GRAPHICS_SDL2_GLES2_MODEL_INSTANCE_H

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
    
    float position_transform_timeline;
    unsigned int texture_transform_timeline;
    
    ModelInstance(
        const glm::vec3 &pos,
        const glm::quat &rot,
        const glm::vec2 &offset) :
            position( pos ),
            rotation( rot ),
            texture_offset( offset ),
            position_transform_timeline( 0.0f ),
            texture_transform_timeline( 0 ) {}
public:
    /**
     * This method is to be called only in Environment, because this class is responsiable for handling the position and rotation of the model.
     * @param env_r this is the environment that will be attached to this model instance.
     * @param obj_identifier this is the index to the object.
     * @param position the 3D position of the model.
     * @param rotation the rotation of the model. Warning, this is a quaterion, and it should be manually normalized.
     * @param texture_offset This is the texture offset. It is used to change the "color" of the models.
     * @return a valid pointer to model instance.
     */
    static Graphics::ModelInstance* alloc( Environment &env_r,
                                           uint32_t obj_identifier,
                                           const glm::vec3 &position,
                                           const glm::quat &rotation = glm::quat(),
                                           const glm::vec2 &texture_offset = glm::vec2( 0, 0 ) );

    /**
     * @warning Be sure that this is attached to an environement before deletion.
     */
    virtual ~ModelInstance();
    
    static bool exists( Graphics::Environment &env_r, uint32_t obj_identifier );

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
     * @param position_transform_timeline set the timeline of the instance.
     */
    virtual void setPositionTransformTimeline( float position_transform_timeline ) { this->position_transform_timeline = position_transform_timeline; }

    /**
     * This sets the time of the instance.
     * @param texture_transform_timeline set the timeline of the instance.
     */
    virtual void setTextureTransformTimeline( unsigned int texture_transform_timeline ) { this->texture_transform_timeline = texture_transform_timeline; }

    /**
     * This sets the time of the instance.
     * @param texture_transform_timeline set the timeline of the instance.
     */
    virtual void setTextureTransformTimelineSeconds( float texture_transform_timeline );

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

    float getPositionTransformTimeline() const { return position_transform_timeline; }
    unsigned int getTextureTransformTimeline() const { return texture_transform_timeline; }
    float getTextureTransformTimelineSeconds() const;
    
    /**
     * Get the regular bounding sphere of the model instance.
     */
    virtual bool getBoundingSphere( glm::vec3 &position, float &radius ) const = 0;
};

}

#endif // GRAPHICS_SDL2_GLES2_MODEL_INSTANCE_H
