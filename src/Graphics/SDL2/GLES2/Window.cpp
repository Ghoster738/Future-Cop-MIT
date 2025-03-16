#include "Window.h" // Include the internal class

#include "Environment.h"

namespace Graphics::SDL2::GLES2 {

Graphics::Window* Environment::allocateWindow() {
    if(this->window_p != nullptr)
        return this->window_p;

    auto window_p = new Graphics::SDL2::GLES2::Window( *this );

    window_p->prioritize_opengl_2_fallback = this->force_gl2;

    this->window_p = window_p;

    return window_p;
}

Window::Window( Graphics::Environment &env ) : Graphics::SDL2::Window( env ), GL_context( 0 ) {}

Window::~Window() {}

int Window::attach() {
    auto error_log = Utilities::logger.getLog( Utilities::Logger::ERROR );
    error_log.info << "GLES 2 Graphics Window::attach\n";
    auto info_log = Utilities::logger.getLog( Utilities::Logger::INFO );

    int major_version = 0;
    int success = -1;
    
    const std::string CONTEXT_NAMES[ 2 ] = { "OpenGLES", "OpenGL" };
    const int SDL2_CONTEXTS[ 2 ] = { SDL_GL_CONTEXT_PROFILE_ES, 0 };
    
    for( int i = 0; i < 2 && major_version != 2; i++ ) {
        int mirror_index;

        if(!prioritize_opengl_2_fallback)
            mirror_index = i;
        else
            mirror_index = 1 - i;

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
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL2_CONTEXTS[ mirror_index ] );
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        
        // This simple logic is there to determine if the window is centered or not.
        auto position = getPosition();
        
        if( this->is_centered )
            position = glm::u32vec2( SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED );
        
        window_p = SDL_CreateWindow( getWindowTitle().c_str(),
                                    getPosition().x, getPosition().y,
                                    getDimensions().x, getDimensions().y,
                                    flags | SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL );
        
        if( window_p != nullptr ) {
            GL_context = SDL_GL_CreateContext( window_p );
            
            int version = gladLoadGLES2((GLADloadfunc) SDL_GL_GetProcAddress);

            if( version == 0 ) {
                error_log.output << "GLAD INIT Failure for " << CONTEXT_NAMES[ mirror_index ] << "\n";
                // TODO Print out the problem if failure is detected.
            }
            else
                info_log.output << "GLAD INIT Success for " << CONTEXT_NAMES[ mirror_index ] << "\n";
        }
        else
        {
            error_log.output << "SDL Window Error: " << SDL_GetError() << " for " << CONTEXT_NAMES[ mirror_index ] << "\n";
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
                error_log.output << "SDL Context Status Error: " << SDL_GetError() << " for " << CONTEXT_NAMES[ mirror_index ] << "\n";
            }
        }
        else
        {
            error_log.output << "Context Allocation Failure!\n";
            error_log.output << "SDL Error: " << SDL_GetError() << " for " << CONTEXT_NAMES[ mirror_index ] << "\n";
        }
    }
    
    if( window_p != nullptr )
        SDL_ShowWindow( window_p );
    
    if( success < 0 ) {
        major_version = 0;
        
        error_log.output << "Failed to initialize OpenGLES2\n";
        SDL_GL_GetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, &major_version );
        error_log.output << "  SDL_GL_CONTEXT_MAJOR_VERSION = " << major_version << "\n";
        SDL_GL_GetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, &major_version );
        error_log.output << "  SDL_GL_CONTEXT_MINOR_VERSION = " << major_version << "\n";
        SDL_GL_GetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, &major_version );
        error_log.output << "  SDL_GL_CONTEXT_PROFILE_MASK ID = " << major_version << "\n";

        if( (major_version & SDL_GL_CONTEXT_PROFILE_ES) != 0 )
            error_log.output << "  SDL_GL_CONTEXT_PROFILE_MASK = SDL_GL_CONTEXT_PROFILE_ES\n";
        if( (major_version & SDL_GL_CONTEXT_PROFILE_CORE) != 0 )
            error_log.output << "  SDL_GL_CONTEXT_PROFILE_MASK = SDL_GL_CONTEXT_PROFILE_CORE\n";
        if( (major_version & SDL_GL_CONTEXT_PROFILE_COMPATIBILITY) != 0 )
            error_log.output << "  SDL_GL_CONTEXT_PROFILE_MASK = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY\n";
        error_log.output << "\n";
    }
    
    return success;
}

}
