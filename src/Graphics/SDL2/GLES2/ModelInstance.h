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
    Internal::StaticModelDraw::ModelArray *array;
    Internal::StaticModelDraw *model_draw_r;
    glm::vec3 culling_sphere_position;
    float culling_sphere_radius;
    
    int index_position;
    
    ModelInstance(  Graphics::Environment &env_r, int cobj_index, const glm::vec3 &position_param, const glm::quat &rotation_param, const glm::vec2 &texture_offset_param );
    virtual ~ModelInstance();
    
    virtual bool getBoundingSphere( glm::vec3 &position, float &radius ) const;
};

}
}
}

#endif // GRAPHICS_MODEL_INTERNAL_DATA_H
