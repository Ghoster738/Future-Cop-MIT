#ifndef GRAPHICS_SDL2_GLES2_CAMERA_H
#define GRAPHICS_SDL2_GLES2_CAMERA_H

#include "../../Camera.h"

#include "Text2DBuffer.h"
#include "../../../Utilities/Collision/GJKPolyhedron.h"
#include "Internal/DynamicTriangleDraw.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

class Camera : public Graphics::Camera {
public:
    std::vector<Graphics::Text2DBuffer*> text_2d_buffers;
    Utilities::GridBase2D<float> culling_info;
    Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::DrawCommand transparent_triangles;

    Camera();
    virtual ~Camera();

    virtual const std::vector<Graphics::Text2DBuffer*> *const getText2DBuffer() const;
    virtual int attachText2DBuffer( Graphics::Text2DBuffer& buffer_p );
    virtual int removeText2DBuffer( Graphics::Text2DBuffer* buffer_p );
};

}
}
}

#endif // GRAPHICS_SDL2_GLES2_CAMERA_H
