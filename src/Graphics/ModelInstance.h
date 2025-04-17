#ifndef GRAPHICS_MODEL_INSTANCE_H
#define GRAPHICS_MODEL_INSTANCE_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Graphics {

class ModelInstance {
protected:
    // These units are for the 3D environment.
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation; // This value is a quaternion.
    glm::vec2 texture_offset;
    glm::vec3 color;
    
    float position_transform_timeline;
    unsigned int texture_transform_timeline;
    float texture_transform_remainder;

    bool visable;
    
    ModelInstance() :
            position( glm::vec3(0, 0, 0) ),
            scale( glm::vec3(1, 1, 1) ),
            rotation( glm::quat( glm::vec3(0, 0, 0) ) ),
            texture_offset( glm::vec2(0 ,0 ) ),
            color( glm::vec3(1, 1, 1) ),
            position_transform_timeline( 0.0f ),
            texture_transform_timeline( 0 ),
            texture_transform_remainder( 0 ),
            visable( false ) {}

public:
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
     * This sets the scale of the model.
     * @param scale the scale of the model.
     */
    virtual void setScale( const glm::vec3 &scale );

    /**
     * This sets the offset of the texture.
     * @param texture_offset the rotation of the model.
     */
    virtual void setTextureOffset( const glm::vec2 &texture_offset );

    virtual void setColor( const glm::vec3 &color );

    /**
     * This sets the time of the instance.
     * @warning This function sets the parameter position_transform_timeline.
     * @param position_transform_timeline set the timeline of the instance.
     */
    virtual void setPositionTransformTimeline( float position_transform_timeline ) = 0;

    /**
     * This sets the time of the instance.
     * @param texture_transform_timeline set the timeline of the instance.
     */
    virtual void setTextureTransformTimeline( unsigned int texture_transform_timeline ) { this->texture_transform_timeline = texture_transform_timeline; }

    /**
     * This sets the time of the instance.
     * @param texture_transform_timeline set the timeline of the instance.
     */
    virtual void addTextureTransformTimelineSeconds( float texture_transform_timeline );

    void setVisable(bool visable) { this->visable = visable; }

    /**
     * This gets the position of the model.
     * @return the 3D position of the model to be overwritten.
     */
    glm::vec3 getPosition() const;

    /**
     * This gets the scale of the model.
     * @return scale the scale of the model.
     */
    glm::vec3 getScale() const;

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

    glm::vec3 getColor() const;

    bool getVisable() const { return this->visable; }

    float getPositionTransformTimeline() const { return position_transform_timeline; }
    unsigned int getTextureTransformTimeline() const { return texture_transform_timeline; }
    
    /**
     * Get the regular bounding sphere of the model instance.
     */
    virtual bool getBoundingSphere( glm::vec3 &position, float &radius ) const = 0;
};

}

#endif // GRAPHICS_MODEL_INSTANCE_H
