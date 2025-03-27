#ifndef GRAPHICS_SDL2_GLES2_WINDOWS_H
#define GRAPHICS_SDL2_GLES2_WINDOWS_H

#include "../Window.h"
#include "Window.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

class Environment;

class Window : public Graphics::SDL2::Window {
public:
    Graphics::SDL2::GLES2::Environment* env_r;
    SDL_GLContext GL_context;
    bool prioritize_opengl_2_fallback;
    
    Window( Graphics::SDL2::GLES2::Environment &env_r );
    virtual ~Window();
    
    int attach();
};

}
}
}

#endif // GRAPHICS_SDL2_GLES2_WINDOWS_H
