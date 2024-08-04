#ifndef GRAPHICS_MODEL_INTERNAL_DATA_H
#define GRAPHICS_MODEL_INTERNAL_DATA_H

#include "../../ModelInstance.h"
#include "../../Environment.h"

// Implementation includes.
#include "Internal/GLES2.h"
#include "Internal/StaticModelDraw.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

class ModelInstance : public Graphics::ModelInstance {
public:
    Internal::StaticModelDraw::ModelArray *array_r;
    Internal::StaticModelDraw::ModelArray *bb_array_r;
    std::vector<float> star_timings;
    glm::vec3 culling_sphere_position;
    float culling_sphere_radius;
    
    ModelInstance( Graphics::Environment &env_r, uint32_t obj_identifier, const glm::vec3 &position_param, const glm::quat &rotation_param, const glm::vec2 &texture_offset_param );
    virtual ~ModelInstance();
    
    static bool exists( Graphics::Environment &env_r, uint32_t obj_identifier );
    
    virtual bool getBoundingSphere( glm::vec3 &position, float &radius ) const;
};

}
}
}

#endif // GRAPHICS_MODEL_INTERNAL_DATA_H
