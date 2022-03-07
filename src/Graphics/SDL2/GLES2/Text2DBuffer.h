#ifndef GRAPHICS_2D_TEXT_BUFFER_INTERNAL_H
#define GRAPHICS_2D_TEXT_BUFFER_INTERNAL_H

#include "Internal/GLES2.h"
#include "Internal/FontSystem.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

struct Text2DBufferInternalData {
    Internal::FontSystem *font_system_r;
    std::vector<Internal::FontSystem::Text2D*> text_data;
    Internal::FontSystem::Text2D *current_text_2D_r; // This merely references the text_data vector.
    unsigned int buffer_size_per_font_KiB; // This is the memory size of the pages.
    unsigned int text_2D_expand_factor; // The amount of characters the text 2D expands
};

}
}
}

#endif // GRAPHICS_2D_TEXT_BUFFER_H
