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

void Draw2D::setVertexShader( const GLchar *const shader_source ) {
    this->vertex_shader.setShader( Shader::TYPE::VERTEX, shader_source, this->attributes, this->varyings );
}

const GLchar* Draw2D::getDefaultFragmentShader() {
    return default_fragment_shader;
}

int Draw2D::loadFragmentShader( const char *const file_path ) {
    return fragment_shader.loadShader( Shader::TYPE::FRAGMENT, file_path );
}

void Draw2D::setFragmentShader( const GLchar *const shader_source ) {
    this->fragment_shader.setShader( Shader::TYPE::FRAGMENT, shader_source, {}, this->varyings );
}

int Draw2D::compileProgram() {
    bool link_success     = true;
    bool uniform_failed   = false;
    bool attribute_failed = false;

    // The two shaders should be allocated first.
    if( this->vertex_shader.getType() == Shader::TYPE::VERTEX && this->fragment_shader.getType() == Shader::TYPE::FRAGMENT ) {

        // Allocate the opengl program for the map.
        this->program.allocate();

        // Give the program these two shaders.
        this->program.setVertexShader( &vertex_shader );
        this->program.setFragmentShader( &fragment_shader );

        // Link the shader
        if( !program.link() )
            link_success = false;
        else
        {
            // Setup the uniforms for the map.
            this->texture_uniform_id = this->program.getUniform( "Texture",   &std::cout, &uniform_failed );
            this->matrix_uniform_id  = this->program.getUniform( "Transform", &std::cout, &uniform_failed );

            attribute_failed |= !this->program.isAttribute(   "POSITION", &std::cout );
            attribute_failed |= !this->program.isAttribute( "TEXCOORD_0", &std::cout );
            attribute_failed |= !this->program.isAttribute(    "COLOR_0", &std::cout );
        }

        this->vertex_array.addAttribute( "POSITION",   2, GL_FLOAT,         false, sizeof( Vertex ), 0 );
        this->vertex_array.addAttribute( "TEXCOORD_0", 2, GL_FLOAT,         false, sizeof( Vertex ), reinterpret_cast<void*>(2 * alignof( Vertex )) );
        this->vertex_array.addAttribute( "COLOR_0",    4, GL_UNSIGNED_BYTE, true,  sizeof( Vertex ), reinterpret_cast<void*>(4 * alignof( Vertex )) );

        this->vertex_array.allocate( program );
        this->vertex_array.cullUnfound();

        if( !link_success || uniform_failed || attribute_failed )
        {
            std::cout << "Font System Error\n"
                << this->program.getInfoLog()
                << "\nVertex shader log\n"
                << this->vertex_shader.getInfoLog()
                << "\nFragment shader log\n"
                << this->fragment_shader.getInfoLog() << std::endl;

            return 1;
        }
        else
            return -1;

        return 1;
    }
    else
        return 0; // Not every shader was loaded.
}

Draw2D::Draw2D() {
    this->attributes.push_back( Shader::Attribute( Shader::Type::HIGH,   "vec4 POSITION" ) );
    this->attributes.push_back( Shader::Attribute( Shader::Type::MEDIUM, "vec2 TEXCOORD_0" ) );
    this->attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "vec4 COLOR_0" ) );

    this->varyings.push_back( Shader::Varying( Shader::Type::LOW, "vec4 vertex_color" ) );
    this->varyings.push_back( Shader::Varying( Shader::Type::MEDIUM, "vec2 texture_coord" ) );

    this->text_draw_routine_p = nullptr;
}

Draw2D::~Draw2D() {
    if( this->text_draw_routine_p != nullptr ) {
        delete this->text_draw_routine_p;
        this->text_draw_routine_p = nullptr;
    }
}

void Draw2D::draw(Graphics::SDL2::GLES2::Camera& camera) {
    // Get the 2D matrix
    glm::mat4 camera_3D_projection_view_model;
    camera.getProjectionView2D( camera_3D_projection_view_model );

    // Use the text shader.
    this->program.use();

    // We can now send the matrix to the program.
    glUniformMatrix4fv( this->matrix_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &camera_3D_projection_view_model ) );

    for( auto i = camera.getText2DBuffer()->begin(); i != camera.getText2DBuffer()->end(); i++ ) {
        // TODO Eventually remove this kind of upcasts. They are dangerious.
        auto text_2d_draw_routine = dynamic_cast<Text2DBuffer*>( *i );

        assert( text_2d_draw_routine != nullptr );

        text_2d_draw_routine->draw( camera_3D_projection_view_model );
    }
}

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
