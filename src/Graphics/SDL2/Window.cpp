#include "Window.h" // Include the internal class

Graphics::SDL2::Window::Window( Environment &env ) : Graphics::Window( env ), flags( 0 ), is_centered( false ), window_p( nullptr )
{
}

Graphics::SDL2::Window::~Window() {
    if( window_p != nullptr )
        SDL_DestroyWindow( window_p );
}

void Graphics::SDL2::Window::setWindowTitle( const std::string &window_title ) {
    this->window_title = window_title;
    
    if( window_p != nullptr )
        SDL_SetWindowTitle( window_p, this->window_title.c_str() );
}

void Graphics::SDL2::Window::setPosition( glm::u32vec2 position ) {
    this->position = position;

    if( window_p != nullptr )
        SDL_SetWindowPosition( window_p, this->position.x, this->position.y );
}

int Graphics::SDL2::Window::setDimensions( glm::u32vec2 dimensions ) {
    // Only allow the window to be resized when it was not loaded yet.
    if( window_p == nullptr )
    {
        this->dimensions = dimensions;
        return 1;
    }
    else // If the window is attached to the environment.
        return 0; // Not Supported
}

void Graphics::SDL2::Window::setFullScreen( bool is_full_screen ) {
    // TODO Implement a way for SDL_WINDOW_FULLSCREEN_DESKTOP to be used when the resolution is the same.
    // TODO Implement logging, but first there must be buffer limits in place.

    if( is_full_screen )
        SDL_SetWindowFullscreen( this->window_p, SDL_WINDOW_FULLSCREEN );
    else
        SDL_SetWindowFullscreen( this->window_p, 0 );
}

int Graphics::SDL2::Window::center() {
    int display_index;
    SDL_Rect screen;

    if( window_p != nullptr ) {
        // First we need the display index of the window.
        display_index = SDL_GetWindowDisplayIndex( window_p );
        
        // Check if the display index is valid.
        if( display_index >= 0 )
        {
            // Attempt to get the bounds of the window
            if( SDL_GetDisplayBounds( display_index, &screen ) == 0 )
            {
                setPosition( glm::u32vec2( (screen.w - dimensions.x) / 2, (screen.h - dimensions.y) / 2 ) );
                
                return 1;
            }
            else
                return -2;
        }
        else
            return -1;
    }
    else
    {
        this->is_centered = true;
        return 1;
    }
}
