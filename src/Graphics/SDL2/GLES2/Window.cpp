#include "Window.h" // Include the internal class
#include "Internal/Extensions.h"
#include <iostream> // std::cout TODO Remove this.

#include "../../Environment.h"

Graphics::SDL2::GLES2::Window::Window( Environment &env ) : Graphics::SDL2::Window( env ), GL_context( 0 )
{
}

Graphics::SDL2::GLES2::Window::~Window() {
}

/*
 Uint32 flags = 0;
 int major_version = 0;
 int success = -1;
 
 while( major_version != 2 ) {
     if( env_r->window_p != nullptr )
         SDL_DestroyWindow( window_p );
     if( GL_context != nullptr )
         SDL_GL_DeleteContext( GL_context );
     env_r->window_p = nullptr;
     GL_context = nullptr;
     
     SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
     SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
     SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, 0);
     SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
     SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
     
     window_p = SDL_CreateWindow( getWindowTitle().c_str(),
                                 getPosition().x, getPosition().y,
                                 getDimensions().x, getDimensions().y,
                                 flags | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL );
     
     if( window_p != nullptr )
         GL_context = SDL_GL_CreateContext( window_p );
     else
     {
         std::cout << "Window Allocation Failure!\n";
         GLenum err;
         while((err = glGetError()) != GL_NO_ERROR)
         {
             std::cout << "GL_ERROR: 0x" << std::hex << err << std::dec << "\n";
         }
         std::cout << std::endl;
     }
     
     if( GL_context != nullptr ) {
         int make_current_status = SDL_GL_MakeCurrent( window_p, GL_context );
         
         if( make_current_status == 0 ) {
             major_version = SDL_GL_GetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, &major_version );
             std::cout << "major_version = " << major_version << std::endl;
             assert( major_version == 2 );
         }
         else
         {
             std::cout << "SDL Error: 0x" << SDL_GetError() << std::endl;
         }
     }
     else
     {
         std::cout << "Context Allocation Failure!\n";
         GLenum err;
         do
         {
             err = glGetError();
             std::cout << "GL_ERROR: 0x" << std::hex << err << std::dec << "\n";
         }
         while( err != GL_NO_ERROR );
         std::cout << std::endl;
     }
     
     if( major_version == 2 ) {
         success = 1;
         env_r->window_p = this;
     }
     
     major_version = 2;
 }
 
 major_version = 0;
 
 if( success < 0 ) {
     std::cout << "Failed to initialize OpenGLES2\n";
     SDL_GL_GetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, &major_version );
     std::cout << "  SDL_GL_CONTEXT_MAJOR_VERSION = " << major_version << "\n";
     SDL_GL_GetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, &major_version );
     std::cout << "  SDL_GL_CONTEXT_MINOR_VERSION = " << major_version << "\n";
     SDL_GL_GetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, &major_version );
     std::cout << "  SDL_GL_CONTEXT_PROFILE_MASK = " << major_version << std::endl;
 }
 
 if( !Graphics::SDL2::GLES2::Internal::getGlobalExtension()->hasBeenLoaded() )
 {
     GLenum err;
     while((err = glGetError()) != GL_NO_ERROR) {
         std::cout << "GL_ERROR 371: 0x" << std::hex << err << std::dec << std::endl;
     }
     auto number = Graphics::SDL2::GLES2::Internal::getGlobalExtension()->loadAllExtensions();
     {
         while((err = glGetError()) != GL_NO_ERROR)  {
             std::cout << "GL_ERROR 380: 0x" << std::hex << err << std::dec << std::endl;
         }
     }
     std::cout << "Number of Extensions is " << number << std::endl;
     std::cout << "The available ones are " << std::endl;
     number = Graphics::SDL2::GLES2::Internal::Extensions::printAvailableExtensions( std::cout );
     std::cout << std::endl << "Which is " << number << std::endl;
     std::cout << std::hex << "Vertex binding is 0x" << Graphics::SDL2::GLES2::Internal::getGlobalExtension()->vertexArrayBindingStatus() << std::dec << std::endl;
 }
 
 return success;
 */

int Graphics::SDL2::GLES2::Window::attach() {
    Uint32 flags = 0;
    int major_version = 0;
    int success = -1;
    
    const int SDL2_CONTEXTS[ 2 ] = { SDL_GL_CONTEXT_PROFILE_ES, 0 };
    
    for( int i = 0; i < 2 && major_version != 2; i++ ) {
        // Deallocate the window if there
        if( env_r->window_p != nullptr )
            SDL_DestroyWindow( window_p );
        
        // Deallocate the context as well.
        if( GL_context != nullptr )
            SDL_GL_DeleteContext( GL_context );
        
        // Set them to nullptr.
        env_r->window_p = nullptr;
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
        
        if( window_p != nullptr )
            GL_context = SDL_GL_CreateContext( window_p );
        else
        {
            std::cout << "SDL Error: " << SDL_GetError() << std::endl;
        }
        
        if( GL_context != nullptr ) {
            int make_current_status = SDL_GL_MakeCurrent( window_p, GL_context );
            
            glFinish();
            
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
        std::cout << "  SDL_GL_CONTEXT_PROFILE_MASK = " << major_version << std::endl;
    }
    
    if( !Graphics::SDL2::GLES2::Internal::getGlobalExtension()->hasBeenLoaded() )
    {
        GLenum err;
        while((err = glGetError()) != GL_NO_ERROR) {
            std::cout << "GL_ERROR 371: 0x" << std::hex << err << std::dec << std::endl;
        }
        auto number = Graphics::SDL2::GLES2::Internal::getGlobalExtension()->loadAllExtensions();
        {
            while((err = glGetError()) != GL_NO_ERROR)  {
                std::cout << "GL_ERROR 380: 0x" << std::hex << err << std::dec << std::endl;
            }
        }
        std::cout << "Number of Extensions is " << number << std::endl;
        std::cout << "The available ones are " << std::endl;
        number = Graphics::SDL2::GLES2::Internal::Extensions::printAvailableExtensions( std::cout );
        std::cout << std::endl << "Which is " << number << std::endl;
        std::cout << std::hex << "Vertex binding is 0x" << Graphics::SDL2::GLES2::Internal::getGlobalExtension()->vertexArrayBindingStatus() << std::dec << std::endl;
    }
    
    return success;
}
