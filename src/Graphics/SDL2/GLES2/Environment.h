#ifndef GRAPHICS_SDL2_GLES2_ENVIRONMENT_H
#define GRAPHICS_SDL2_GLES2_ENVIRONMENT_H

#include <vector>

#include "Internal/GLES2.h"

#include "Internal/MorphModelDraw.h"
#include "Internal/Texture2D.h"
#include "Internal/Draw2D.h"
#include "Internal/StaticModelDraw.h"
#include "Internal/SkeletalModelDraw.h"
#include "Internal/ParticleDraw.h"
#include "Internal/World.h"
#include "../../Camera.h"
#include "../../Environment.h"
#include "Window.h"

namespace Graphics::SDL2::GLES2 {

class Environment : public Graphics::Environment {
public:
    Graphics::SDL2::GLES2::Window *window_p;

    std::map<uint32_t, const Data::Mission::ANMResource*> anm_resources;
    std::map<uint32_t, Internal::Texture2D*> textures;
    Internal::Texture2D          *shiney_texture_p; // This holds the environment map.
    Internal::World              *world_p; // This handles drawing the whole world.
    bool display_world;

    bool has_initialized_routines;
    Internal::StaticModelDraw     static_model_draw_routine;
    Internal::MorphModelDraw      morph_model_draw_routine;
    Internal::SkeletalModelDraw   skeletal_model_draw_routine;
    Internal::Draw2D              draw_2d_routine;
    Internal::ParticleDraw        particle_draw_routine;
    Internal::DynamicTriangleDraw dynamic_triangle_draw_routine;

    bool draw_bounding_boxes;

    // Configuration
    bool force_gl2;
    int semi_transparent_limit;

public:
    Environment();
    virtual ~Environment();

    static int initSystem();
    static int deinitEntireSystem();

    virtual std::string getEnvironmentIdentifier() const;
    virtual int loadResources( const Data::Accessor &accessor );

    virtual Graphics::ExternalImage* allocateExternalImage(bool has_alpha = false);
    virtual Graphics::Camera* allocateCamera();
    virtual Graphics::Image* allocateImage();
    virtual Graphics::ModelInstance* allocateModel(uint32_t obj_resource_id);
    virtual bool doesModelExist(uint32_t obj_resource_id) const;
    virtual Graphics::ParticleInstance* allocateParticleInstance();
    virtual Graphics::QuadInstance* allocateQuadInstance();
    virtual Graphics::Text2DBuffer* allocateText2DBuffer();
    virtual Graphics::ANMFrame* allocateVideoANM(uint32_t track_offset);
    virtual Graphics::Window* allocateWindow();
    virtual Graphics::Window* getWindow();
    virtual bool displayMap( bool state );
    virtual size_t getTilAmount() const;
    virtual int setTilBlink( unsigned til_index, float seconds );
    virtual int setTilPolygonBlink( unsigned polygon_type, float rate = 1.0f);
    virtual bool getBoundingBoxDraw() const;
    virtual void setBoundingBoxDraw(bool draw);
    virtual void setupFrame();
    virtual void drawFrame();
    virtual bool screenshot( Utilities::Image2D &image ) const;
    virtual void advanceTime( std::chrono::microseconds delta );
};

}

#endif // GRAPHICS_SDL2_GLES2_ENVIRONMENT_H
