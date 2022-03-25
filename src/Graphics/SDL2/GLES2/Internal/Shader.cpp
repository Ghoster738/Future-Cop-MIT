#include "Shader.h"

#include <fstream>

Graphics::SDL2::GLES2::Internal::Shader::Shader() : shader_type( EMPTY ), shader_id( 0 ) {
}

Graphics::SDL2::GLES2::Internal::Shader::Shader( TYPE type, const GLchar *const shader_source ) :
        shader_type( EMPTY ), // Set these values to the defaults just in case.
        shader_id( 0 )
{
    setShader( type, shader_source );
}

Graphics::SDL2::GLES2::Internal::Shader::~Shader() {
    // When the shader is being deleted set the shader itself to be deallocated.
    deallocate();
}

void Graphics::SDL2::GLES2::Internal::Shader::setShader( TYPE type, const GLchar *const shader_source ) {
    if( type != EMPTY )
    {
        // Deallocate the previous shader.
        deallocate();

        // Set the shader type.
        shader_type = type;

        // Create and compile the shader.
        shader_id = glCreateShader( type );
        glShaderSource( shader_id, 1, &shader_source, nullptr );
        glCompileShader( shader_id );
    }
}

int Graphics::SDL2::GLES2::Internal::Shader::loadShader( TYPE type, const char *const file_path ) {
    std::ifstream shader_file;
    shader_file.open( file_path, std::ios::out );

    if( shader_file )
    {
        // First we need the file size of the file.
        shader_file.seekg( 0, shader_file.end );
        const size_t FILE_SIZE = shader_file.tellg();
        shader_file.seekg( 0, shader_file.beg );

        // Then allocate the shader_buffer which will contain the shader code.
        GLchar *shader_buffer = new GLchar [ FILE_SIZE ];

        // Read the shader file's data and put it into the buffer.
        shader_file.read( shader_buffer, FILE_SIZE );

        // Set this shader with the shader buffer.
        setShader( type, shader_buffer );

        // DO NOT FORGET TO DELETE THE BUFFFER WHEN DONE.
        delete [] shader_buffer;

        // Return 1 for success.
        return 1;
    }
    else
        return 0; // The file is not found.
}

void Graphics::SDL2::GLES2::Internal::Shader::deallocate() {
    if( shader_type != EMPTY )
        glDeleteShader( shader_id );

    // Reset the Shader's state to empty.
    shader_type = EMPTY;
    shader_id = 0;
}
std::string Graphics::SDL2::GLES2::Internal::Shader::getInfoLog() const {
    std::string returnry;
    GLchar *temporary_string_p;
    GLint info_length;
    GLsizei actual_info_length;
    
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_length);
    
    if( info_length > 0 )
    {
        temporary_string_p = new GLchar [ info_length ];
        
        if( temporary_string_p != nullptr )
        {
            returnry.reserve( info_length );
            
            glGetShaderInfoLog(shader_id, info_length, &actual_info_length, temporary_string_p );
            
            for( GLsizei a = 0; a < actual_info_length; a++ ) {
                returnry.push_back( temporary_string_p[ a ] );
            }
            
            delete [] temporary_string_p;
        }
        else
            returnry = "shader's getInfoLog has ran out of memory?";
    }
    else
        returnry = "This shader has no info log.";
    
    return returnry;
}
