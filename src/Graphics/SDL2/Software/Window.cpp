#include "Window.h" // Include the internal class

#include "Environment.h"

namespace Graphics::SDL2::Software {

Graphics::Window* Environment::allocateWindow() {
    auto window_p = new Graphics::SDL2::Software::Window( *this );

    return window_p;
}

Window::Window( Graphics::Environment &env ) : Graphics::SDL2::Window( env ), renderer_p( nullptr ), texture_p( nullptr ), differred_buffer_p( nullptr ), pixel_buffer_p( nullptr ) {}

Window::~Window() {
    if( this->texture_p != nullptr )
        SDL_DestroyTexture(this->texture_p);
    this->texture_p = nullptr;

    if( this->renderer_p != nullptr )
        SDL_DestroyRenderer(this->renderer_p);
    this->renderer_p = nullptr;

    if( this->pixel_buffer_p != nullptr )
        delete this->pixel_buffer_p;
    this->pixel_buffer_p = nullptr;

    if( this->differred_buffer_p != nullptr )
        delete this->differred_buffer_p;
    this->differred_buffer_p = nullptr;
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

        if( this->pixel_buffer_p != nullptr )
            delete this->pixel_buffer_p;
        this->pixel_buffer_p = nullptr;

        if( this->differred_buffer_p != nullptr )
            delete this->differred_buffer_p;
        this->differred_buffer_p = nullptr;

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
            this->renderer_p = SDL_CreateRenderer(this->window_p, -1, SDL_RENDERER_SOFTWARE);

            if( this->renderer_p != nullptr ) {
                this->texture_p = SDL_CreateTexture(this->renderer_p, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, getDimensions().x, getDimensions().y);

                if( this->texture_p != nullptr ) {
                    this->differred_buffer_p = new DifferredPixel [getDimensions().x * getDimensions().y];
                    this->pixel_buffer_p = new uint32_t [getDimensions().x * getDimensions().y];
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
    
    env_r->window_p = this;
    this->pixel_buffer_pitch = 4 * getDimensions().x;

    for(auto y = getDimensions().y; y != 0; y--) {
        for(auto x = getDimensions().x; x != 0; x--) {
            // Blue screen of nothingness.
            uint32_t pixel = 0xFF008FFF;

            if( (y % 8) > 4)
                pixel = 0xFF008F00;

            this->pixel_buffer_p[(x - 1) + getDimensions().x * (y - 1)] = pixel;
        }
    }
    
    error_log.output << "DifferredPixel: " << sizeof(DifferredPixel) << " bytes.\n";

    return success;
}

}
