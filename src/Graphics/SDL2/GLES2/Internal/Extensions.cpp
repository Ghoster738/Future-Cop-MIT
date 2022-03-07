#include "Extensions.h"

#include <SDL2/SDL.h>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengles2.h>

#include <vector>
#include <string>
#include <algorithm>

namespace {
Graphics::SDL2::GLES2::Internal::Extensions extensions = Graphics::SDL2::GLES2::Internal::Extensions();

/**
 * This is a convience method used to count the number of extensions
 * from OpenGLES 2.0.
 * @param The whole list of extensions listed by OpenGLES 2.0
 * @return The number of extensions avialible.
 */
inline int getNumberOfExtensionCount( const GLubyte* extensions ) {
    int number_of_extensions = 0;
    const GLubyte* i = extensions;

    // Let us not assume that there are spaces at the beginning.

    // Count the amount of spaces between the words.
    // This will either get a number between (word_amount - 1) or
    // word_amount.
    for( i = extensions; *i != '\0'; i++ )
        number_of_extensions += (*i == ' ');

    // This counts just in case there is a word that is not ended with a space.
    if( i - extensions > 0 ) { // Bounds checking is always a good idea.
        // If a non space character is before the null terminator,
        // then increment number_of_extensions.
        number_of_extensions += (*(i - 1) != ' ');
    }

    return number_of_extensions;
}

Graphics::SDL2::GLES2::Internal::BITFIELD getBitField( const GLchar *const name, const std::vector<std::string> *const sorted_extensions ) {
    Graphics::SDL2::GLES2::Internal::BITFIELD bitfield = 0;
    std::string full_extention_name;

    full_extention_name = "GL_ARB_" + std::string( reinterpret_cast<const char *const>(name) );
    bitfield |= std::binary_search( sorted_extensions->begin(), sorted_extensions->end(), full_extention_name ) * Graphics::SDL2::GLES2::Internal::Extensions::ARB_BIT_FIELD;
    full_extention_name = "GL_EXT_" + std::string( reinterpret_cast<const char *const>(name) );
    bitfield |= std::binary_search( sorted_extensions->begin(), sorted_extensions->end(), full_extention_name ) * Graphics::SDL2::GLES2::Internal::Extensions::EXT_BIT_FIELD;
    full_extention_name = "GL_OES_" + std::string( reinterpret_cast<const char *const>(name) );
    bitfield |= std::binary_search( sorted_extensions->begin(), sorted_extensions->end(), full_extention_name ) * Graphics::SDL2::GLES2::Internal::Extensions::OES_BIT_FIELD;
    full_extention_name = "GL_MESA_" + std::string( reinterpret_cast<const char *const>(name) );
    bitfield |= std::binary_search( sorted_extensions->begin(), sorted_extensions->end(), full_extention_name ) * Graphics::SDL2::GLES2::Internal::Extensions::MESA_BIT_FIELD;
    full_extention_name = "GL_ATI_" + std::string( reinterpret_cast<const char *const>(name) );
    bitfield |= std::binary_search( sorted_extensions->begin(), sorted_extensions->end(), full_extention_name ) * Graphics::SDL2::GLES2::Internal::Extensions::ATI_BIT_FIELD;
    full_extention_name = "GL_NV_" + std::string( reinterpret_cast<const char *const>(name) );
    bitfield |= std::binary_search( sorted_extensions->begin(), sorted_extensions->end(), full_extention_name ) * Graphics::SDL2::GLES2::Internal::Extensions::NV_BIT_FIELD;
    full_extention_name = "GL_NVX_" + std::string( reinterpret_cast<const char *const>(name) );
    bitfield |= std::binary_search( sorted_extensions->begin(), sorted_extensions->end(), full_extention_name ) * Graphics::SDL2::GLES2::Internal::Extensions::NVX_BIT_FIELD;

    return bitfield;
}

}

