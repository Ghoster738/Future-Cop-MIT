#include "Window.h" // Include the internal class

#include "Environment.h"

#include <thread>

namespace Graphics::SDL2::Software {

Graphics::Window* Environment::allocateWindow() {
    if(this->window_p != nullptr)
        return this->window_p;

    auto window_p = new Graphics::SDL2::Software::Window( *this );

    this->window_p = window_p;

    this->window_p->rendering_rects.push_back( {} );
    this->window_p->rendering_rects.back().env_r        = this;
    this->window_p->rendering_rects.back().area.start_x =    0;
    this->window_p->rendering_rects.back().area.start_y =    0;
    this->window_p->rendering_rects.back().area.end_x   =   32;
    this->window_p->rendering_rects.back().area.end_y   =   32;

    return window_p;
}

Window::Window( Graphics::SDL2::Software::Environment &env ) : Graphics::SDL2::Window(), renderer_p( nullptr ), texture_p( nullptr ) {
    this->env_r = &env;

    this->rendering_rect_area_index = 0;
}

Window::~Window() {
    if( this->texture_p != nullptr )
        SDL_DestroyTexture(this->texture_p);
    this->texture_p = nullptr;

    if( this->renderer_p != nullptr )
        SDL_DestroyRenderer(this->renderer_p);
    this->renderer_p = nullptr;
}

int Window::attach() {
    auto error_log = Utilities::logger.getLog( Utilities::Logger::ERROR );
    error_log.info << "Software Graphics Window::attach\n";
    auto info_log = Utilities::logger.getLog( Utilities::Logger::INFO );

    int success = -1;

    {
        if( this->texture_p != nullptr )
            SDL_DestroyTexture(this->texture_p);
        this->texture_p = nullptr;

        if( this->renderer_p != nullptr )
            SDL_DestroyRenderer(this->renderer_p);
        this->renderer_p = nullptr;

        if( this->window_p != nullptr )
            SDL_DestroyWindow( this->window_p );
        this->window_p = nullptr;
        
        // This simple logic is there to determine if the window is centered or not.
        auto position = getPosition();
        
        if( this->is_centered )
            position = glm::u32vec2( SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED );
        
        this->window_p = SDL_CreateWindow( getWindowTitle().c_str(),
                                     getPosition().x, getPosition().y,
                                     getDimensions().x, getDimensions().y,
                                     flags | SDL_WINDOW_SHOWN );
        
        if( this->window_p != nullptr ) {
            this->renderer_p = SDL_CreateRenderer(this->window_p, -1, 0);

            if( this->renderer_p != nullptr ) {
                glm::ivec2 resolution = this->getDimensions();

                resolution.x /= this->env_r->pixel_size;
                resolution.y /= this->env_r->pixel_size;

                this->texture_p = SDL_CreateTexture(this->renderer_p, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, resolution.x, resolution.y );

                if( this->texture_p != nullptr ) {
                    this->destination_buffer.setDimensions( resolution.x, resolution.y );

                    this->factor.x = this->getDimensions().x / this->destination_buffer.getWidth();
                    this->factor.y = this->getDimensions().y / this->destination_buffer.getHeight();
                    this->inv_factor = glm::vec2(1. / factor.x, 1. / factor.y);

                    const auto max_thread_count = std::thread::hardware_concurrency();

                    std::lock_guard<std::mutex> lock_guard(this->rendering_rect_area_mutex);

                    // Build grid of rendering_rect_areas
                    const auto advance_x = this->rendering_rects.back().area.end_x - this->rendering_rects.back().area.start_x;
                    const auto advance_y = this->rendering_rects.back().area.end_y - this->rendering_rects.back().area.start_y;

                    for(unsigned y = 0; y < this->destination_buffer.getHeight(); y += advance_y) {
                        for(unsigned x = 0; x < this->destination_buffer.getWidth(); x += advance_x) {
                            RenderingRectArea area;

                            area.start_x = x;
                            area.start_y = y;
                            area.end_x   = x + advance_x;
                            area.end_y   = y + advance_y;

                            if(area.end_x > this->destination_buffer.getWidth())
                                area.end_x = this->destination_buffer.getWidth();

                            if(area.end_y > this->destination_buffer.getHeight())
                                area.end_y = this->destination_buffer.getHeight();

                            rendering_rect_areas.push_back( area );
                        }
                    }

                    this->rendering_rects.back().differred_buffer.setDimensions(
                        this->rendering_rects.back().area.end_x - this->rendering_rects.back().area.start_x,
                        this->rendering_rects.back().area.end_y - this->rendering_rects.back().area.start_y );

                    success = 1;
                }
                else {
                    error_log.output << "Allocating the texture failed: " << SDL_GetError() << "\n";
                }
            }
            else {
                error_log.output << "Allocating the renderer failed: " << SDL_GetError() << "\n";
            }
        }
        else
        {
            error_log.output << "SDL Window Error: " << SDL_GetError() << "\n";
        }
    }

    { // this makes color stripes
        this->destination_buffer_pitch = 4 * this->destination_buffer.getWidth();

        for(auto y = this->destination_buffer.getHeight(); y != 0; y--) {
            for(auto x = this->destination_buffer.getWidth(); x != 0; x--) {
                // Blue strip screen of nothingness.
                uint32_t pixel = 0xFF008FFF;

                if( (y % 8) > 4)
                    pixel = 0xFF008F00;

                this->destination_buffer.setValue((x - 1), (y - 1), pixel);
            }
        }
    }
    
    // error_log.output << "DifferredPixel: " << sizeof(DifferredPixel) << " bytes.\n";

    return success;
}

const Window::RenderingRectArea* Window::getRenderArea() {
    std::lock_guard<std::mutex> lock_guard(this->rendering_rect_area_mutex);

    assert(!this->rendering_rect_areas.empty());
    if(this->rendering_rect_area_index < this->rendering_rect_areas.size())
        return &this->rendering_rect_areas[this->rendering_rect_area_index++];
    else
        return nullptr;
}

void Window::resetRenderAreas() {
    std::lock_guard<std::mutex> lock_guard(this->rendering_rect_area_mutex);
    this->rendering_rect_area_index = 0;
}

}
