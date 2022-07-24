#ifndef GRAPHICS_WINDOW_INSTANCE_INTERNAL_DATA_H
#define GRAPHICS_WINDOW_INSTANCE_INTERNAL_DATA_H

#include <SDL2/SDL.h>
#include "GLES2/Internal/GLES2.h"

namespace Graphics {
namespace SDL2 {

struct WindowInternalData {
    SDL_Window *window_p;
    SDL_GLContext GL_context; // This stores SDL OpenGL Rendering context.
};

class WindowManager : public Graphics::Window::Manager {
protected:
    Graphics::Window *window_p;
public:
    WindowManager();
    
    ~WindowManager();
    
    Window* allocWindow();
};

}
}

#endif // GRAPHICS_WINDOW_INSTANCE_INTERNAL_DATA_H
