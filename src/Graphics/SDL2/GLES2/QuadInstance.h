#ifndef GRAPHICS_SDL2_GLES2_QUAD_INSTANCE_H
#define GRAPHICS_SDL2_GLES2_QUAD_INSTANCE_H

#include "../../QuadInstance.h"

#include "../../../Data/Mission/DCSResource.h"

#include "Environment.h"

namespace Graphics::SDL2::GLES2 {

class QuadInstance : public Graphics::QuadInstance {
public:
    Environment *environment_r;

    const Data::Mission::DCSResource::Image *texture_info_r;

    virtual ~QuadInstance();

    virtual void setQuadID( uint8_t dcs_id );
    virtual uint8_t getQuadID() const;

    virtual void update();
};

}

#endif // GRAPHICS_SDL2_GLES2_QUAD_INSTANCE_H
