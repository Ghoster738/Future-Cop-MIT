#include "Window.h"
#include "Environment.h"

#include "SDL2/Window.h"

Graphics::Window::Window( Environment &env ) : env_r( &env ),
    window_title( "WINDOW TITLE NOT SET" ),
    position( glm::u32vec2( 0, 0 ) ),
    dimensions( glm::u32vec2( 320, 200 ) )
{
    status.window_status = Status::WINDOW;
}

Graphics::Window::~Window() {
    for( unsigned int i = 0; i < cameras.size(); i++ )
        delete cameras[ i ];
}

void Graphics::Window::setWindowTitle( const std::string &window_title ) {
    this->window_title = window_title;
}

void Graphics::Window::setPosition( glm::u32vec2 position ) {
    this->position = position;
}

int Graphics::Window::setDimensions( glm::u32vec2 dimensions ) {
    this->dimensions = dimensions;
    return 1;
}

void Graphics::Window::setFullScreen( bool is_full_screen ) {
    if( is_full_screen )
        status.window_status = Status::FULL_SCREEN;
    else
    if( status.window_status != Status::FULL_SCREEN )
        status.window_status = Status::WINDOW;
}
bool Graphics::Window::getFullScreen() const {
    return status.window_status == Status::FULL_SCREEN;
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
