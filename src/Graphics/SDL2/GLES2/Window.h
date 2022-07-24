#ifndef GRAPHICS_SDL2_GLES2_WINDOWS_H
#define GRAPHICS_SDL2_GLES2_WINDOWS_H

#include "../Window.h"
#include "Window.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

class Window : public Graphics::SDL2::Window {
public:
    SDL_GLContext GL_context;
    
    Window();
    virtual ~Window();
};

}
}
}

#endif // GRAPHICS_SDL2_GLES2_WINDOWS_H
