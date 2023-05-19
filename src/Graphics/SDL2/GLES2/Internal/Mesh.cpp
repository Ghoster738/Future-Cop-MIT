#include "Mesh.h"
#include <cassert>
#include <iostream>

Graphics::SDL2::GLES2::Internal::Mesh::Mesh( Program *program_r ) {
    this->program_r = program_r;
    draw_command_array_mode = GL_TRIANGLES;
    morph_frame_amount = 0;
}

Graphics::SDL2::GLES2::Internal::Mesh::~Mesh() {
    glDeleteBuffers( 1, &vertex_buffer_object );
}

void Graphics::SDL2::GLES2::Internal::Mesh::addCommand( GLint first, GLsizei opeque_count, GLsizei count, const Texture2D *texture_r ) {
    draw_command.push_back( DrawCommand() );
    draw_command.back().first = first;
    draw_command.back().opeque_count = opeque_count;
    draw_command.back().count = count;
    draw_command.back().texture_r = texture_r;
}

void Graphics::SDL2::GLES2::Internal::Mesh::setup( Utilities::ModelBuilder &model, const std::map<uint32_t, Internal::Texture2D*>& textures ) {
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

    GLsizei transparent_count = 0;

    for( unsigned int a = 0; a < model.getNumMaterials(); a++ ) {
        model.getMaterial( a, material );

        Internal::Texture2D *texture_2d_r = nullptr;
        
        if( textures.find( material.cbmp_resource_id ) != textures.end() )
            texture_2d_r = const_cast<Internal::Texture2D *>( textures.at( material.cbmp_resource_id ) );
        else if( !textures.empty() ) {
            texture_2d_r = const_cast<Internal::Texture2D *>( textures.begin()->second );
        }

        GLsizei opeque_count = std::min( material.count, material.opeque_count );

        transparent_count += material.count - material.opeque_count;

        addCommand( material.starting_vertex_index, opeque_count, material.count, texture_2d_r );
    }

    transparent_triangles.reserve( transparent_count );

    GLsizei material_count = 0;

    for( unsigned int a = 0; a < model.getNumMaterials(); a++ ) {
        model.getMaterial( a, material );

        uint32_t cbmp_id;

        if( textures.find( material.cbmp_resource_id ) != textures.end() )
            cbmp_id = material.cbmp_resource_id;
        else if( !textures.empty() ) {
            cbmp_id = textures.begin()->first;
        }
        else
            cbmp_id = 0;

        GLsizei opeque_count = std::min( material.count, material.opeque_count );

        glm::vec4   positions[3] = {glm::vec4(0, 0, 0, 1)};
        glm::vec4      colors[3] = {glm::vec4(0, 0, 0, 1)};
        glm::vec4 coordinates[3] = {glm::vec4(0, 0, 0, 1)};

        // TODO This needs to be dynamic. There should not be assumptions. Cobj might not work if this is not fixed.
        const auto   position_compenent_index = 0;
        const auto      color_compenent_index = 2;
        const auto coordinate_compenent_index = 3;
        const unsigned vertex_per_triangle = 3;

        for( GLsizei i = opeque_count; i < material.count; i += vertex_per_triangle ) {
            DynamicTriangleDraw::Triangle triangle;

            model.getTransformation(   positions[0],   position_compenent_index, material_count + i + 0 );
            model.getTransformation(      colors[0],      color_compenent_index, material_count + i + 0 );
            model.getTransformation( coordinates[0], coordinate_compenent_index, material_count + i + 0 );
            model.getTransformation(   positions[1],   position_compenent_index, material_count + i + 1 );
            model.getTransformation(      colors[1],      color_compenent_index, material_count + i + 1 );
            model.getTransformation( coordinates[1], coordinate_compenent_index, material_count + i + 1 );
            model.getTransformation(   positions[2],   position_compenent_index, material_count + i + 2 );
            model.getTransformation(      colors[2],      color_compenent_index, material_count + i + 2 );
            model.getTransformation( coordinates[2], coordinate_compenent_index, material_count + i + 2 );

            triangle.vertices[0].position = { positions[0].x, positions[0].y, positions[0].z };
            triangle.vertices[0].color = 2.0f * colors[0];
            triangle.vertices[0].color.w = 1;
            triangle.vertices[0].coordinate = coordinates[0];
            triangle.vertices[1].position = { positions[1].x, positions[1].y, positions[1].z };
            triangle.vertices[1].color = 2.0f * colors[1];
            triangle.vertices[1].color.w = 1;
            triangle.vertices[1].coordinate = coordinates[1];
            triangle.vertices[2].position = { positions[2].x, positions[2].y, positions[2].z };
            triangle.vertices[2].color = 2.0f * colors[2];
            triangle.vertices[2].color.w = 1;
            triangle.vertices[2].coordinate = coordinates[2];

            triangle.setup( cbmp_id, glm::vec3(0, 0, 0) );

            transparent_triangles.push_back( triangle );
        }

        material_count += material.count;
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

   auto  err = glGetError();

    if( err != GL_NO_ERROR )
        std::cout << "This vertex_array has a problem " << err << std::endl;
}

void Graphics::SDL2::GLES2::Internal::Mesh::noPreBindDraw( GLuint active_switch_texture, GLuint texture_switch_uniform ) const {
    for( auto i = draw_command.begin(); i < draw_command.end(); i++ )
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
    for( auto i = draw_command.begin(); i < draw_command.end(); i++ )
    {
        if( (*i).texture_r != nullptr )
            (*i).texture_r->bind( active_switch_texture, texture_switch_uniform );

        glDrawArrays( draw_command_array_mode, (*i).first, (*i).opeque_count );
        auto err = glGetError();

        if( err != GL_NO_ERROR )
            std::cout << "glDrawArrays could have a problem! " << err << std::endl;
    }
}

void Graphics::SDL2::GLES2::Internal::Mesh::noPreBindDrawTransparent( GLuint active_switch_texture, GLuint texture_switch_uniform ) const {
    for( auto i = draw_command.begin(); i < draw_command.end(); i++ )
    {
        if( (*i).texture_r != nullptr )
            (*i).texture_r->bind( active_switch_texture, texture_switch_uniform );

        glDrawArrays( draw_command_array_mode, (*i).first + (*i).opeque_count, (*i).count - (*i).opeque_count );
        auto err = glGetError();

        if( err != GL_NO_ERROR )
            std::cout << "glDrawArrays could have a problem! " << err << std::endl;
    }
}

void Graphics::SDL2::GLES2::Internal::Mesh::addTransparentTriangles( const glm::vec3 &camera_position, DynamicTriangleDraw &triangles_draw ) const {
    for( auto current : transparent_triangles ) {
        DynamicTriangleDraw::Triangle *draw_triangle_r = triangles_draw.getTriangle();

        if( draw_triangle_r != nullptr ) {
            *draw_triangle_r = current.addTriangle( camera_position );
        }
        else
            break;
    }
}

void Graphics::SDL2::GLES2::Internal::Mesh::addTransparentTriangles( const glm::vec3 &camera_position, const glm::mat4 &matrix, DynamicTriangleDraw &triangles_draw ) const {
    for( auto current : transparent_triangles ) {
        DynamicTriangleDraw::Triangle *draw_triangle_r = triangles_draw.getTriangle();

        if( draw_triangle_r != nullptr ) {
            *draw_triangle_r = current.addTriangle( camera_position, matrix );
        }
        else
            break;
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
