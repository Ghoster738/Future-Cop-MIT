#ifndef GRAPHICS_WINDOW_INSTANCE_INTERNAL_DATA_H
#define GRAPHICS_WINDOW_INSTANCE_INTERNAL_DATA_H

#include "SDL.h"
#include "GLES2/Internal/GLES2.h"
#include "../Window.h"

namespace Graphics {
namespace SDL2 {

class Window : public Graphics::Window {
protected:
    Uint32 flags;
    bool is_centered;
    
    Window( Environment &env_r );
public:
    SDL_Window *window_p;
    
    virtual ~Window();
    
    virtual void setWindowTitle( const std::string &window_title );
    virtual void setPosition( glm::u32vec2 position );
    virtual int setDimensions( glm::u32vec2 dimensions );
    
    virtual int center();
};

}
}

#endif // GRAPHICS_WINDOW_INSTANCE_INTERNAL_DATA_H
