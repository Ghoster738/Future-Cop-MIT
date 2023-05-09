#include "DynamicTriangleDraw.h"
#include <algorithm>
#include <cassert>
#include <cmath> // fmod()
#include <iostream> // fmod()
#include <glm/mat4x4.hpp>
#include "SDL.h"

const GLchar* Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::default_vertex_shader =
    // Vertex shader uniforms
    "uniform mat4 Transform;\n" // projection * view * model.

    "void main()\n"
    "{\n"
    "   texture_coord_1 = TEXCOORD_0;\n"
    "   gl_Position = Transform * vec4(POSITION.xyz, 1.0);\n"
    "}\n";
const GLchar* Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::default_fragment_shader =
    "uniform sampler2D Texture;\n"

    "void main()\n"
    "{\n"
    "  vec4 color = texture2D(Texture, texture_coord_1);\n"
    "   if( color.a < 0.015625 )\n"
    "       discard;\n"
    "   gl_FragColor = color;\n"
    "}\n";

void Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::Triangle::setDistance( const glm::vec3 &camera_position ) {
    glm::vec3 combine = vertices[0].position + vertices[1].position + vertices[2].position;
    combine *= 1.0 / 3.0;

    glm::vec3 status = combine - camera_position;

    vertices[1].metadata.distance_from_camera = status.x * status.x + status.y * status.y + status.z * status.z;
}

void Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::deleteTriangles() {
    if( transparent_triangles_p != nullptr ) {
        glDeleteBuffers(1, &vertex_buffer_object);

        delete [] transparent_triangles_p;
    }
    transparent_triangles_p = nullptr;
    transparent_triangles_max = 0;
    transparent_triangles_amount = 0;
    vertex_buffer_object = 0;
}

Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::DynamicTriangleDraw() {
    deleteTriangles();

    vertex_array.addAttribute( "POSITION",   3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>( offsetof(Vertex, position) ) );
    vertex_array.addAttribute( "COLOR_0",    4, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>( offsetof(Vertex, color) ) );
    vertex_array.addAttribute( "TEXCOORD_0", 2, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>( offsetof(Vertex, coordinate) ) );

    attributes.push_back( Shader::Attribute( Shader::Type::MEDIUM, "vec4 POSITION" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "vec3 COLOR_0" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "vec2 TEXCOORD_0" ) );

    varyings.push_back( Shader::Varying( Shader::Type::LOW, "vec2 texture_coord_1" ) );
}

Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::~DynamicTriangleDraw() {
    deleteTriangles();
}

const GLchar* Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::getDefaultVertexShader() {
    return default_vertex_shader;
}

const GLchar* Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::getDefaultFragmentShader() {
    return default_fragment_shader;
}

void Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::setVertexShader( const GLchar *const shader_source ) {
    vertex_shader.setShader( Shader::TYPE::VERTEX, shader_source, attributes, varyings );
}

int Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::loadVertexShader( const char *const file_path ) {
    return vertex_shader.loadShader( Shader::TYPE::VERTEX, file_path );
}

void Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::setFragmentShader( const GLchar *const shader_source ) {
    fragment_shader.setShader( Shader::TYPE::FRAGMENT, shader_source, {}, varyings  );
}

int Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::loadFragmentShader( const char *const file_path ) {
    return fragment_shader.loadShader( Shader::TYPE::FRAGMENT, file_path );
}

int Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::compileProgram() {
    bool uniform_failed = false;
    bool attribute_failed = false;
    bool link_success = true;

    // The two shaders should be allocated first.
    if( vertex_shader.getType() == Shader::TYPE::VERTEX && fragment_shader.getType() == Shader::TYPE::FRAGMENT ) {

        // Allocate the opengl program for the map.
        program.allocate();

        // Give the program these two shaders.
        program.setVertexShader( &vertex_shader );
        program.setFragmentShader( &fragment_shader );

        // Attempt to link the shader
        if( !program.link() )
            link_success = false;
        else
        {
            // Setup the uniforms for the map.
            diffusive_texture_uniform_id = program.getUniform( "Texture", &std::cout, &uniform_failed );
            matrix_uniform_id = program.getUniform( "Transform", &std::cout, &uniform_failed );

            attribute_failed |= !program.isAttribute( "POSITION", &std::cout );
            attribute_failed |= !program.isAttribute( "NORMAL", &std::cout );
            attribute_failed |= !program.isAttribute( "TEXCOORD_0", &std::cout );

            link_success = true;
        }

        if( !link_success || uniform_failed || attribute_failed ) {
            std::cout << "StaticModelDraw program has failed." << std::endl;

            if( !link_success )
                std::cout << "There is trouble with linking." << std::endl;
            if( uniform_failed )
                std::cout << "There is trouble with the uniforms." << std::endl;
            if( attribute_failed )
                std::cout << "There is trouble with the attributes." << std::endl;

            std::cout << program.getInfoLog();
            std::cout << "\nVertex shader log\n";
            std::cout << vertex_shader.getInfoLog();
            std::cout << "\nFragment shader log\n";
            std::cout << fragment_shader.getInfoLog() << std::endl;
        }

        return link_success;
    }
    else
    {
        return 0; // Not every shader was loaded.
    }
}

int Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::allocateTriangles( size_t limit ) {
    GLint size = 0;

    if( limit == 0 )
        return false;

    deleteTriangles();

    glGenBuffers(1, &vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, limit, nullptr, GL_DYNAMIC_DRAW);

    transparent_triangles_max = limit;
    transparent_triangles_p = new Triangle [limit];

    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    if( size == 0 ) {
        deleteTriangles();
        return false;
    }
    else
        return true;
}

void Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::addTriangle( const Triangle& triangle, const glm::vec3 &camera_position ) {
    if( transparent_triangles_p == nullptr )
        return;

    if( transparent_triangles_amount >= transparent_triangles_max )
        return;

    transparent_triangles_p[transparent_triangles_amount] = triangle;
    transparent_triangles_p[transparent_triangles_amount].setDistance( camera_position );

    transparent_triangles_amount++;
}

namespace {

bool compare( Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::Triangle a, Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::Triangle b ) {
    return a.vertices[1].metadata.distance_from_camera > b.vertices[1].metadata.distance_from_camera;
}

}

void Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::draw( const Graphics::Camera &camera, const std::map<uint32_t, Graphics::SDL2::GLES2::Internal::Texture2D*> &textures ) {
    if( transparent_triangles_amount == 0 )
        return; // There is no semi-transparent triangle to draw.

    glm::mat4 camera_3D_projection_view; // This holds the camera transform along with the view.

    camera.getProjectionView3D( camera_3D_projection_view ); // camera_3D_projection_view = current_camera 3D matrix.

    // Use the static shader for the static models.
    program.use();

    // We can now send the matrix to the program.
    glUniformMatrix4fv( matrix_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &camera_3D_projection_view[0][0] ) );

    // Sort the trinagles
    std::sort( transparent_triangles_p, transparent_triangles_p + transparent_triangles_amount, compare );

    // Finally we can draw the triangles.
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    vertex_array.bind();

    uint32_t texture_id = 0;
    Graphics::SDL2::GLES2::Internal::Texture2D* current_texture_r = nullptr;
    if( textures.size() != 0 ) {
        texture_id = textures.begin()->first;
        current_texture_r = textures.begin()->second;
    }

    size_t t_last = 0;

    for( size_t t = 0; t < transparent_triangles_amount; t++ ) {
        if( texture_id != transparent_triangles_p[t].vertices[0].metadata.bitfield.texture_id ) {
            texture_id = transparent_triangles_p[t].vertices[0].metadata.bitfield.texture_id;

            auto item = textures.find( texture_id );

            if( item != textures.end() ) {
                current_texture_r = item->second;

                if( t_last != t ) {
                    glBufferSubData( GL_ARRAY_BUFFER, t_last * sizeof(Triangle), (t - t_last) * sizeof(Triangle), &transparent_triangles_p[t_last] );
                    glDrawArrays( GL_TRIANGLES, t_last, (t - t_last) * 3 );

                    t_last = t;
                }

                current_texture_r->bind( 0, diffusive_texture_uniform_id );
            }
        }
    }
}
