#ifndef GRAPHICS_SDL2_SOFTWARE_WINDOW_H
#define GRAPHICS_SDL2_SOFTWARE_WINDOW_H

#include "../Window.h"

namespace Graphics::SDL2::Software {

class Environment;

class Window : public Graphics::SDL2::Window {
public:
    Graphics::SDL2::Software::Environment* env_r;

    struct DifferredPixel {
        uint8_t  colors[4]; // Last one is texture id.
        uint8_t  texture_coordinates[2];
        uint32_t depth; // Can be even 16 bit which reduces DifferredPixel to 8 bytes from 12 bytes.
    };

    SDL_Renderer                          *renderer_p;
    SDL_Texture                           *texture_p;
    Utilities::GridBase2D<DifferredPixel>  differred_buffer;
    Utilities::GridBase2D<uint32_t>        destination_buffer;
    int                                    destination_buffer_pitch;

    unsigned pixel_size;
    
    Window( Graphics::SDL2::Software::Environment &env_r );
    virtual ~Window();
    
    int attach();
};

}

#endif // GRAPHICS_SDL2_SOFTWARE_WINDOW_H
