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

namespace {
struct BlendModeCommand {
    GLenum es_modeRGB;
    GLenum es_modeAlpha;
    GLenum fs_srcRGB;
    GLenum fs_dstRGB;
    GLenum fs_srcAlpha;
    GLenum fs_dstAlpha;

    bool isEqual(const BlendModeCommand &last) const {
        if(es_modeRGB != last.es_modeRGB)
            return false;
        if(es_modeAlpha != last.es_modeAlpha)
            return false;
        if(fs_srcRGB != last.fs_srcRGB)
            return false;
        if(fs_dstRGB != last.fs_dstRGB)
            return false;
        if(fs_srcAlpha != last.fs_srcAlpha)
            return false;
        if(fs_dstAlpha != last.fs_dstAlpha)
            return false;
        return true;
    }
};

struct DrawTriangleCommand {
    BlendModeCommand blend_mode;
    uint32_t texture_id;

    size_t triangle_index;
    size_t triangle_count;
};

bool getDrawCommand(const Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::Triangle *const triangles_r, const size_t triangles_amount, DrawTriangleCommand &draw_command) {
    // If the triangle index is equal to the triangles amount then we are done.
    if(draw_command.triangle_index == triangles_amount)
        return false; // Return false there is no other drawing command. We are done.

    // In no circumstances should the triangle index exceed the amount of triangles.
    assert(draw_command.triangle_index < triangles_amount);

    const Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::Vertex &vertex = triangles_r[draw_command.triangle_index].vertices[0];

    auto current_polygon_type = static_cast<Graphics::RenderMode>( vertex.metadata.bitfield.polygon_type );

    // This affects the blending code. Redundent, but I would like my code to be expandable.
    switch( current_polygon_type ) {
        case Graphics::RenderMode::ADDITION:
            draw_command.blend_mode.es_modeRGB = GL_FUNC_ADD;
            draw_command.blend_mode.es_modeAlpha = GL_FUNC_ADD;
            draw_command.blend_mode.fs_srcRGB = GL_SRC_ALPHA;
            draw_command.blend_mode.fs_dstRGB = GL_ONE;
            draw_command.blend_mode.fs_srcAlpha = GL_ONE;
            draw_command.blend_mode.fs_dstAlpha = GL_ZERO;
            break;
        default:
        case Graphics::RenderMode::MIX:
            draw_command.blend_mode.es_modeRGB = GL_FUNC_ADD;
            draw_command.blend_mode.es_modeAlpha = GL_FUNC_ADD;
            draw_command.blend_mode.fs_srcRGB = GL_SRC_ALPHA;
            draw_command.blend_mode.fs_dstRGB = GL_ONE_MINUS_SRC_ALPHA;
            draw_command.blend_mode.fs_srcAlpha = GL_ONE;
            draw_command.blend_mode.fs_dstAlpha = GL_ZERO;
    };

    // Get the texture id.
    draw_command.texture_id = vertex.metadata.bitfield.texture_id;

    // This line of code is very important. It resets the triangle count.
    draw_command.triangle_count = 0;

    // Go through every triangle.
    while(draw_command.triangle_index + draw_command.triangle_count < triangles_amount) {

        // Increment the triangle count by one.
        draw_command.triangle_count++;

        const Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::Vertex &current_vertex = triangles_r[draw_command.triangle_index + draw_command.triangle_count].vertices[0];

        // Check if the drawing mode differs for each triangle.
        if(vertex.metadata.bitfield.texture_id != current_vertex.metadata.bitfield.texture_id)
            return true; // exit the loop and return true to tell that there might be another draw command.
        else
        if(vertex.metadata.bitfield.polygon_type != current_vertex.metadata.bitfield.polygon_type)
            return true; // exit the loop and return true to tell that there might be another draw command.
    }
    return true; // Return true to tell that there might be another draw command?
}
}

void Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::DrawCommand::draw( const VertexAttributeArray &vertex_array, const std::map<uint32_t, Graphics::SDL2::GLES2::Internal::Texture2D*> &textures, GLuint diffusive_texture_uniform_id ) const {
    // Do not bother rendering triangles if there are none.
    if( triangles_amount == 0 )
        return;

    // Bind the array buffer.
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);

    // Upload the triangle data to the array buffer.
    glBufferSubData( GL_ARRAY_BUFFER, 0, triangles_amount * sizeof(Triangle), triangles_p );

    // Bind the vertex array for the triangles.
    vertex_array.bind();

    DrawTriangleCommand draw_command, last_draw_command;

    draw_command.triangle_index = 0;

    // Loop until every draw command has been decoded.
    while(getDrawCommand(triangles_p, triangles_amount, draw_command)) {

        // Blend mode can affect the rendering.
        const BlendModeCommand &blend_mode = draw_command.blend_mode;

        // Set the blend method if the draw command is first or if blend method has changed.
        if(draw_command.triangle_index == 0 || !last_draw_command.blend_mode.isEqual(blend_mode)) {
            glBlendEquationSeparate(blend_mode.es_modeRGB, blend_mode.es_modeAlpha);
            glBlendFuncSeparate(blend_mode.fs_srcRGB, blend_mode.fs_dstRGB, blend_mode.fs_srcAlpha, blend_mode.fs_dstAlpha);
        }

        if(draw_command.triangle_index == 0 || draw_command.texture_id != last_draw_command.texture_id) {
            // Set the texture if it exists.
            auto current_texture_r = textures.find( draw_command.texture_id );
            if( current_texture_r != textures.end() ) {
                current_texture_r->second->bind( 0, diffusive_texture_uniform_id );
            }
        }

        // Draw the triangles.
        glDrawArrays( GL_TRIANGLES, 3 * draw_command.triangle_index, 3 * draw_command.triangle_count );

        last_draw_command = draw_command;

        // Increment the triangle index of the draw command. Unless you like infinite cycles.
        draw_command.triangle_index += draw_command.triangle_count;
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
    "  float BLENDING = 1.0 - other.a;\n"
    "  if( specular > 0.5 ) {\n"
    "    if( other.a < 0.5 )\n"
    "      other.a = 0.5;\n"
    "    gl_FragColor = vec4(texture2D(Shine, world_reflection.xz).rgb * BLENDING + other.rgb, other.a);\n"
    "  } else {\n"
    "    if( other.a < 0.015625 )\n"
    "      discard;\n"
    "    gl_FragColor = other;\n"
    "  }\n"
    "}\n";

void Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::Triangle::setup( uint32_t texture_id, const glm::vec3 &camera_position, Graphics::RenderMode poly_type ) {
    vertices[0].metadata.bitfield.texture_id = texture_id;
    vertices[0].metadata.bitfield.polygon_type = poly_type;
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

unsigned Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::Triangle::addStar(
    DynamicTriangleDraw::Triangle *draw_triangles_r, size_t number_of_triangles,
    const glm::vec3 &camera_position, const glm::mat4 &transform, const glm::vec3 &camera_right, const glm::vec3 &camera_up,
    const glm::vec3 &position, const glm::vec3 &color, float width, unsigned number_of_edges)
{
    if(number_of_triangles == 0)
        return 0;

    const float UNIT_45_DEGREES = 0.707106781187; // M_SQRT2 / 2.0;
    const float UNIT_30_DEGREES = 0.866025403785; // M_SQRT3 / 2.0;

    size_t index = 0;

    glm::vec2 circle_8[8] = { { 0, 1}, { UNIT_45_DEGREES, UNIT_45_DEGREES}, { 1, 0}, { UNIT_45_DEGREES,-UNIT_45_DEGREES}, { 0,-1}, {-UNIT_45_DEGREES,-UNIT_45_DEGREES}, {-1, 0}, {-UNIT_45_DEGREES, UNIT_45_DEGREES} };
    glm::vec2 circle_12[12] = { { 0, 1}, { 0.5, UNIT_30_DEGREES}, { UNIT_30_DEGREES, 0.5}, { 1, 0}, { UNIT_30_DEGREES,-0.5}, { 0.5,-UNIT_30_DEGREES}, { 0,-1}, {-0.5,-UNIT_30_DEGREES}, {-UNIT_30_DEGREES,-0.5}, {-1, 0}, {-UNIT_30_DEGREES, 0.5}, {-0.5, UNIT_30_DEGREES} };

    for(int x = 0; x < 3; x++) {
        draw_triangles_r[ index ].vertices[x].position   = position;
        draw_triangles_r[ index ].vertices[x].normal     = glm::vec3(0, 1, 0);
        draw_triangles_r[ index ].vertices[x].coordinate = glm::vec2(0, 0);
        draw_triangles_r[ index ].vertices[x].vertex_metadata = glm::i16vec2(0, 0);
    }
    draw_triangles_r[ index ].vertices[0].color = glm::vec4(color.x, color.y, color.z, 0.5) * 2.0f;
    draw_triangles_r[ index ].vertices[1].color = glm::vec4(color.x, color.y, color.z, 0.0) * 2.0f;
    draw_triangles_r[ index ].vertices[2].color = draw_triangles_r[ index ].vertices[1].color;

    draw_triangles_r[ index ].setup( 0, camera_position, Graphics::RenderMode::ADDITION );

    draw_triangles_r[ index ] = draw_triangles_r[ index ].addTriangle( camera_position, transform );

    if(number_of_edges == 4) {
        for(int t = 0; t < 4; t++) {
            if(t != 0)
                draw_triangles_r[ index ] = draw_triangles_r[ index - 1 ];

            const int cur_circle_index = t * 2;
            const int next_circle_index = ((t + 1) * 2) % 8;

            draw_triangles_r[ index ].vertices[1].position = draw_triangles_r[ index ].vertices[0].position + (camera_right * circle_8[next_circle_index].x * width) + (camera_up * circle_8[next_circle_index].y * width);
            draw_triangles_r[ index ].vertices[2].position = draw_triangles_r[ index ].vertices[0].position + (camera_right * circle_8[ cur_circle_index].x * width) + (camera_up * circle_8[ cur_circle_index].y * width);
            index++; index = std::min(number_of_triangles - 1, index);
        }
    }
    else if(number_of_edges == 8) {
        for(int t = 0; t < 8; t++) {
            if(t != 0)
                draw_triangles_r[ index ] = draw_triangles_r[ index - 1 ];

            const int cur_circle_index = t;
            const int next_circle_index = (t + 1) % 8;

            draw_triangles_r[ index ].vertices[1].position = draw_triangles_r[ index ].vertices[0].position + (camera_right * circle_8[next_circle_index].x * width) + (camera_up * circle_8[next_circle_index].y * width);
            draw_triangles_r[ index ].vertices[2].position = draw_triangles_r[ index ].vertices[0].position + (camera_right * circle_8[ cur_circle_index].x * width) + (camera_up * circle_8[ cur_circle_index].y * width);
            index++; index = std::min(number_of_triangles - 1, index);
        }
    }
    else {
        for(int t = 0; t < 12; t++) {
            if(t != 0)
                draw_triangles_r[ index ] = draw_triangles_r[ index - 1 ];

            const int cur_circle_index = t;
            const int next_circle_index = (t + 1) % 12;

            draw_triangles_r[ index ].vertices[1].position = draw_triangles_r[ index ].vertices[0].position + (camera_right * circle_12[next_circle_index].x * width) + (camera_up * circle_12[next_circle_index].y * width);
            draw_triangles_r[ index ].vertices[2].position = draw_triangles_r[ index ].vertices[0].position + (camera_right * circle_12[ cur_circle_index].x * width) + (camera_up * circle_12[ cur_circle_index].y * width);
            index++; index = std::min(number_of_triangles - 1, index);
        }
    }

    return index;
}
unsigned Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::Triangle::addBillboard(
    DynamicTriangleDraw::Triangle *draw_triangles_r, size_t number_of_triangles,
    const glm::vec3 &camera_position, const glm::mat4 &transform, const glm::vec3 &camera_right, const glm::vec3 &camera_up,
    const glm::vec3 &position, const glm::vec3 &param_color, float width,
    Graphics::RenderMode visability_mode, uint32_t bmp_id, const glm::vec2 (&coords)[4])
{
    const uint8_t QUAD_TABLE[2][3] = { {3, 2, 1}, {1, 0, 3}};
    const glm::vec2 QUAD[4] = {{-1.0f, 1.0f}, { 1.0f, 1.0f}, { 1.0f,-1.0f}, {-1.0f,-1.0f}};

    if(number_of_triangles == 0)
        return 0;

    size_t index = 0;

    glm::vec4 color = glm::vec4(param_color, 0.5) * 2.0f;

    for(int x = 0; x < 3; x++) {
        draw_triangles_r[ index ].vertices[x].position   = position;
        draw_triangles_r[ index ].vertices[x].normal     = glm::vec3(0, 1, 0);
        draw_triangles_r[ index ].vertices[x].color      = color;
        draw_triangles_r[ index ].vertices[x].vertex_metadata = glm::i16vec2(0, 0);
    }

    draw_triangles_r[ index ].setup( bmp_id, camera_position, visability_mode );
    draw_triangles_r[ index ] = draw_triangles_r[ index ].addTriangle( camera_position, transform );

    draw_triangles_r[ index + 1 ] = draw_triangles_r[ index ];

    for(int t = 0; t < 2; t++) {
        for(int x = 0; x < 3; x++) {
            draw_triangles_r[ index ].vertices[x].position += (camera_right * QUAD[QUAD_TABLE[t][x]].x * width) + (camera_up * QUAD[QUAD_TABLE[t][x]].y * width);

            draw_triangles_r[ index ].vertices[x].coordinate = coords[QUAD_TABLE[t][x]];
        }
        index++; index = std::min(number_of_triangles - 1, index);
    }

    return index;
}

Graphics::SDL2::GLES2::Internal::DynamicTriangleDraw::DynamicTriangleDraw() {
    vertex_array.addAttribute( "POSITION",   3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>( offsetof(Vertex, position) ) );
    vertex_array.addAttribute( "NORMAL",     3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>( offsetof(Vertex, normal) ) );
    vertex_array.addAttribute( "COLOR_0",    4, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>( offsetof(Vertex, color) ) );
    vertex_array.addAttribute( "TEXCOORD_0", 2, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>( offsetof(Vertex, coordinate) ) );
    vertex_array.addAttribute( "_METADATA",  2, GL_SHORT, false, sizeof(Vertex), reinterpret_cast<void*>( offsetof(Vertex, vertex_metadata) ) );

    attributes.push_back( Shader::Attribute( Shader::Type::HIGH, "vec3 POSITION" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,  "vec3 NORMAL" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,  "vec4 COLOR_0" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,  "vec2 TEXCOORD_0" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,  "vec2 _METADATA" ) );

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
