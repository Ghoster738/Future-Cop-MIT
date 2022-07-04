#include "../Window.h" // Include the interface class
#include "Window.h" // Include the internal class

Graphics::Window::Window() :
    window_internal_data_p( nullptr ),
    window_title( "WINDOW TITLE NOT SET" ),
    position( glm::u32vec2( 0, 0 ) ),
    dimensions( glm::u32vec2( 320, 200 ) )
{
    status.window_status = Status::WINDOW;

    window_internal_data_p = reinterpret_cast<void*>( new SDL2::WindowInternalData() );

    auto window_internal_p = reinterpret_cast<Graphics::SDL2::WindowInternalData*>( window_internal_data_p );

    window_internal_p->window_p = nullptr;
}

Graphics::Window::~Window() {
    auto window_internal_p = reinterpret_cast<Graphics::SDL2::WindowInternalData*>( window_internal_data_p );

    for( unsigned int i = 0; i < cameras.size(); i++ )
        delete cameras[ i ];

    if( window_internal_p->window_p != nullptr )
        SDL_DestroyWindow( window_internal_p->window_p );

    delete window_internal_p;
}

void Graphics::Window::setWindowTitle( const std::string &window_title ) {
    this->window_title = window_title;

    auto window_internal_p = reinterpret_cast<Graphics::SDL2::WindowInternalData*>( window_internal_data_p );
    if( window_internal_p != nullptr )
        SDL_SetWindowTitle( window_internal_p->window_p, this->window_title.c_str() );
}

void Graphics::Window::setPosition( glm::u32vec2 position ) {
    this->position = position;

    auto window_internal_p = reinterpret_cast<Graphics::SDL2::WindowInternalData*>( window_internal_data_p );
    if( window_internal_p->window_p != nullptr )
        SDL_SetWindowPosition( window_internal_p->window_p, this->position.x, this->position.y );
}

int Graphics::Window::setDimensions( glm::u32vec2 dimensions ) {
    // For now simply forbid the resizing of a window.
    auto window_internal_p = reinterpret_cast<Graphics::SDL2::WindowInternalData*>( window_internal_data_p );

    // Only allow the window to be resized when it was not loaded yet.
    if( window_internal_p->window_p == nullptr )
    {
        this->dimensions = dimensions;
        return 1;
    }
    else // If the window is attached to the environment.
        return 0; // Not Supported
}

void Graphics::Window::setFullScreen( bool is_full_screen ) {
    if( is_full_screen )
        status.window_status = Status::FULL_SCREEN;
    else
    if( status.window_status != Status::FULL_SCREEN )
        status.window_status = Status::WINDOW;
}

void Graphics::Window::setBoarder( bool boarder ) {
    if( status.window_status != Status::FULL_SCREEN )
    {
        if( boarder == true )
            status.window_status = Status::WINDOW_BOARDER;
        else
            status.window_status = Status::WINDOW;
    }
}

int Graphics::Window::center() {
    auto window_internal_p = reinterpret_cast<Graphics::SDL2::WindowInternalData*>( window_internal_data_p );
    int display_index;
    SDL_Rect screen;

    if( status.window_status != Status::FULL_SCREEN && window_internal_p != nullptr )
    {
        // First we need the display index of the window.
        display_index = SDL_GetWindowDisplayIndex( window_internal_p->window_p );

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
                return -3;
        }
        else
            return -2;
    }
    else
        return -1;
}

int Graphics::Window::attachCamera( Graphics::Camera &camera_instance ) {
    cameras.push_back( &camera_instance );

    return 1;
}

int Graphics::Window::removeCamera( Graphics::Camera *camera_p ) {
    int erased_value = 0;

    if( camera_p != nullptr )
    {
        for( auto i = cameras.begin(); i != cameras.end(); i++ )
        {
            if( *i == camera_p )
            {
                // The camera has been found and deleted
                delete camera_p;

                cameras.erase( i );

                erased_value = 1;

                i = cameras.end();
            }
        }
    }

    return erased_value;
}

const std::vector<Graphics::Camera*> *const Graphics::Window::getCameras() const {
    return &cameras;
}

std::string Graphics::Window::getWindowTitle() const {
    return window_title;
}

glm::u32vec2 Graphics::Window::getPosition() const {
    return position;
}

glm::u32vec2 Graphics::Window::getDimensions() const {
    return dimensions;
}
