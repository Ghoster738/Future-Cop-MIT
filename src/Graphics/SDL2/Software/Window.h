#ifndef GRAPHICS_SDL2_SOFTWARE_WINDOW_H
#define GRAPHICS_SDL2_SOFTWARE_WINDOW_H

#include "../Window.h"
#include "Window.h"

namespace Graphics::SDL2::Software {

class Window : public Graphics::SDL2::Window {
public:
    struct DifferredPixel {
        uint8_t colors[4];
        struct {
            uint32_t   depth: 28;
            uint32_t texture:  4;
        } depth_texture;
        uint8_t texture_coordinates[2];
    };


    SDL_Renderer   *renderer_p;
    SDL_Texture    *texture_p;
    DifferredPixel *differred_buffer_p;
    uint32_t       *pixel_buffer_p;
    int             pixel_buffer_pitch;
    
    Window( Environment &env_r );
    virtual ~Window();
    
    int attach();
};

}

#endif // GRAPHICS_SDL2_SOFTWARE_WINDOW_H
