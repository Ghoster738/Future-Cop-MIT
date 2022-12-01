#ifndef GRAPHICS_CAMERA_INSTANCE_H
#define GRAPHICS_CAMERA_INSTANCE_H

#include "Text2DBuffer.h"
#include "../Utilities/DataTypes.h"
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

    // These are for the 2D environment which would mostly be used for GUI.
    glm::mat4 projection2D;
    glm::mat4 view2D;
    glm::mat4 PV2D;

    // Viewport locations
    glm::u32vec2 origin;
    glm::u32vec2 dimensions; // x = width, y = height

    // Hold the Text 2D Buffers.
    std::vector<Text2DBuffer*> text_2d_buffers;

    /**
     * When projection3D or view3D had been changed, the PV3D needs to reflect this.
     * This multiplies projection3D and view3D, and stores the result in PV3D.
     */
    void updatePV3D();

    /**
     * When projection2D or view2D had been changed, the PV2D needs to reflect this.
     * This multiplies projection2D and view3D, and stores the result in PV2D.
     */
    void updatePV2D();

public:
    Camera();
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
     * This sets the 2d projection, and multiplies it with the 2d view matrix.
     * Use this command only when view 2d stays constant, or is not changed.
     * @param projection2D sets the projection of this camera's 2D mode.
     */
    void setProjection2D( const glm::mat4 &projection2D );

    /**
     * This sets the 2d view, and multiplies it with the 2d projection matrix.
     * Use this command only when Projection 2d stays constant, or is not changed.
     * @param view2D sets the view of this camera's 2D mode.
     */
    void setView2D( const glm::mat4 &view2D );

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
     * If both the projection and the view 2D matrices had been changed use this method.
     * @param projection2D sets the projection of this camera's 2D mode.
     * @param view2D sets the view of this camera's 2D mode.
     */
    void setProjectionView2D( const glm::mat4 &projection2D, const glm::mat4 &view2D );

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
     * Get the 2D matrix of the projection.
     * @param projection2D the parameter for the 2D projection.
     */
    void getProjection2D( glm::mat4 &projection2D ) const;

    /**
     * Get the 2D matrix of the view.
     * @param view2D the parameter for the 2D view.
     */
    void getView2D( glm::mat4 &view2D ) const;

    /**
     * Get the 2D matrix multiplication of projection2D and view2D.
     * @param PV2D the parameter for the 2D projection and view.
     */
    void getProjectionView2D( glm::mat4 &PV2D ) const;

    /**
     * Get the 2D Text buffers attached to the camera.
     */
    const std::vector<Text2DBuffer*> *const getText2DBuffer() const;

    /**
     * @param origin the origin of the viewport.
     */
    glm::u32vec2 getViewportOrigin() const;

    /**
     * @param dimensions the dimensions of the viewport.
     */
    glm::u32vec2 setViewportDimensions() const;

    /**
     * Attach the Text 2D Buffer.
     * @return 1 if the buffer gets attached.
     */
    int attachText2DBuffer( Text2DBuffer& buffer_p );

    /**
     * Delete the Text 2D Buffer.
     * @return true if the buffer is found and deleted.
     */
    int removeText2DBuffer( Text2DBuffer* buffer_p );
};

}

#endif // GRAPHICS_CAMERA_INSTANCE_H
