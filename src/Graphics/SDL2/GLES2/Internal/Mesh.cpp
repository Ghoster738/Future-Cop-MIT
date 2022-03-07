#include "Mesh.h"
#include <cassert>
#include <iostream>

Graphics::SDL2::GLES2::Internal::Mesh::Mesh( Program *program ) {
    this->programR = program;
    draw_command_array_mode = GL_TRIANGLES;
    morph_frame_amount = 0;
}

Graphics::SDL2::GLES2::Internal::Mesh::~Mesh() {
    glDeleteBuffers( 1, &vertex_buffer_object );
}

void Graphics::SDL2::GLES2::Internal::Mesh::addCommand( GLint first, GLsizei count, const Texture2D *texture_ref ) {
    draw_command.push_back( DrawCommand() );
    draw_command.back().first = first;
    draw_command.back().count = count;
    draw_command.back().texture_ref = texture_ref;
}

void Graphics::SDL2::GLES2::Internal::Mesh::setup( Utilities::ModelBuilder &model, const std::vector<Texture2D*> textures ) {
    void * vertex_buffer_data = model.getBuffer( vertex_buffer_size );
    
    morph_buffer_size = 0;
    model.getMorphBuffer( 0, morph_buffer_size );

    // Create a Vertex Buffer Object
    glGenBuffers(1, &vertex_buffer_object);
    glBindBuffer( GL_ARRAY_BUFFER, vertex_buffer_object);
    
    morph_frame_amount = model.getNumMorphFrames();
    
    if( morph_frame_amount == 0 )
    {
        // Generate the buffer
        glBufferData( GL_ARRAY_BUFFER, vertex_buffer_size, vertex_buffer_data, GL_STATIC_DRAW );
    }
    else
    {
        frame_amount = morph_frame_amount;
        
        size_t buffer_size = vertex_buffer_size + morph_buffer_size * morph_frame_amount;
        uint8_t *buffer = new uint8_t[ buffer_size ];
        
        for( size_t i = 0; i < vertex_buffer_size; i++ ) {
            buffer[ i ] = reinterpret_cast<uint8_t*>( vertex_buffer_data )[ i ];
        }
        
        size_t buffer_offset = vertex_buffer_size;
        
        for( unsigned int i = 0; i < morph_frame_amount; i++ ) {
            unsigned int morph_size;
            
            uint8_t *morph_buff = reinterpret_cast<uint8_t*>( model.getMorphBuffer( i, morph_size ) );
            for( size_t f = 0; f < morph_buffer_size; f++ ) {
                buffer[ buffer_offset++ ] = morph_buff[ f ];
            }
        }
        
        glBufferData( GL_ARRAY_BUFFER, buffer_size, buffer, GL_STATIC_DRAW );
        
        // Wait until the buffer is done uploading.
        glFinish();
        
        delete [] buffer;
    }

    vertex_array.getAttributesFrom( *programR, model );
    
    vertex_array.allocate( *programR );
    
    // If there is some kind of bug where there are some attributes not recognized
    // then cull them. They will just spam Mesh with countless errors.
    vertex_array.cullUnfound( &std::cout );

    Utilities::ModelBuilder::TextureMaterial material;

    for( unsigned int a = 0; a < model.getNumMaterials(); a++ ) {
        model.getMaterial( a, material );

        // The mod operation ensures that there is no out of bound case.
        unsigned int texture_index = material.texture_index % textures.size();
        if( texture_index == -1 )
            texture_index = 0;

        addCommand( material.starting_vertex_index, material.count, textures[ texture_index ] );
    }
}

void Graphics::SDL2::GLES2::Internal::Mesh::draw( GLuint active_switch_texture, GLuint texture_switch_uniform ) const {
    bindArray();
    noPreBindDraw( active_switch_texture, texture_switch_uniform );
}

void Graphics::SDL2::GLES2::Internal::Mesh::bindArray() const {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    vertex_array.bind();

    auto err = glGetError();

    if( err != GL_NO_ERROR )
        std::cout << "This vertex_array has a problem " << err << std::endl;
}

void Graphics::SDL2::GLES2::Internal::Mesh::noPreBindDraw( GLuint active_switch_texture, GLuint texture_switch_uniform ) const {
    for( auto i = draw_command.begin(); i < draw_command.end(); i++ )
    {
        if( (*i).texture_ref != nullptr )
            (*i).texture_ref->bind( active_switch_texture, texture_switch_uniform );

        glDrawArrays( draw_command_array_mode, (*i).first, (*i).count );
        auto err = glGetError();

        if( err != GL_NO_ERROR )
            std::cout << "glDrawArrays could have a problem! " << err << std::endl;
    }
}

size_t Graphics::SDL2::GLES2::Internal::Mesh::getMorphOffset( unsigned int morph_frame_index ) const {
    return vertex_buffer_size + morph_buffer_size * morph_frame_index;
}
