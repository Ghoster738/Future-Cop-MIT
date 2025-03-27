#ifndef GRAPHICS_SDL2_SOFTWARE_INTERNAL_WORLD_DRAW_3D_H
#define GRAPHICS_SDL2_SOFTWARE_INTERNAL_WORLD_DRAW_3D_H

#include "../../../../Utilities/GridBase2D.h"

#include "../Window.h"

#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace Data {

class Accessor;

}

namespace Graphics::SDL2::Software::Internal {

class WorldDraw3D {
public:
    struct Vertex {
        glm::vec3 position;
        glm::u8vec3  color;
        glm::u8vec2     uv;
    };

    struct Section {
        std::vector<Vertex> vertices;
    };

    std::vector<Section> sections;

    Utilities::GridBase2D<Section*> map;

    virtual ~WorldDraw3D();

    bool load( const Data::Accessor &accessor );

    void drawOpaque(Window::RenderingRect &rendering_rect) const;
    void draw(Window::RenderingRect &rendering_rect) const;
};

}

#endif // GRAPHICS_SDL2_SOFTWARE_INTERNAL_WORLD_DRAW_3D_H
