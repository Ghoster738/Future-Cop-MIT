#include "Text2DBuffer.h"

#include "Environment.h"
#include "SDL2/GLES2/Text2DBuffer.h"

Graphics::Text2DBuffer::Text2DBuffer() {
}

Graphics::Text2DBuffer* Graphics::Text2DBuffer::alloc( Graphics::Environment &env_r ) {
    if( env_r.getEnvironmentIdentifier().compare( "OpenGL ES 2" ) == 0 ) {
        return new Graphics::SDL2::GLES2::Text2DBuffer( env_r );
    }
    else
        return nullptr;
}

Graphics::Text2DBuffer::~Text2DBuffer() {
}

int Graphics::Text2DBuffer::loadFonts( Graphics::Environment &env_r, const std::vector<Data::Mission::IFF*> &data_r ) {
    if( env_r.getEnvironmentIdentifier().compare( "OpenGL ES 2" ) == 0 ) {
        return Graphics::SDL2::GLES2::Text2DBuffer::loadFonts( env_r, data_r );
    }
    else
        return -1;
}
