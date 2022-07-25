#include "Window.h" // Include the internal class
#include "Internal/Extensions.h"
#include <iostream> // std::cout TODO Remove this.

#include "../../Environment.h"

Graphics::SDL2::GLES2::Window::Window( Environment &env ) : Graphics::SDL2::Window( env ), GL_context( 0 )
{
}

Graphics::SDL2::GLES2::Window::~Window() {
}

int Graphics::SDL2::GLES2::Window::attach() {
    Uint32 flags = 0;
    
    if( env_r->window_p == nullptr ) {
        // TODO modify variable flags ask for what kind of window to allocate.
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        #ifndef GRAPHICS_GLES2_EXCLUDE_CONTEXT_PROFILE
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, GLES2_SDL_GL_CONTEXT_SETTING);
        #endif
        
        SDL_GL_SetSwapInterval(0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        
        window_p = SDL_CreateWindow( getWindowTitle().c_str(),
            getPosition().x, getPosition().y,
            getDimensions().x, getDimensions().y,
            flags | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL );
        
        int major_version = 0;
        SDL_GL_GetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, &major_version );
        
        if( window_p == nullptr || major_version != 2 ) {
            std::cout << "Failed to allocate OpenGLES context. Using normal context." << std::endl;
            
            if( window_p != nullptr )
                SDL_DestroyWindow( window_p );
            window_p = nullptr;
            
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,  0);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,          1);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,            24);
            
            window_p = SDL_CreateWindow( getWindowTitle().c_str(),
                getPosition().x, getPosition().y,
                getDimensions().x, getDimensions().y,
                flags | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL );
            
            SDL_GL_GetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, &major_version );
        }
        
        if( window_p != nullptr )
        {
            GL_context = SDL_GL_CreateContext( window_p );
            
            #ifdef GLEW_FOUND
            auto glew_status = glewInit();
            
            if( glew_status != GLEW_OK ) {
                std::cout << "GLEW INIT Failure: " << glewGetErrorString( glew_status ) << std::endl;
            }
            else {
                std::cout << "GLEW INIT Success: " << glewGetErrorString( glew_status ) << std::endl;
            }
            #endif
            
            GLenum err;
            while((err = glGetError()) != GL_NO_ERROR)
            {
                std::cout << "GL_ERROR 351: 0x" << std::hex << err << std::dec << std::endl;
            }
            
            if( GL_context != nullptr )
            {
                SDL_GL_MakeCurrent( window_p, GL_context );
                SDL_GL_SetSwapInterval(0);
            
                std::cout << "OpenGL version is " << glGetString( GL_VERSION ) << std::endl;
                
                // TODO check for the reason why GLES2 needs this? Am I doing something wrong?
                #ifndef FORCE_FULL_OPENGL_2
                SDL_CreateRenderer( window_p, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE );
                #endif
                
                while((err = glGetError()) != GL_NO_ERROR)
                {
                    std::cout << "GL_ERROR 363: 0x" << std::hex << err << std::dec << std::endl;
                }

                // TODO Find a way for the user to turn the extensions off by command parameter.
                if( !Graphics::SDL2::GLES2::Internal::getGlobalExtension()->hasBeenLoaded() )
                {
                    while((err = glGetError()) != GL_NO_ERROR)
                    {
                        std::cout << "GL_ERROR 371: 0x" << std::hex << err << std::dec << std::endl;
                    }
                    
                    auto number = Graphics::SDL2::GLES2::Internal::getGlobalExtension()->loadAllExtensions();
                    
                    if( number < 0 )
                    {
                        while((err = glGetError()) != GL_NO_ERROR)
                        {
                            std::cout << "GL_ERROR 380: 0x" << std::hex << err << std::dec << std::endl;
                        }
                    }

                    std::cout << "Number of Extensions is " << number << std::endl;
                    std::cout << "The available ones are " << std::endl;
                    number = Graphics::SDL2::GLES2::Internal::Extensions::printAvailableExtensions( std::cout );
                    std::cout << std::endl << "Which is " << number << std::endl;

                    std::cout << std::hex << "Vertex binding is 0x" << Graphics::SDL2::GLES2::Internal::getGlobalExtension()->vertexArrayBindingStatus() << std::dec << std::endl;
                }
                
                env_r->window_p = this;
                
                return 1;
            }
            else
            {
                std::cout << "SDL_GL_CreateContext ERROR: " << SDL_GetError() << std::endl;
                return -2;
            }
        }
        else
        {
            std::cout << "SDL_CreateWindow ERROR: " << SDL_GetError() << std::endl;
            return -1;
        }
    }
    else
        return 0;
}
