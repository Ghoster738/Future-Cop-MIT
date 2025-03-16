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

Window::Window( Graphics::Environment &env ) : Graphics::SDL2::Window( env ), renderer_p( nullptr ), texture_p( nullptr ), pixel_buffer_p( nullptr ) {}

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
                this->texture_p = SDL_CreateTexture(this->renderer_p, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, getDimensions().x, getDimensions().y);

                if( this->texture_p != nullptr ) {
                    this->differred_buffer.setDimensions( getDimensions().x, getDimensions().y );
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

    if(this->pixel_buffer_p != nullptr) {
        this->pixel_buffer_pitch = 4 * getDimensions().x;

        for(auto y = getDimensions().y; y != 0; y--) {
            for(auto x = getDimensions().x; x != 0; x--) {
                // Blue strip screen of nothingness.
                uint32_t pixel = 0xFF008FFF;

                if( (y % 8) > 4)
                    pixel = 0xFF008F00;

                this->pixel_buffer_p[(x - 1) + getDimensions().x * (y - 1)] = pixel;
            }
        }
    }

    {
        int texture_id = 0;
        int choices = 0;

        uint8_t r_choices[] = {0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00};
        uint8_t g_choices[] = {0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff};
        uint8_t b_choices[] = {0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff};

        for(auto y = getDimensions().y; y != 0; y--) {
            for(auto x = getDimensions().x; x != 0; x--) {
                DifferredPixel pixel;

                texture_id = (x - 1) / 0x100;
                choices    = (y - 1) / 0x100;

                pixel.colors[0] = r_choices[choices % (sizeof(r_choices) / sizeof(r_choices[0]))];
                pixel.colors[1] = g_choices[choices % (sizeof(g_choices) / sizeof(g_choices[0]))];
                pixel.colors[2] = b_choices[choices % (sizeof(b_choices) / sizeof(b_choices[0]))];
                pixel.colors[3] = texture_id % 12;
                pixel.texture_coordinates[0] = (x - 1) % 0x100;
                pixel.texture_coordinates[1] = (y - 1) % 0x100;
                pixel.depth = 0xFFFF;

                this->differred_buffer.setValue((x - 1), (y - 1), pixel);
            }
        }
    }
    
    error_log.output << "DifferredPixel: " << sizeof(DifferredPixel) << " bytes.\n";

    return success;
}

}