Graphics::SDL2::GLES2::Internal::Extensions* Graphics::SDL2::GLES2::Internal::getGlobalExtension() {
    return &extensions;
}

const Graphics::SDL2::GLES2::Internal::BITFIELD Graphics::SDL2::GLES2::Internal::Extensions::ARB_BIT_FIELD   = 0b00000001;
const Graphics::SDL2::GLES2::Internal::BITFIELD Graphics::SDL2::GLES2::Internal::Extensions::EXT_BIT_FIELD   = 0b00000010;
const Graphics::SDL2::GLES2::Internal::BITFIELD Graphics::SDL2::GLES2::Internal::Extensions::OES_BIT_FIELD   = 0b00000100;
const Graphics::SDL2::GLES2::Internal::BITFIELD Graphics::SDL2::GLES2::Internal::Extensions::MESA_BIT_FIELD  = 0b00001000;
const Graphics::SDL2::GLES2::Internal::BITFIELD Graphics::SDL2::GLES2::Internal::Extensions::ATI_BIT_FIELD   = 0b00010000;
const Graphics::SDL2::GLES2::Internal::BITFIELD Graphics::SDL2::GLES2::Internal::Extensions::NV_BIT_FIELD    = 0b00100000;
const Graphics::SDL2::GLES2::Internal::BITFIELD Graphics::SDL2::GLES2::Internal::Extensions::NVX_BIT_FIELD   = 0b01000000;
const Graphics::SDL2::GLES2::Internal::BITFIELD Graphics::SDL2::GLES2::Internal::Extensions::OTHER_BIT_FIELD = 0b10000000;

Graphics::SDL2::GLES2::Internal::Extensions::Extensions() {
    is_set_up = false;
}

int Graphics::SDL2::GLES2::Internal::Extensions::loadAllExtensions() {
    // This clears every single state, and cannot be called twice.
    // So why reinvent the wheel?
    loadNoExtensions();

    // This holds the amount of extensions.
    std::vector<std::string> *extensions_array = new std::vector<std::string>;

    const GLubyte* extensions = glGetString( GL_EXTENSIONS );

    const int EXTENSIONS_AMOUNT = getNumberOfExtensionCount( extensions );

    extensions_array->reserve( EXTENSIONS_AMOUNT );

    int start = 0;
    int end = 0;

    for( int i = 0; i < EXTENSIONS_AMOUNT; i++ ) {
        for( ; extensions[ end ] != ' ' && extensions[ end ] != '\0'; end++ ) {}

        extensions_array->push_back( std::string( reinterpret_cast<const char*>(extensions + start), end - start ) );

        end++;
        start = end;
    }

    // Now sort all the extensions.
    std::sort( extensions_array->begin(), extensions_array->end() );

    // Finally, we can begin checking for extensions.

    // This counts the extensions that will be enabled.
    int extensions_used = 0;
    ES2_compatibility_extensions = getBitField( "ES2_compatibility_extensions", extensions_array );
    extensions_used += (ES2_compatibility_extensions != 0);
    vertex_array_object_extensions = getBitField( "vertex_array_object", extensions_array );
    extensions_used += (vertex_array_object_extensions != 0);

    // Extensions is not needed anymore.
    delete extensions_array;

    return extensions_used;
}

void Graphics::SDL2::GLES2::Internal::Extensions::loadNoExtensions() {
    // Make it so that this method must be called only once.
    if( is_set_up )
        throw DoubleExtensionLoadingCallException();

    // To make the error state valid.
    is_set_up = true;

    // Set every OpenGL extension to zero.
    vertex_array_object_extensions = 0;
}

int Graphics::SDL2::GLES2::Internal::Extensions::printAvailableExtensions( std::ostream &output ) {
    const GLubyte* extensions = glGetString( GL_EXTENSIONS );

    output << extensions << std::endl;

    return getNumberOfExtensionCount( extensions );
}
