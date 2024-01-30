#include "DynamicTriangleDraw.h"
#include <algorithm>
#include <cassert>
#include <cmath> // fmod()
#include <iostream> // fmod()
#include <glm/mat4x4.hpp>
#include "SDL.h"

#include "../Camera.h"

Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::DrawCommand::DrawCommand() {
    triangles_p = nullptr;
}

size_t Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::DrawCommand::allocateBuffer( size_t limit ) {
    GLint size = 0;

    if( limit == 0 )
        return 0;

    deleteBuffer();

    glGenBuffers(1, &vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, limit * sizeof(Triangle), nullptr, GL_DYNAMIC_DRAW);

    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    if( size < static_cast<ssize_t>( sizeof(Triangle) ) ) {
        return 0;
    }

    triangles_max = size / sizeof(Triangle);
    triangles_amount = 0;
    triangles_p = new Triangle [triangles_max];
    // glBufferSubData(GL_ARRAY_BUFFER, 0, limit * sizeof(Triangle), transparent_triangles_p);

    return triangles_max;
}

void Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::DrawCommand::deleteBuffer() {
    if( triangles_p != nullptr ) {
        glDeleteBuffers(1, &vertex_buffer_object);

        delete [] triangles_p;
    }
    triangles_p = nullptr;
    triangles_max = 0;
    triangles_amount = 0;
    vertex_buffer_object = 0;
}

void Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::DrawCommand::reset() {
    triangles_amount = 0;
}

size_t Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::DrawCommand::getTriangles( size_t number_of_triangles, Triangle** triangles_r ) {
    if( triangles_p == nullptr && triangles_amount >= triangles_max ) {
        *triangles_r = nullptr;
        return 0;
    }

    *triangles_r = &triangles_p[ triangles_amount ];

    size_t clipped_triangle_number = number_of_triangles - std::min( number_of_triangles, triangles_max - triangles_amount );

    triangles_amount += number_of_triangles - clipped_triangle_number;

    // If the number of triangles exceeds the normal size then crash.
    // A segmentation fault would occur in this case anyways.
    assert( triangles_max >= triangles_amount );

    return number_of_triangles - clipped_triangle_number;
}

namespace {

// This method sorts the traingles.
bool compare( Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::Triangle a, Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::Triangle b ) {
    return a.vertices[1].metadata.distance_from_camera > b.vertices[1].metadata.distance_from_camera;
}

}

void Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::DrawCommand::sortTriangles() {
    // Sort the trinagles from back to front for the transparent triangles.
    std::sort( triangles_p, triangles_p + triangles_amount, compare );
}

void Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::DrawCommand::draw( const VertexAttributeArray &vertex_array, const std::map<uint32_t, Graphics::SDL2::GLES2::Internal::Texture2D*> &textures, GLuint diffusive_texture_uniform_id ) const {
    // Finally, we can draw the triangles.
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    glBufferSubData( GL_ARRAY_BUFFER, 0, triangles_amount * sizeof(Triangle), triangles_p );
    vertex_array.bind();

    uint32_t texture_id = 0;
    Graphics::SDL2::GLES2::Internal::Texture2D* current_texture_r = nullptr;
    if( textures.size() != 0 ) {
        texture_id = textures.begin()->first;
        current_texture_r = textures.begin()->second;

        current_texture_r->bind( 0, diffusive_texture_uniform_id );
    }

    size_t t_last = 0;

    for( size_t t = 0; t < triangles_amount; t++ ) {
        if( texture_id != triangles_p[t].vertices[0].metadata.bitfield.texture_id ) {
            texture_id = triangles_p[t].vertices[0].metadata.bitfield.texture_id;

            auto item = textures.find( texture_id );

            if( item != textures.end() ) {
                current_texture_r = item->second;

                if( t_last != t ) {
                    glDrawArrays( GL_TRIANGLES, t_last * 3, (t - t_last) * 3 );

                    t_last = t;
                }

                current_texture_r->bind( 0, diffusive_texture_uniform_id );
            }
        }
    }

    if( t_last < triangles_amount ) {
        glDrawArrays( GL_TRIANGLES, t_last * 3, (triangles_amount - t_last) * 3 );
    }
}

