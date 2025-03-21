#ifndef GRAPHICS_CAMERA_INSTANCE_H
#define GRAPHICS_CAMERA_INSTANCE_H

#include "../Utilities/DataTypes.h"
#include "../Utilities/Collision/GJKPolyhedron.h"

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include <vector>

namespace Graphics {

class Camera {
protected:
    // These are for the 3D environment.
    glm::mat4 projection3D;
    glm::mat4 view3D;
    glm::mat4 PV3D;

    // Viewport locations
    glm::u32vec2 origin;
    glm::u32vec2 dimensions; // x = width, y = height

    /**
     * When projection3D or view3D had been changed, the PV3D needs to reflect this.
     * This multiplies projection3D and view3D, and stores the result in PV3D.
     */
    void updatePV3D();

    Camera();

public:
    virtual ~Camera();

    /**
     * This sets the 3d projection, and multiplies it with the 3d view matrix.
     * Use this command only when view 3d stays constant, or is not changed.
     * @param projection3D sets the projection of this camera's 3D mode.
     */
    void setProjection3D( const glm::mat4 &projection3D );

    /**
     * This sets the 3d view, and multiplies it with the 3d projection matrix.
     * Use this command only when Projection 3d stays constant, or is not changed.
     * @param view3D sets the view of this camera's 3D mode.
     */
    void setView3D( const glm::mat4 &view3D );

    /**
     * If both the projection and the view 3D matrices had been changed use this method.
     * @param projection3D sets the projection of this camera's 3D mode.
     * @param view3D sets the view of this camera's 3D mode.
     */
    void setProjectionView3D( const glm::mat4 &projection3D, const glm::mat4 &view3D );

    /**
     * This is to set the viewport origin.
     * @param origin This sets the origin of the viewport.
     */
    void setViewportOrigin( const glm::u32vec2 &origin );

    /**
     * This is to set the viewport width and height.
     * @param dimensions This sets the dimensions of the viewport.
     * @note The dimensions.x is width and dimensions.y is height.
     */
    void setViewportDimensions( const glm::u32vec2 &dimensions );

    /**
     * Get the 3D matrix of the projection.
     * @param projection3D the parameter for the 3D projection.
     */
    void getProjection3D( glm::mat4 &projection3D ) const;

    /**
     * Get the 3D matrix of the view.
     * @param view3D the parameter for the 3D view.
     */
    void getView3D( glm::mat4 &view3D ) const;

    /**
     * Get the 3D matrix multiplication of projection3D and view3D.
     * @param PV3D the parameter for the 3D projection and view.
     */
    void getProjectionView3D( glm::mat4 &PV3D ) const;

    /**
     * @return the origin of the viewport.
     */
    glm::u32vec2 getViewportOrigin() const;

    /**
     * @return the dimensions the dimensions of the viewport.
     */
    glm::u32vec2 getViewportDimensions() const;

    /**
     * This gets the 3D camera shape from the projection matrix.
     * @note This might actually be useful for gameplay purposes.
     * @return the camera frustrum shape.
     */
    Utilities::Collision::GJKPolyhedron getProjection3DShape() const;

    /**
     * This gets the 3D camera shape from the projection matrix.
     * @note This might actually be useful for gameplay purposes.
     * @return the camera position.
     */
    glm::vec3 getPosition() const;
};

}

#endif // GRAPHICS_CAMERA_INSTANCE_H
