#include "Draw2D.h"

#include "../ExternalImage.h"

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
    "    vec4 color = texture2D(Texture, texture_coord).rgba;\n"
    "    if( color.a < 0.03125 )\n"
    "        discard;\n"
    "    gl_FragColor = vertex_color * color;\n"
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
            return -1;
        }

        this->buffer_p = new Draw2D::Vertex [ 3 * this->max_triangles ];

        glGenBuffers( 1, &this->vertex_buffer_object );
        glBindBuffer( GL_ARRAY_BUFFER, this->vertex_buffer_object );
        glBufferData( GL_ARRAY_BUFFER, 3 * sizeof( Draw2D::Vertex ) * this->max_triangles, nullptr, GL_DYNAMIC_DRAW );

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

    this->buffer_p = nullptr;
    this->max_triangles = 1024;
}

Draw2D::~Draw2D() {
    if( this->buffer_p != nullptr ) {
        delete [] this->buffer_p;
        glDeleteBuffers( 1, &this->vertex_buffer_object );
    }

    if( this->text_draw_routine_p != nullptr ) {
        delete this->text_draw_routine_p;
        this->text_draw_routine_p = nullptr;
    }
}

void Draw2D::draw(Graphics::SDL2::GLES2::Camera& camera) {
    Color color;

    // Get the 2D matrix
    glm::mat4 camera_3D_projection_view_model = glm::ortho( 0.0f, static_cast<float>( camera.getViewportDimensions().x ), -static_cast<float>( camera.getViewportDimensions().y ), 0.0f, -1.0f, 1.0f );

    // Use the text shader.
    this->program.use();

    // We can now send the matrix to the program.
    glUniformMatrix4fv( this->matrix_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &camera_3D_projection_view_model ) );

    // Draw the cbmp images.
    for(auto texture_iterator = images.begin(); texture_iterator != images.end(); texture_iterator++) {
        texture_iterator->first->bind( 0, this->texture_uniform_id );

        size_t num_triangles = 0;

        glBindBuffer( GL_ARRAY_BUFFER, this->vertex_buffer_object );

        for(auto image_instance_iterator = texture_iterator->second.begin(); image_instance_iterator != texture_iterator->second.end(); image_instance_iterator++) {
            ImageData *image_data_r = &image_instance_iterator->second;

            // Flush the memory to make more space
            if(num_triangles + 2 >= this->max_triangles) {
                glBufferSubData( GL_ARRAY_BUFFER, 0, 3 * num_triangles * sizeof( Draw2D::Vertex ), this->buffer_p );
                this->vertex_array.bind();
                glDrawArrays( GL_TRIANGLES, 0, 3 * num_triangles );
                num_triangles = 0;
            }

            if(!image_data_r->visable || num_triangles + 2 >= this->max_triangles)
                continue;

            color.set(image_data_r->color.x * 255.0, image_data_r->color.y * 255.0, image_data_r->color.z * 255.0, image_data_r->color.w * 255.0);

            this->buffer_p[3 * num_triangles + 0].set(image_data_r->positions[0].x, -image_data_r->positions[1].y, image_data_r->texture_coords[0].x, image_data_r->texture_coords[1].y, color.color_rgba);
            this->buffer_p[3 * num_triangles + 1].set(image_data_r->positions[1].x, -image_data_r->positions[1].y, image_data_r->texture_coords[1].x, image_data_r->texture_coords[1].y, color.color_rgba);
            this->buffer_p[3 * num_triangles + 2].set(image_data_r->positions[1].x, -image_data_r->positions[0].y, image_data_r->texture_coords[1].x, image_data_r->texture_coords[0].y, color.color_rgba);
            num_triangles++;

            this->buffer_p[3 * num_triangles + 0].set(image_data_r->positions[1].x, -image_data_r->positions[0].y, image_data_r->texture_coords[1].x, image_data_r->texture_coords[0].y, color.color_rgba);
            this->buffer_p[3 * num_triangles + 1].set(image_data_r->positions[0].x, -image_data_r->positions[0].y, image_data_r->texture_coords[0].x, image_data_r->texture_coords[0].y, color.color_rgba);
            this->buffer_p[3 * num_triangles + 2].set(image_data_r->positions[0].x, -image_data_r->positions[1].y, image_data_r->texture_coords[0].x, image_data_r->texture_coords[1].y, color.color_rgba);
            num_triangles++;
        }

        glBufferSubData( GL_ARRAY_BUFFER, 0, 3 * num_triangles * sizeof( Draw2D::Vertex ), this->buffer_p );
        this->vertex_array.bind();
        glDrawArrays( GL_TRIANGLES, 0, 3 * num_triangles );
    }

    // Draw the dynamic images.
    for(auto texture_iterator = this->dynamic_images.begin(); texture_iterator != this->dynamic_images.end(); texture_iterator++) {
        DynamicImageData *image_data_r = &texture_iterator->second;

        if(!image_data_r->visable || image_data_r->texture_2d == nullptr)
            continue;

       image_data_r->texture_2d->bind( 0, this->texture_uniform_id );

       color.set(image_data_r->color.x * 255.0, image_data_r->color.y * 255.0, image_data_r->color.z * 255.0, image_data_r->color.w * 255.0);

        this->buffer_p[0].set(image_data_r->positions[0].x, -image_data_r->positions[1].y, 0, 1, color.color_rgba);
        this->buffer_p[1].set(image_data_r->positions[1].x, -image_data_r->positions[1].y, 1, 1, color.color_rgba);
        this->buffer_p[2].set(image_data_r->positions[1].x, -image_data_r->positions[0].y, 1, 0, color.color_rgba);

        this->buffer_p[3].set(image_data_r->positions[1].x, -image_data_r->positions[0].y, 1, 0, color.color_rgba);
        this->buffer_p[4].set(image_data_r->positions[0].x, -image_data_r->positions[0].y, 0, 0, color.color_rgba);
        this->buffer_p[5].set(image_data_r->positions[0].x, -image_data_r->positions[1].y, 0, 1, color.color_rgba);

        glBindBuffer( GL_ARRAY_BUFFER, this->vertex_buffer_object );
        glBufferSubData( GL_ARRAY_BUFFER, 0, 6 * sizeof( Draw2D::Vertex ), this->buffer_p );
        this->vertex_array.bind();
        glDrawArrays( GL_TRIANGLES, 0, 6 );
    }

    // Lastly, draw the font.
    for( auto i = this->text_2d_buffers.begin(); i != this->text_2d_buffers.end(); i++ ) {
        auto text_2d_buffer_r = *i;

        assert( text_2d_buffer_r != nullptr ); // No null allowed in set.

        assert( this->text_draw_routine_p != nullptr );
        assert( text_2d_buffer_r->text_data_p.size() != 0 );

        this->text_draw_routine_p->draw( camera_3D_projection_view_model, this->texture_uniform_id, this->vertex_array, text_2d_buffer_r->text_data_p );
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

void Draw2D::updateDynamicImageData(const Graphics::ImageBase *const image_base_r, const DynamicImageData& dynamic_image_data) {
    auto search = this->dynamic_images.find( image_base_r );

    this->dynamic_images[image_base_r].positions[0] = dynamic_image_data.positions[0];
    this->dynamic_images[image_base_r].positions[1] = dynamic_image_data.positions[1];
    this->dynamic_images[image_base_r].color        = dynamic_image_data.color;
    this->dynamic_images[image_base_r].visable      = dynamic_image_data.visable;

    if(search == this->dynamic_images.end()) {
        this->dynamic_images[image_base_r].texture_2d = nullptr;
    }
}

void Draw2D::uploadDynamicImageData(const Graphics::ImageBase *const image_base_r, const Utilities::Image2D& image_2d, GLenum image_gl_format) {
    auto search = this->dynamic_images.find( image_base_r );

    if(search == this->dynamic_images.end())
        return;

    DynamicImageData *image_data_r = &search->second;

    if(image_data_r->texture_2d == nullptr || (image_2d.getWidth() != image_data_r->width || image_2d.getHeight() != image_data_r->height)) {
        if(image_data_r->texture_2d != nullptr)
            delete image_data_r->texture_2d;

        image_data_r->texture_2d = new Internal::Texture2D;

        image_data_r->texture_2d->setFilters( 0, GL_NEAREST, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        image_data_r->texture_2d->setImage(0, 0, image_gl_format, image_2d.getWidth(), image_2d.getHeight(), 0, image_gl_format, GL_UNSIGNED_BYTE, image_2d.getDirectGridData());

        image_data_r->width  = image_2d.getWidth();
        image_data_r->height = image_2d.getHeight();
    }
    else {
        image_data_r->texture_2d->updateImage(0, 0, image_2d.getWidth(), image_2d.getHeight(), image_gl_format, GL_UNSIGNED_BYTE, image_2d.getDirectGridData());
    }
}

void Draw2D::removeDynamicImageData(const Graphics::ImageBase *const image_base_r) {
    auto search = this->dynamic_images.find( image_base_r );

    if(search != this->dynamic_images.end()) {
        if(search->second.texture_2d != nullptr)
            delete search->second.texture_2d;

        this->dynamic_images.erase(search);
    }
}


}
