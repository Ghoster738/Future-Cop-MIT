#include "Texture2D.h"

Graphics::SDL2::GLES2::Internal::Texture2D::Texture2D() : texture_id( 0 ), is_allocated( false )
{
}

Graphics::SDL2::GLES2::Internal::Texture2D::~Texture2D() {
    if( is_allocated ) {
        glDeleteTextures( 1, &texture_id );
    }
}

void Graphics::SDL2::GLES2::Internal::Texture2D::generate() {
    glGenTextures( 1, &texture_id );
    is_allocated = true;
}

void Graphics::SDL2::GLES2::Internal::Texture2D::setFilters( GLuint active_texture, GLint mag, GLint min, GLint wrap_s, GLint wrap_t ) {
    if( !is_allocated )
        generate();

    bind( active_texture );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t );
}

void Graphics::SDL2::GLES2::Internal::Texture2D::genChecker( GLuint active_texture, GLsizei width, GLsizei height, GLubyte pixel1[3], GLubyte pixel2[3] ) {
    GLubyte * checker_data_p = new GLubyte [ width * height * 3 ];

    bool flip = false; // used to flip the pixels to a checker board
    GLsizei sync = 0; // used to count to the width
    unsigned int offset; // used to for the pixel offsets.
    bool bounds_check; // On when sync is lower than the width.

    for( GLsizei x = 0; x < width * height; x++, sync++ ) {
        offset = x * 3;

        bounds_check = (sync < width);
        flip = bounds_check * flip | !bounds_check * !flip; // Flip the bit when
        sync = bounds_check * sync; // reset sync when the width hits

        // This choses between two colors depending on the flip bit
        checker_data_p[ offset + 0 ] = flip * pixel1[0] | !flip * pixel2[0];
        checker_data_p[ offset + 1 ] = flip * pixel1[1] | !flip * pixel2[1];
        checker_data_p[ offset + 2 ] = flip * pixel1[2] | !flip * pixel2[2];

        // Flip the bit when done.
        flip = !flip;
    }

    this->setImage( active_texture, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, checker_data_p );

    delete [] checker_data_p;
}

void Graphics::SDL2::GLES2::Internal::Texture2D::setImage( GLuint active_texture, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void * data_r ) {
    if( !is_allocated )
        generate();

    bind( active_texture );

    glTexImage2D( GL_TEXTURE_2D, level, internalformat, width, height, border, format, type, data_r );
}

void Graphics::SDL2::GLES2::Internal::Texture2D::updateImage( GLuint active_texture, GLint level, GLsizei width, GLsizei height, GLenum format, GLenum type, const void * data_r ) {
    bind( active_texture );

    glTexSubImage2D( GL_TEXTURE_2D, level, 0, 0, width, height, format, type, data_r );
}

void Graphics::SDL2::GLES2::Internal::Texture2D::bind( GLuint active_texture, GLint texture_uniform_id ) const {
    glActiveTexture( GL_TEXTURE0 + active_texture );
    glBindTexture( GL_TEXTURE_2D, texture_id );
    glUniform1i( texture_uniform_id, active_texture );
}

void Graphics::SDL2::GLES2::Internal::Texture2D::bind( GLuint active_texture ) const {
    glActiveTexture( GL_TEXTURE0 + active_texture );
    glBindTexture( GL_TEXTURE_2D, texture_id );
}
