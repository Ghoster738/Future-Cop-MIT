#include "Window.h" // Include the internal class
#include <iostream> // std::cout TODO Remove this.

#include "../../Environment.h"

Graphics::SDL2::GLES2::Window::Window( Environment &env ) : Graphics::SDL2::Window( env ), GL_context( 0 )
{
}

Graphics::SDL2::GLES2::Window::~Window() {
}

int Graphics::SDL2::GLES2::Window::attach() {
    Uint32 flags = 0;
    int major_version = 0;
    int success = -1;
    
    const int SDL2_CONTEXTS[ 2 ] = { SDL_GL_CONTEXT_PROFILE_ES, 0 };
    
    for( int i = 0; i < 2 && major_version != 2; i++ ) {
        // Deallocate the window if there
        if( window_p != nullptr )
            SDL_DestroyWindow( window_p );
        
        // Deallocate the context as well.
        if( GL_context != nullptr )
            SDL_GL_DeleteContext( GL_context );
        
        // Set them to nullptr.
        window_p = nullptr;
        GL_context = nullptr;
        
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL2_CONTEXTS[ i ] );
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        
        window_p = SDL_CreateWindow( getWindowTitle().c_str(),
                                    getPosition().x, getPosition().y,
                                    getDimensions().x, getDimensions().y,
                                    flags | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL );
        
        if( window_p != nullptr ) {
            GL_context = SDL_GL_CreateContext( window_p );
            
            int version = gladLoadGLES2((GLADloadfunc) SDL_GL_GetProcAddress);

            if( version != 0 ) {
                std::cout << "GLAD INIT Failure: ";
                // TODO Print out the problem if failure is detected.
            }
            else
                std::cout << "GLAD INIT Success: ";
            
            std::cout << std::endl;
        }
        else
        {
            std::cout << "SDL Window Error: " << SDL_GetError() << std::endl;
        }
        
        if( GL_context != nullptr ) {
            int make_current_status = SDL_GL_MakeCurrent( window_p, GL_context );
            
            if( make_current_status == 0 ) {
                SDL_GL_GetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, &major_version );
                
                if( major_version == 2 )
                    success = 1;
            }
            else
            {
                std::cout << "SDL Error: " << SDL_GetError() << std::endl;
            }
        }
        else
        {
            std::cout << "Context Allocation Failure!\n";
            std::cout << "SDL Error: " << SDL_GetError() << std::endl;
        }
    }
    
    env_r->window_p = this;
    
    if( success < 0 ) {
        major_version = 0;
        
        std::cout << "Failed to initialize OpenGLES2\n";
        SDL_GL_GetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, &major_version );
        std::cout << "  SDL_GL_CONTEXT_MAJOR_VERSION = " << major_version << "\n";
        SDL_GL_GetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, &major_version );
        std::cout << "  SDL_GL_CONTEXT_MINOR_VERSION = " << major_version << "\n";
        SDL_GL_GetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, &major_version );
        std::cout << "  SDL_GL_CONTEXT_PROFILE_MASK ID = " << major_version << "\n";

        if( (major_version & SDL_GL_CONTEXT_PROFILE_ES) != 0 )
            std::cout << "  SDL_GL_CONTEXT_PROFILE_MASK = SDL_GL_CONTEXT_PROFILE_ES\n";
        if( (major_version & SDL_GL_CONTEXT_PROFILE_CORE) != 0 )
            std::cout << "  SDL_GL_CONTEXT_PROFILE_MASK = SDL_GL_CONTEXT_PROFILE_CORE\n";
        if( (major_version & SDL_GL_CONTEXT_PROFILE_COMPATIBILITY) != 0 )
            std::cout << "  SDL_GL_CONTEXT_PROFILE_MASK = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY\n";
        std::cout << std::endl;
    }
    
    return success;
}
