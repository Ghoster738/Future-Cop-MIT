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
    Internal::Texture2D *GUI_texture; // This holds the indicators of Health, Ammo, and Radar for the game.
    std::vector<Internal::Texture2D> general_textures; // This holds the primary textures for the 3d map and the 3d models, and sometimes the small gui elements.
    std::vector<Internal::Texture2D*> general_textures_r;
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
