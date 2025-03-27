#ifndef GRAPHICS_SDL2_SOFTWARE_WINDOW_H
#define GRAPHICS_SDL2_SOFTWARE_WINDOW_H

#include "../Window.h"

#include "../../../Utilities/GridBase2D.h"

#include <mutex>
#include <vector>

namespace Graphics::SDL2::Software {

class Environment;

class Window : public Graphics::SDL2::Window {
public:
    struct DifferredPixel {
        uint8_t  colors[4]; // Last one is texture id.
        uint8_t  texture_coordinates[2];
        uint32_t depth; // Can be even 16 bit which reduces DifferredPixel to 8 bytes from 12 bytes.
    };

    struct RenderingRectArea {
        unsigned start_x, end_x;
        unsigned start_y, end_y;
    };

    struct RenderingRect {
        Utilities::GridBase2D<Window::DifferredPixel> differred_buffer; // Only write and read pixels.
        Environment *env_r; // Do not modify while drawing!
        RenderingRectArea area; // Do not modify while drawing!
    };

    glm::vec2 factor;
    glm::vec2 inv_factor;

    Environment                     *env_r;
    SDL_Renderer                    *renderer_p;
    SDL_Texture                     *texture_p;
    Utilities::GridBase2D<uint32_t>  destination_buffer;
    int                              destination_buffer_pitch;

    std::vector<RenderingRect>       rendering_rects;

private:
    std::mutex rendering_rect_area_mutex;
    size_t rendering_rect_area_index;
    std::vector<RenderingRectArea> rendering_rect_areas;

public:
    Window( Graphics::SDL2::Software::Environment &env_r );
    virtual ~Window();
    
    int attach();

    const RenderingRectArea* getRenderArea();
    void resetRenderAreas();
};

}

#endif // GRAPHICS_SDL2_SOFTWARE_WINDOW_H
