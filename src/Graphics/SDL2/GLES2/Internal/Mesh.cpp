#include "Mesh.h"
#include <cassert>
#include <iostream>

Graphics::SDL2::GLES2::Internal::Mesh::DynamicTriangleTransform::~DynamicTriangleTransform() {
}

void Graphics::SDL2::GLES2::Internal::Mesh::DynamicNormal::addTriangles( const std::vector<DynamicTriangleDraw::Triangle> &triangles, DynamicTriangleDraw::DrawCommand &triangles_draw ) const {
    DynamicTriangleDraw::Triangle *draw_triangles_r;

    size_t number_of_triangles = triangles_draw.getTriangles( triangles.size(), &draw_triangles_r );

    for( size_t i = 0; i < number_of_triangles; i++ ) {
        draw_triangles_r[ i ] = triangles[ i ].addTriangle( this->camera_position, transform );
    }
}

Graphics::SDL2::GLES2::Internal::Mesh::Mesh( Program *program_r ) {
    this->program_r = program_r;
    draw_command_array_mode = GL_TRIANGLES;
    morph_frame_amount = 0;
}

Graphics::SDL2::GLES2::Internal::Mesh::~Mesh() {
    glDeleteBuffers( 1, &vertex_buffer_object );
}

void Graphics::SDL2::GLES2::Internal::Mesh::addCommand( GLint first, GLsizei opaque_count, GLsizei count, const Texture2D *texture_r ) {
    draw_command.push_back( DrawCommand() );
    draw_command.back().first = first;
    draw_command.back().count = count;
    draw_command.back().opaque_count = opaque_count;
    draw_command.back().texture_r = texture_r;
}

void Graphics::SDL2::GLES2::Internal::Mesh::setup( Utilities::ModelBuilder &model, const std::map<uint32_t, Internal::Texture2D*>& textures, const VertexAttributeArray *const default_vertex_array ) {
    switch(model.getPrimativeMode()) {
        case Utilities::ModelBuilder::MeshPrimativeMode::POINTS:
            draw_command_array_mode = GL_POINTS;
            break;
        case Utilities::ModelBuilder::MeshPrimativeMode::LINES:
            draw_command_array_mode = GL_LINES;
            break;
        case Utilities::ModelBuilder::MeshPrimativeMode::LINE_LOOP:
            draw_command_array_mode = GL_LINE_LOOP;
            break;
        case Utilities::ModelBuilder::MeshPrimativeMode::LINE_STRIP:
            draw_command_array_mode = GL_LINE_STRIP;
            break;
        case Utilities::ModelBuilder::MeshPrimativeMode::TRIANGLES:
            draw_command_array_mode = GL_TRIANGLES;
            break;
        case Utilities::ModelBuilder::MeshPrimativeMode::TRIANGLE_STRIP:
            draw_command_array_mode = GL_TRIANGLE_STRIP;
            break;
        case Utilities::ModelBuilder::MeshPrimativeMode::TRIANGLE_FAN:
            draw_command_array_mode = GL_TRIANGLE_FAN;
            break;
    }

    void * vertex_buffer_data = model.getBuffer( vertex_buffer_size );
    bool bounds;
    
    bounds = model.getBoundingSphere( this->culling_sphere_position, this->culling_sphere_radius );
    
    assert( bounds );
    
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

    vertex_array.getAttributesFrom( *program_r, model );
    
    vertex_array.allocate( *program_r );
    
    // If there is some kind of bug where there are some attributes not recognized
    // then cull them. They will just spam Mesh with countless errors.
    vertex_array.cullUnfound();

    Utilities::ModelBuilder::TextureMaterial material;

    for( unsigned int a = 0; a < model.getNumMaterials(); a++ ) {
        model.getMaterial( a, material );

        Internal::Texture2D *texture_2d_r = nullptr;
        
        if( textures.find( material.cbmp_resource_id ) != textures.end() )
            texture_2d_r = const_cast<Internal::Texture2D *>( textures.at( material.cbmp_resource_id ) );
        else if( !textures.empty() ) {
            texture_2d_r = const_cast<Internal::Texture2D *>( textures.begin()->second );
        }

        GLsizei addition_index = std::min( material.count, material.addition_index );
        GLsizei mix_index = std::min( material.count, material.mix_index );
        GLsizei opaque_count = std::min(addition_index, mix_index);

        addCommand( material.starting_vertex_index, opaque_count, material.count, texture_2d_r );
    }
}

