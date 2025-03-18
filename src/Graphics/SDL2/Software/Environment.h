#ifndef GRAPHICS_SDL2_SOFTWARE_ENVIRONMENT_H
#define GRAPHICS_SDL2_SOFTWARE_ENVIRONMENT_H

#include "../../Environment.h"

#include "Window.h"

#define CBMP_TEXTURE Utilities::GridBase2D<TexturePixel>

namespace Graphics::SDL2::Software {

class Environment : public Graphics::Environment {
public:
    Window *window_p;
    bool display_world;

    struct TexturePixel {
        uint8_t data[4];
    };

    struct CBMPTexture {
        uint32_t resource_id;
        CBMP_TEXTURE *texture_p;
    };

    std::vector<CBMPTexture> textures;

    // Configuration
    unsigned pixel_size;

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
    virtual Graphics::ModelInstance* allocateModel(uint32_t obj_resource_id, const glm::vec3 &position_param, const glm::quat &rotation_param, const glm::vec2 &texture_offset_param);
    virtual bool doesModelExist(uint32_t obj_resource_id) const;
    virtual Graphics::ParticleInstance* allocateParticleInstance();
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
    virtual void advanceTime( float seconds_passed );
};

}

#endif // GRAPHICS_SDL2_SOFTWARE_ENVIRONMENT_H
