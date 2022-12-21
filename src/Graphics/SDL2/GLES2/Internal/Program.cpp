#include "Program.h"

Graphics::SDL2::GLES2::Internal::Program::Program() : is_allocated( false ), vertex_r( nullptr ), fragment_r( nullptr ), shader_program_id( 0 ) {
}

Graphics::SDL2::GLES2::Internal::Program::Program( Shader *vertex_r, Shader *fragment_r ) : is_allocated( false ), vertex_r( nullptr ), fragment_r( nullptr ), shader_program_id( 0 ) {
    allocate();
    setVertexShader( vertex_r );
    setFragmentShader( fragment_r );
    link();
}

Graphics::SDL2::GLES2::Internal::Program::~Program() {
    deallocate();
}

void Graphics::SDL2::GLES2::Internal::Program::allocate() {
    if( !is_allocated )
    {
        is_allocated = true;
        
        shader_program_id = glCreateProgram();
    }
}

void Graphics::SDL2::GLES2::Internal::Program::deallocate() {
    if( is_allocated )
        glDeleteProgram( shader_program_id );
    
    is_allocated = false;
    vertex_r   = nullptr;
    fragment_r = nullptr;
    shader_program_id = 0;
}

Graphics::SDL2::GLES2::Internal::Shader* Graphics::SDL2::GLES2::Internal::Program::setShader( Shader *new_shader_r, Shader *old_shader_r ) {
    if( old_shader_r != nullptr )
        glDetachShader( shader_program_id, old_shader_r->getShader() );
    
    glAttachShader( shader_program_id, new_shader_r->getShader() );
    
    return new_shader_r;
}

void Graphics::SDL2::GLES2::Internal::Program::setVertexShader( Graphics::SDL2::GLES2::Internal::Shader *new_vertex_r ) {
    vertex_r = setShader( new_vertex_r, vertex_r );
}


void Graphics::SDL2::GLES2::Internal::Program::setFragmentShader( Graphics::SDL2::GLES2::Internal::Shader *new_fragment_r  ) {
    fragment_r = setShader( new_fragment_r, fragment_r);
}

bool Graphics::SDL2::GLES2::Internal::Program::link() {
    GLint program_linked_status;
    
    glLinkProgram( shader_program_id );
    
    glGetProgramiv( shader_program_id, GL_LINK_STATUS, &program_linked_status);
    
    return (program_linked_status == 1);
}

std::string Graphics::SDL2::GLES2::Internal::Program::getInfoLog() const {
    std::string returnry;
    GLchar *temporary_string_p;
    GLint info_length;
    GLsizei actual_info_length;
    
    glGetProgramiv(shader_program_id, GL_INFO_LOG_LENGTH, &info_length);
    
    if( info_length > 0 )
    {
        temporary_string_p = new GLchar [ info_length ];
        
        if( temporary_string_p != nullptr )
        {
            returnry.reserve( info_length );
            
            glGetProgramInfoLog(shader_program_id, info_length, &actual_info_length, temporary_string_p );
            
            // TODO Remove this horiable code
            for( GLsizei a = 0; a < actual_info_length; a++ ) {
                returnry.push_back( temporary_string_p[ a ] );
            }
            
            delete [] temporary_string_p;
        }
        else
            returnry = "getInfoLog has ran out of memory?";
    }
    else
        returnry = "There is no error log. Hopefully, the shaders have the error log.";
    
    return returnry;
}

void Graphics::SDL2::GLES2::Internal::Program::use() {
    glUseProgram( shader_program_id );
}

GLuint Graphics::SDL2::GLES2::Internal::Program::getShaderID( Shader* shader_r ) const {
    if( shader_r != nullptr )
        return shader_r->getShader();
    else
        return 0;
}

GLuint Graphics::SDL2::GLES2::Internal::Program::getVertexShaderID() const {
    return getShaderID( vertex_r );
}

GLuint Graphics::SDL2::GLES2::Internal::Program::getFragmentShaderID() const {
    return getShaderID( fragment_r );
}
