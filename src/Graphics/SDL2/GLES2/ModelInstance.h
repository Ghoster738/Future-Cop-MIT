#ifndef GRAPHICS_MODEL_INTERNAL_DATA_H
#define GRAPHICS_MODEL_INTERNAL_DATA_H

#include "Internal/GLES2.h"

#include "Internal/MorphModelDraw.h"
#include "Internal/StaticModelDraw.h"
#include "Internal/SkeletalModelDraw.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

struct ModelInternalData {
    Internal::StaticModelDraw::ModelArray *array;
    glm::vec3 culling_sphere_position;
    float culling_sphere_radius;
    
    int index_position;
};

}
}
}

#endif // GRAPHICS_MODEL_INTERNAL_DATA_H
