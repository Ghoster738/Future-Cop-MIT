#ifndef GRAPHICS_ENVIRONMENT_DATA_H
#define GRAPHICS_ENVIRONMENT_DATA_H

#include <vector>

#include "Internal/GLES2.h"

#include "Internal/MorphModelDraw.h"
#include "Internal/Texture2D.h"
#include "Internal/FontSystem.h"
#include "Internal/StaticModelDraw.h"
#include "Internal/SkeletalModelDraw.h"
#include "Internal/ParticleDraw.h"
#include "Internal/World.h"
#include "../../Camera.h"
#include "../../Environment.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

class Environment : public Graphics::Environment {
public:
    SDL2::GLES2::Internal::FontSystem                   *text_draw_routine_p;

    std::map<uint32_t, Graphics::SDL2::GLES2::Internal::Texture2D*> textures;
    Graphics::SDL2::GLES2::Internal::Texture2D          *shiney_texture_p; // This holds the environment map.
    Graphics::SDL2::GLES2::Internal::World              *world_p; // This handles drawing the whole world.
    bool display_world;

    bool has_initialized_routines;
    Graphics::SDL2::GLES2::Internal::StaticModelDraw     static_model_draw_routine;
    Graphics::SDL2::GLES2::Internal::MorphModelDraw      morph_model_draw_routine;
    Graphics::SDL2::GLES2::Internal::SkeletalModelDraw   skeletal_model_draw_routine;
    Graphics::SDL2::GLES2::Internal::ParticleDraw        particle_draw_routine;
    Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw dynamic_triangle_draw_routine;

    bool draw_bounding_boxes;

public:
    Environment();
    virtual ~Environment();

    static int initSystem();
    static int deinitEntireSystem();

    virtual std::string getEnvironmentIdentifier() const;
    virtual int loadResources( const Data::Accessor &accessor );

    virtual Graphics::Image* allocateImage();
    virtual Graphics::ExternalImage* allocateExternalImage();
    virtual Graphics::ParticleInstance* allocateParticleInstance();
    virtual bool displayMap( bool state );
    virtual size_t getTilAmount() const;
    virtual int setTilBlink( unsigned til_index, float seconds );
    virtual int setTilPolygonBlink( unsigned polygon_type, float rate = 1.0f);
    virtual bool getBoundingBoxDraw() const;
    virtual void setBoundingBoxDraw(bool draw);
    virtual void setupFrame();
    virtual void drawFrame();
    virtual bool screenshot( Utilities::Image2D &image ) const;
    virtual void advanceTime( float seconds_passed );
};

}
}
}

#endif // GRAPHICS_ENVIRONMENT_DATA_H
