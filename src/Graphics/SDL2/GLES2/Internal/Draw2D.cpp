#include "Draw2D.h"

#include <iostream>

namespace Graphics::SDL2::GLES2::Internal {

const GLchar* Draw2D::default_vertex_shader =
    // Vertex shader uniforms
    "uniform mat4  Transform;\n" // projection * view * model.
    "void main()\n"
    "{\n"
    "   texture_coord = TEXCOORD_0;\n"
    "   vertex_color = COLOR_0;\n"
    "   gl_Position = Transform * vec4(POSITION.xy, 0.0, 1.0);\n"
    "}\n";

const GLchar* Draw2D::default_fragment_shader =
    "uniform sampler2D Texture;\n"
    "void main()\n"
    "{\n"
    "    float visable = texture2D(Texture, texture_coord).r;\n"
    "    if( visable < 0.03125 )\n"
    "        discard;\n"
    "    gl_FragColor = vertex_color;\n"
    "}\n";

const GLchar* Draw2D::getDefaultVertexShader() {
    return default_vertex_shader;
}

int Draw2D::loadVertexShader( const char *const file_path ) {
    return vertex_shader.loadShader( Shader::TYPE::VERTEX, file_path );
}

const GLchar* Draw2D::getDefaultFragmentShader() {
    return default_fragment_shader;
}

int Draw2D::loadFragmentShader( const char *const file_path ) {
    return fragment_shader.loadShader( Shader::TYPE::FRAGMENT, file_path );
}

int Draw2D::compileProgram() {
    bool link_success     = true;
    bool uniform_failed   = false;
    bool attribute_failed = false;

    // The two shaders should be allocated first.
    if( vertex_shader.getType() == Shader::TYPE::VERTEX && fragment_shader.getType() == Shader::TYPE::FRAGMENT ) {

        // Allocate the opengl program for the map.
        program.allocate();

        // Give the program these two shaders.
        program.setVertexShader( &vertex_shader );
        program.setFragmentShader( &fragment_shader );

        // Link the shader
        if( !program.link() )
            link_success = false;
        else
        {
            // Setup the uniforms for the map.
            texture_uniform_id = program.getUniform( "Texture",   &std::cout, &uniform_failed );
            matrix_uniform_id  = program.getUniform( "Transform", &std::cout, &uniform_failed );

            attribute_failed |= !program.isAttribute(   "POSITION", &std::cout );
            attribute_failed |= !program.isAttribute( "TEXCOORD_0", &std::cout );
            attribute_failed |= !program.isAttribute(    "COLOR_0", &std::cout );
        }

        vertex_array.addAttribute( "POSITION",   2, GL_FLOAT,         false, sizeof( Vertex ), 0 );
        vertex_array.addAttribute( "TEXCOORD_0", 2, GL_FLOAT,         false, sizeof( Vertex ), reinterpret_cast<void*>(2 * alignof( Vertex )) );
        vertex_array.addAttribute( "COLOR_0",    4, GL_UNSIGNED_BYTE, true,  sizeof( Vertex ), reinterpret_cast<void*>(4 * alignof( Vertex )) );

        vertex_array.allocate( program );
        vertex_array.cullUnfound();

        if( !link_success || uniform_failed || attribute_failed )
        {
            std::cout << "Font System Error\n"
                << program.getInfoLog()
                << "\nVertex shader log\n"
                << vertex_shader.getInfoLog()
                << "\nFragment shader log\n"
                << fragment_shader.getInfoLog() << std::endl;

            return 1;
        }
        else
            return -1;

        return 1;
    }
    else
    {
        return 0; // Not every shader was loaded.
    }
}

Draw2D::Draw2D() {
    attributes.push_back( Shader::Attribute( Shader::Type::HIGH,   "vec4 POSITION" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::MEDIUM, "vec2 TEXCOORD_0" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "vec4 COLOR_0" ) );

    varyings.push_back( Shader::Varying( Shader::Type::LOW, "vec4 vertex_color" ) );
    varyings.push_back( Shader::Varying( Shader::Type::MEDIUM, "vec2 texture_coord" ) );
}

Draw2D::~Draw2D() {}

void Draw2D::draw(Graphics::SDL2::GLES2::Camera& camera) {}

void Draw2D::updateImageData(const Texture2D *const internal_texture_r, const Image *const image_r, const ImageData& image_data) {
    auto internal_texture_it = this->images.find( internal_texture_r );

    if(internal_texture_it == this->images.end())
        this->images[ internal_texture_r ]; // This allocates the texture

    this->images[ internal_texture_r ][ image_r ] = image_data;
}

void Draw2D::removeImageData(const Texture2D *const internal_texture_r, const Image *const image_r) {
    auto internal_texture_it = this->images.find( internal_texture_r );

    // Check for emptiness
    if(internal_texture_it == this->images.end())
        return;

    auto image_it = this->images[ internal_texture_r ].find( image_r );

    // Check for emptiness
    if(image_it == this->images[ internal_texture_r ].end())
        return;

    this->images[ internal_texture_r ].erase(image_it);

    if(this->images[ internal_texture_r ].empty())
        this->images.erase(internal_texture_it);
}

void Draw2D::updateExternalImageData(const ExternalImage *const external_image_r, const ExternalImageData& external_image_data) {
    this->external_images[external_image_r] = external_image_data;
}

void Draw2D::removeExternalImageData(const ExternalImage *const external_image_r) {
    auto search = this->external_images.find( external_image_r );

    if(search != this->external_images.end())
        this->external_images.erase(search);
}


}