const GLchar* Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::default_vertex_shader =
    // Vertex shader uniforms
    "uniform mat4 ModelViewInv;\n"
    "uniform mat4 ModelView;\n"
    "uniform mat4 Transform;\n" // projection * view * model.

    "void main()\n"
    "{\n"
    // This reflection code is based on https://stackoverflow.com/questions/27619078/reflection-mapping-in-opengl-es
    "   vec3 eye_coord_position = vec3( ModelView * vec4(POSITION, 1) );\n" // Model View multiplied by Model Position.
    "   vec3 eye_coord_normal   = vec3( ModelView * vec4(NORMAL, 0.0));\n"
    "   eye_coord_normal        = normalize( eye_coord_normal );\n"
    "   vec3 eye_reflection     = reflect( eye_coord_position, eye_coord_normal);\n"
    // Find a way to use the spherical projection properly.
    "   world_reflection        = vec3( ModelViewInv * vec4(eye_reflection, 0.0 ));\n"
    "   world_reflection        = normalize( world_reflection ) * 0.5 + vec3( 0.5, 0.5, 0.5 );\n"
    "   texture_coord_1 = TEXCOORD_0;\n"
    "   color = COLOR_0;\n"
    "   specular = _METADATA[0];\n"
    "   gl_Position = Transform * vec4(POSITION.xyz, 1.0);\n"
    "}\n";
const GLchar* Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::default_fragment_shader =
    "uniform sampler2D Texture;\n"
    "uniform sampler2D Shine;\n"

    "void main()\n"
    "{\n"
    "  vec4 other = texture2D(Texture, texture_coord_1) * color;\n"
    "  if( other.a < 0.015625 )\n"
    "    discard;\n"
    "  float BLENDING = 1.0 - other.a;\n"
    "  if( specular > 0.5 )\n"
    "    gl_FragColor = vec4(texture2D(Shine, world_reflection.xz).rgb * BLENDING + other.rgb, other.a);\n"
    "  else\n"
    "    gl_FragColor = other;\n"
    "}\n";

void Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::Triangle::setup( uint32_t texture_id, const glm::vec3 &camera_position ) {
    vertices[0].metadata.bitfield.texture_id = texture_id;
    vertices[1].metadata.distance_from_camera = genDistanceSq( camera_position );
}


float Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::Triangle::genDistanceSq( const glm::vec3 &camera_position ) const {
    glm::vec3 combine = vertices[0].position + vertices[1].position + vertices[2].position;
    combine *= 1.0 / 3.0;

    glm::vec3 status = combine - camera_position;

    return status.x * status.x + status.y * status.y + status.z * status.z;
}

Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::Triangle Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::Triangle::addTriangle( const glm::vec3 &camera_position ) const {
    Triangle triangle = *this;

    triangle.vertices[1].metadata.distance_from_camera = triangle.genDistanceSq( camera_position );

    return triangle;
}

Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::Triangle Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::Triangle::addTriangle( const glm::vec3 &camera_position, const glm::mat4 &matrix ) const {
    Triangle triangle = *this;

    for( unsigned i = 0; i < 3; i++ ) {
        const auto position = matrix * glm::vec4( triangle.vertices[i].position, 1 );
        const auto normal   = matrix * glm::vec4( triangle.vertices[i].normal,   0 );

        const auto scale = 1.0f / position.w;

        triangle.vertices[i].position = glm::vec3( position.x, position.y, position.z ) * scale;
        triangle.vertices[i].normal   = glm::vec3(   normal.x,   normal.y,   normal.z );
    }

    triangle.vertices[1].metadata.distance_from_camera = triangle.genDistanceSq( camera_position );

    return triangle;
}

Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::DynamicTriangleDraw() {
    vertex_array.addAttribute( "POSITION",   3, GL_FLOAT,         false, sizeof(Vertex), reinterpret_cast<void*>( offsetof(Vertex, position) ) );
    vertex_array.addAttribute( "NORMAL",     3, GL_FLOAT,         false, sizeof(Vertex), reinterpret_cast<void*>( offsetof(Vertex, normal) ) );
    vertex_array.addAttribute( "COLOR_0",    4, GL_FLOAT,         false, sizeof(Vertex), reinterpret_cast<void*>( offsetof(Vertex, color) ) );
    vertex_array.addAttribute( "TEXCOORD_0", 2, GL_FLOAT,         false, sizeof(Vertex), reinterpret_cast<void*>( offsetof(Vertex, coordinate) ) );
    vertex_array.addAttribute( "_METADATA",  4, GL_UNSIGNED_BYTE,  true, sizeof(Vertex), reinterpret_cast<void*>( offsetof(Vertex, vertex_metadata) ) );

    attributes.push_back( Shader::Attribute( Shader::Type::MEDIUM, "vec3 POSITION" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "vec3 NORMAL" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "vec4 COLOR_0" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "vec2 TEXCOORD_0" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "vec4 _METADATA" ) );

    varyings.push_back( Shader::Varying( Shader::Type::LOW, "vec2 texture_coord_1" ) );
    varyings.push_back( Shader::Varying( Shader::Type::LOW, "vec3 world_reflection" ) );
    varyings.push_back( Shader::Varying( Shader::Type::MEDIUM, "float specular" ) );
    varyings.push_back( Shader::Varying( Shader::Type::LOW, "vec4 color" ) );
}

Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::~DynamicTriangleDraw() {
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
            specular_texture_uniform_id = program.getUniform( "Shine", &std::cout, &uniform_failed );
            matrix_uniform_id = program.getUniform( "Transform", &std::cout, &uniform_failed );
            view_uniform_id = program.getUniform( "ModelView", &std::cout, &uniform_failed );
            view_inv_uniform_id = program.getUniform( "ModelViewInv", &std::cout, &uniform_failed );

            attribute_failed |= !program.isAttribute( "POSITION", &std::cout );
            attribute_failed |= !program.isAttribute( "NORMAL", &std::cout );
            attribute_failed |= !program.isAttribute( "COLOR_0", &std::cout );
            attribute_failed |= !program.isAttribute( "TEXCOORD_0", &std::cout );
            attribute_failed |= !program.isAttribute( "_METADATA", &std::cout );

            vertex_array.allocate( program );
            vertex_array.cullUnfound( &std::cout );

            link_success = true;
        }

        if( !link_success || uniform_failed || attribute_failed ) {
            std::cout << "DynamicTriangleDraw program has failed." << std::endl;

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

void Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::setEnvironmentTexture( Texture2D *env_texture_ref ) {
    this->env_texture_r = env_texture_ref;
}

void Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::draw( Graphics::SDL2::GLES2::Camera &camera, const std::map<uint32_t, Graphics::SDL2::GLES2::Internal::Texture2D*> &textures ) {
    if( camera.transparent_triangles.triangles_amount == 0 )
        return; // There is no semi-transparent triangle to draw.

    glm::mat4 camera_3D_projection_view; // This holds the camera transform along with the view.
    glm::mat4 model_view;
    glm::mat4 model_view_inv;

    camera.getProjectionView3D( camera_3D_projection_view ); // camera_3D_projection_view = current_camera 3D matrix.

    // Use the static shader for the static models.
    program.use();

    // Check if there is even a shiney texture.
    if( env_texture_r != nullptr )
        env_texture_r->bind( 1, specular_texture_uniform_id );

    camera.getView3D( model_view );
    model_view_inv = glm::inverse( model_view );
    glUniformMatrix4fv( view_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &model_view[0][0] ) );
    glUniformMatrix4fv( view_inv_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &model_view_inv[0][0] ) );

    // We can now send the matrix to the program.
    glUniformMatrix4fv( matrix_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &camera_3D_projection_view[0][0] ) );

    camera.transparent_triangles.sortTriangles();
    camera.transparent_triangles.draw( vertex_array, textures, diffusive_texture_uniform_id );
}
