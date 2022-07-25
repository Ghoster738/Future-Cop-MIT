#include "Text2DBuffer.h"

#include "Environment.h"
#include "SDL2/GLES2/Text2DBuffer.h"

Graphics::Text2DBuffer::Text2DBuffer( Environment &env ) : env_r( &env ) {
}

Graphics::Text2DBuffer* Graphics::Text2DBuffer::alloc( Environment &env_r ) {
    if( env_r.getEnvironmentIdentifier().compare( "OpenGL ES 2" ) == 0 ) {
        return new Graphics::SDL2::GLES2::Text2DBuffer( env_r );
    }
    else
        return nullptr;
}

Graphics::Text2DBuffer::~Text2DBuffer() {
}

int Graphics::Text2DBuffer::loadFonts( Environment &environment, const std::vector<Data::Mission::FontResource*> &fonts ) {
    if( environment.getEnvironmentIdentifier().compare( "OpenGL ES 2" ) == 0 ) {
        return Graphics::SDL2::GLES2::Text2DBuffer::loadFonts( environment, fonts );
    }
    else
        return -1;
}