void Graphics::SDL2::GLES2::Internal::Mesh::draw( GLuint active_switch_texture, GLuint texture_switch_uniform ) const {
    bindArray();
    noPreBindDraw( active_switch_texture, texture_switch_uniform );
}

void Graphics::SDL2::GLES2::Internal::Mesh::drawOpaque( GLuint active_switch_texture, GLuint texture_switch_uniform ) const {
    bindArray();
    noPreBindDrawOpaque( active_switch_texture, texture_switch_uniform );
}

void Graphics::SDL2::GLES2::Internal::Mesh::drawTransparent( GLuint active_switch_texture, GLuint texture_switch_uniform ) const {
    bindArray();
    noPreBindDrawTransparent( active_switch_texture, texture_switch_uniform );
}

void Graphics::SDL2::GLES2::Internal::Mesh::bindArray() const {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    vertex_array.bind();

   auto err = glGetError();

    if( err != GL_NO_ERROR )
        std::cout << "This vertex_array has a problem " << err << std::endl;
}

void Graphics::SDL2::GLES2::Internal::Mesh::noPreBindDraw( GLuint active_switch_texture, GLuint texture_switch_uniform ) const {
    for( auto i = draw_command.begin(); i != draw_command.end(); i++ )
    {
        if( (*i).texture_r != nullptr )
            (*i).texture_r->bind( active_switch_texture, texture_switch_uniform );

        glDrawArrays( draw_command_array_mode, (*i).first, (*i).count );
        auto err = glGetError();

        if( err != GL_NO_ERROR )
            std::cout << "glDrawArrays could have a problem! " << err << std::endl;
    }
}

void Graphics::SDL2::GLES2::Internal::Mesh::noPreBindDrawOpaque( GLuint active_switch_texture, GLuint texture_switch_uniform ) const {
    for( auto i = draw_command.begin(); i != draw_command.end(); i++ )
    {
        if( (*i).texture_r != nullptr )
            (*i).texture_r->bind( active_switch_texture, texture_switch_uniform );

        glDrawArrays( draw_command_array_mode, (*i).first, (*i).opaque_count );
        auto err = glGetError();
        assert( (*i).texture_r != nullptr );

        if( err != GL_NO_ERROR )
            std::cout << "glDrawArrays could have a problem! " << err << std::endl;
    }
}

void Graphics::SDL2::GLES2::Internal::Mesh::noPreBindDrawTransparent( GLuint active_switch_texture, GLuint texture_switch_uniform ) const {
    for( auto i = draw_command.begin(); i != draw_command.end(); i++ )
    {
        if( (*i).texture_r != nullptr )
            (*i).texture_r->bind( active_switch_texture, texture_switch_uniform );

        glDrawArrays( draw_command_array_mode, (*i).first + (*i).opaque_count, (*i).count - (*i).opaque_count );
        auto err = glGetError();

        if( err != GL_NO_ERROR )
            std::cout << "glDrawArrays could have a problem! " << err << std::endl;
    }
}

size_t Graphics::SDL2::GLES2::Internal::Mesh::getMorphOffset( unsigned int morph_frame_index ) const {
    return vertex_buffer_size + morph_buffer_size * morph_frame_index;
}

bool Graphics::SDL2::GLES2::Internal::Mesh::getBoundingSphere( glm::vec3 &position, float &radius ) const {
    position = this->culling_sphere_position;
    radius   = this->culling_sphere_radius;
    return true;
}
