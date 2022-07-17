#ifndef GRAPHICS_ENVIRONMENT_DATA_H
#define GRAPHICS_ENVIRONMENT_DATA_H

#include <vector>

#include "Internal/GLES2.h"

#include "Internal/FontSystem.h"
#include "Internal/MorphModelDraw.h"
#include "Internal/Texture2D.h"
#include "Internal/StaticModelDraw.h"
#include "Internal/SkeletalModelDraw.h"
#include "Internal/World.h"
#include "../../Camera.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

struct EnvironmentInternalData {
    std::map<uint32_t, Internal::Texture2D> textures;
    
    Internal::Texture2D shiney_texture; // This holds the environment map.

    Internal::World *world; // This handles drawing the whole world.
    Internal::FontSystem                 *text_draw_routine;
    Internal::StaticModelDraw     static_model_draw_routine;
    Internal::MorphModelDraw       morph_model_draw_routine;
    Internal::SkeletalModelDraw skeletal_model_draw_routine;
};

}
}
}

#endif // GRAPHICS_ENVIRONMENT_DATA_H
