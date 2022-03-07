#include "Program.h"

Graphics::SDL2::GLES2::Internal::Program::Program() : is_allocated( false ), vertex_ref( nullptr ), fragment_ref( nullptr ), shader_program( 0 ) {
}

Graphics::SDL2::GLES2::Internal::Program::Program( Shader *vertex_reference, Shader *fragment_reference ) : is_allocated( false ), vertex_ref( nullptr ), fragment_ref( nullptr ), shader_program( 0 ) {
    allocate();
    setVertexShader( vertex_reference );
    setFragmentShader( fragment_reference );
    link();
}

Graphics::SDL2::GLES2::Internal::Program::~Program() {
    deallocate();
}

void Graphics::SDL2::GLES2::Internal::Program::allocate() {
    if( !is_allocated )
    {
        is_allocated = true;
        
        shader_program = glCreateProgram();
    }
}

void Graphics::SDL2::GLES2::Internal::Program::deallocate() {
    if( is_allocated )
        glDeleteProgram( shader_program );
    
    is_allocated = false;
    vertex_ref   = nullptr;
    fragment_ref = nullptr;
    shader_program = 0;
}

Graphics::SDL2::GLES2::Internal::Shader* Graphics::SDL2::GLES2::Internal::Program::setShader( Shader *newShader, Shader *oldShader ) {
    if( oldShader != nullptr )
        glDetachShader( shader_program, oldShader->getShader() );
    
    glAttachShader( shader_program, newShader->getShader() );
    
    return newShader;
}

void Graphics::SDL2::GLES2::Internal::Program::setVertexShader( Graphics::SDL2::GLES2::Internal::Shader *new_vertex_reference ) {
    vertex_ref = setShader( new_vertex_reference, vertex_ref );
}


void Graphics::SDL2::GLES2::Internal::Program::setFragmentShader( Graphics::SDL2::GLES2::Internal::Shader *new_fragment_reference  ) {
    fragment_ref = setShader( new_fragment_reference, fragment_ref );
}

void Graphics::SDL2::GLES2::Internal::Program::link() {
    glLinkProgram( shader_program );
}

void Graphics::SDL2::GLES2::Internal::Program::use() {
    glUseProgram( shader_program );
}

GLuint Graphics::SDL2::GLES2::Internal::Program::getShaderID( Shader* shader_id ) const {
    if( shader_id != nullptr )
        return shader_id->getShader();
    else
        return 0;
}

GLuint Graphics::SDL2::GLES2::Internal::Program::getVertexShaderID() const {
    return getShaderID( vertex_ref );
}

GLuint Graphics::SDL2::GLES2::Internal::Program::getFragmentShaderID() const {
    return getShaderID( fragment_ref );
}
