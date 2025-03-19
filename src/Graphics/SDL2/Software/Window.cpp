#include "Window.h" // Include the internal class

#include "Environment.h"

namespace Graphics::SDL2::Software {

Graphics::Window* Environment::allocateWindow() {
    if(this->window_p != nullptr)
        return this->window_p;

    auto window_p = new Graphics::SDL2::Software::Window( *this );

    this->window_p = window_p;

    return window_p;
}

Window::Window( Graphics::SDL2::Software::Environment &env ) : Graphics::SDL2::Window(), env_r( &env ), renderer_p( nullptr ), texture_p( nullptr ) {}

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
                    this->differred_buffer.setDimensions( resolution.x, resolution.y );
                    this->destination_buffer.setDimensions( resolution.x, resolution.y );

                    this->factor.x = this->getDimensions().x / this->destination_buffer.getWidth();
                    this->factor.y = this->getDimensions().y / this->destination_buffer.getHeight();
                    this->inv_factor = glm::vec2(1. / factor.x, 1. / factor.y);

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

}
