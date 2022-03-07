#ifndef GRAPHICS_MODEL_INSTANCE_H
#define GRAPHICS_MODEL_INSTANCE_H

#include "ElementInternalData.h"
#include "../Utilities/DataTypes.h"

namespace Graphics {

class ModelInstance : public ElementInternalData {
protected:
    void *ModelInternalData_data;

    // These units are for the 3D environment.
    Utilities::DataTypes::Vec3 position;
    Utilities::DataTypes::Vec4 rotation; // This value is a quaterion.
    Utilities::DataTypes::Vec2 texture_offset;
    float timeline;
public:
    /**
     * This declared a ModelInstance.
     * @warning Be sure to attach this to the Environment before deletion.
     */
    ModelInstance();

    /**
     * This method is to be called only in Environment, because this class is responsiable for handling the position and rotation of the model.
     * @param position the 3D position of the model.
     * @param rotation the rotation of the model. Warning, this is a quaterion, and it should be manually normalized.
     * @param texture_offset This is the texture offset. It is used to change the "color" of the models.
     */
    ModelInstance( const Utilities::DataTypes::Vec3 &position,
                   const Utilities::DataTypes::Vec4 &rotation = Utilities::DataTypes::Vec4( 1, 0, 0, 0 ),
                   const Utilities::DataTypes::Vec2 &texture_offset = Utilities::DataTypes::Vec2( 0, 0 ) );

    /**
     * @warning Be sure that this is attached to an environement before deletion.
     */
    virtual ~ModelInstance();

    /**
     * This sets the position of the model.
     * @param position the 3D position of the model.
     */
    void setPosition( const Utilities::DataTypes::Vec3 &position );

    /**
     * This sets the  the rotation of the model.
     * @param rotation the rotation of the model. Warning this is a quaterion, and it should be manually normalized.
     */
    void setRotation( const Utilities::DataTypes::Vec4 &rotation );

    /**
     * This sets the offset of the texture.
     * @param texture_offset the rotation of the model.
     */
    void setTextureOffset( const Utilities::DataTypes::Vec2 &texture_offset );

    /**
     * This sets the time of the instance.
     * @param timeline set the timeline of the instance.
     */
    void setTimeline( float timeline ) { this->timeline = timeline; }

    /**
     * This gets the position of the model.
     * @return position the 3D position of the model to be overwritten.
     */
    Utilities::DataTypes::Vec3 getPosition() const;

    /**
     * This gets the the rotation of the model.
     * @return rotation the rotation of the model to be overwritten.
     */
    Utilities::DataTypes::Vec4 getRotation() const;

    /**
     * This gets the 2D offset of the texture.
     * @return texture_offset the offset to the texture.
     */
    Utilities::DataTypes::Vec2 getTextureOffset() const;

    float getTimeline() const { return timeline; }

    /**
     * This gets the Graphics api variables for use in the internal code for the Environment.
     */
    void* getInternalData() { return ModelInternalData_data; }
};

}

#endif // GRAPHICS_MODEL_INSTANCE_H
