#ifndef GRAPHICS_SDL2_GLES2_CAMERA_H
#define GRAPHICS_SDL2_GLES2_CAMERA_H

#include "../../Camera.h"

#include "../../ModelInstance.h"

#include "Text2DBuffer.h"
#include "../../../Utilities/Collision/GJKPolyhedron.h"
#include "Internal/DynamicTriangleDraw.h"

namespace Graphics::SDL2::GLES2 {

class Camera : public Graphics::Camera {
public:
    Utilities::GridBase2D<float> culling_info;
    Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::DrawCommand transparent_triangles;

    Camera();
    virtual ~Camera();
    
    bool isVisible( const Graphics::ModelInstance &instance ) const;
    bool isVisible( glm::vec2 bb0, glm::vec2 bb1 ) const;
};

}

#endif // GRAPHICS_SDL2_GLES2_CAMERA_H
