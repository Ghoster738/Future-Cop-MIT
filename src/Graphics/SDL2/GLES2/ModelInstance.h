#ifndef GRAPHICS_SDL2_GLES2_MODEL_INSTANCE_H
#define GRAPHICS_SDL2_GLES2_MODEL_INSTANCE_H

#include "../../ModelInstance.h"
#include "Environment.h"

// Implementation includes.
#include "Internal/GLES2.h"
#include "Internal/StaticModelDraw.h"

namespace Graphics::SDL2::GLES2 {

class ModelInstance : public Graphics::ModelInstance {
public:
    Graphics::SDL2::GLES2::Environment *environment_r;
    Internal::StaticModelDraw::ModelArray *array_r;
    Internal::StaticModelDraw::ModelArray *bb_array_r;
    std::vector<float> star_timings;
    glm::vec3 culling_sphere_position;
    float culling_sphere_radius;
    
    ModelInstance( Graphics::SDL2::GLES2::Environment &env_r, uint32_t obj_identifier, const glm::vec3 &position_param, const glm::quat &rotation_param, const glm::vec2 &texture_offset_param );
    virtual ~ModelInstance();

    virtual void setPositionTransformTimeline( float &position_transform_timeline );
    
    virtual bool getBoundingSphere( glm::vec3 &position, float &radius ) const;
};

}

#endif // GRAPHICS_SDL2_GLES2_MODEL_INSTANCE_H
