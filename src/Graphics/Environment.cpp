#include "Text2DBuffer.h"

#include "Environment.h"
#include "SDL2/GLES2/Environment.h"

namespace Graphics {

const std::string Environment::SDL2_WITH_GLES_2 = "OpenGL ES 2";

Environment::Environment() : window_p( nullptr ) {
}

Environment::~Environment() {
    // Close and destroy the window
    if( this->window_p != nullptr )
        delete this->window_p;
}

std::vector<std::string> Environment::getAvailableIdentifiers() {
    std::vector<std::string> identifiers;

    identifiers.push_back( SDL2_WITH_GLES_2 );

    return identifiers;
}

bool Environment::isIdentifier( const std::string &identifier ) {
    if( identifier.compare( SDL2_WITH_GLES_2 ) == 0 )
        return true;
    else
        return false;
}

Environment* Environment::alloc( const std::string &identifier ) {
    if( identifier.compare( SDL2_WITH_GLES_2 ) == 0 ) {
        return new SDL2::GLES2::Environment();
    }
    else
        return nullptr;
}

int Environment::initSystem( const std::string &identifier ) {
    if( identifier.compare( SDL2_WITH_GLES_2 ) == 0 ) {
        return SDL2::GLES2::Environment::initSystem();
    }
    else
        return -1;
}

int Environment::deinitEntireSystem( const std::string &identifier ) {
    if( identifier.compare( SDL2_WITH_GLES_2 ) == 0 ) {
        return SDL2::GLES2::Environment::deinitEntireSystem();
    }
    else
        return -1;
}

}
