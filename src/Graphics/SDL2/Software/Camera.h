#ifndef GRAPHICS_SDL2_SOFTWARE_CAMERA_H
#define GRAPHICS_SDL2_SOFTWARE_CAMERA_H

#include "../../Camera.h"

#include "../../ModelInstance.h"

#include "Text2DBuffer.h"
#include "../../../Utilities/Collision/GJKPolyhedron.h"

namespace Graphics::SDL2::Software {

class Camera : public Graphics::Camera {
public:
    std::vector<Graphics::Text2DBuffer*> text_2d_buffers;

    Camera();
    virtual ~Camera();
    
    bool isVisible( const Graphics::ModelInstance &instance ) const;
    bool isVisible( glm::vec2 bb0, glm::vec2 bb1 ) const;

    virtual const std::vector<Graphics::Text2DBuffer*> *const getText2DBuffer() const;
    virtual int attachText2DBuffer( Graphics::Text2DBuffer& buffer_p );
    virtual int removeText2DBuffer( Graphics::Text2DBuffer* buffer_p );
};

}

#endif // GRAPHICS_SDL2_GLES2_CAMERA_H
