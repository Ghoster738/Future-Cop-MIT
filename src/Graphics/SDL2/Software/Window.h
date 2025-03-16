#ifndef GRAPHICS_SDL2_SOFTWARE_WINDOW_H
#define GRAPHICS_SDL2_SOFTWARE_WINDOW_H

#include "../Window.h"
#include "Window.h"

namespace Graphics::SDL2::Software {

class Window : public Graphics::SDL2::Window {
public:
    SDL_Renderer *renderer_p;
    SDL_Texture  *texture_p;
    
    Window( Environment &env_r );
    virtual ~Window();
    
    int attach();
};

}

#endif // GRAPHICS_SDL2_SOFTWARE_WINDOW_H
