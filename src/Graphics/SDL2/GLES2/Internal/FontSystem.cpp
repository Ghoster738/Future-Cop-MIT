#include "FontSystem.h"
#include <iostream>
#include <cassert>

namespace {
    /**
     * This holds the vertex for the font system.
     */
    struct TextVertex {
        float x, y;
        float u, v;
        uint32_t color_rgba;
        TextVertex( float set_x, float set_y, float set_u, float set_v,
            uint32_t set_color_rgba ) :
            x( set_x ), y( set_y ), u( set_u ), v( set_v ),
            color_rgba( set_color_rgba )
        {
        }
        void set( float set_x, float set_y, float set_u, float set_v,
            uint32_t set_color_rgba ) {
            x = set_x, y = set_y, u = set_u, v = set_v;
            color_rgba = set_color_rgba;
        }
    };

    const uint8_t DEFAULT_COLOR[4] = { 0xFF, 0xFF, 0xFF, 0xFF };

    struct Color {
        uint32_t color_rgba;
        uint8_t* toPointer() { return reinterpret_cast<uint8_t*>(&color_rgba); }
        void set( uint8_t r, uint8_t b, uint8_t g, uint8_t a ) {
            uint8_t *color = toPointer();
            color[0] = r;
            color[1] = b;
            color[2] = g;
            color[3] = a;
        }
    };
}

const GLchar* Graphics::SDL2::GLES2::Internal::FontSystem::default_vertex_shader =
    // Inputs
    "attribute vec2 POSITION;\n"
    "attribute vec2 TEXCOORD_0;\n"
    "attribute vec4 COLOR_0;\n"

    // Vertex shader uniforms
    "uniform mat4  Transform;\n" // projection * view * model.

    // These are the outputs
    "varying vec4 vertex_color;\n"
    "varying vec2 texture_coord;\n"

    "void main()\n"
    "{\n"
    "   texture_coord = TEXCOORD_0;\n"
    "   vertex_color = COLOR_0;\n"
    "   gl_Position = Transform * vec4(POSITION.xy, 0.0, 1.0);\n"
    "}\n";
const GLchar* Graphics::SDL2::GLES2::Internal::FontSystem::default_fragment_shader =
    "varying vec4 vertex_color;\n"
    "varying vec2 texture_coord;\n"

    "uniform sampler2D Texture;\n"

    "void main()\n"
    "{\n"
    "    float visable = texture2D(Texture, texture_coord).r;\n"
    "    if( visable < 0.03125 )\n"
    "        discard;\n"
    "    gl_FragColor = vertex_color;\n"
    "}\n";

Graphics::SDL2::GLES2::Internal::FontSystem::Text2D * Graphics::SDL2::GLES2::Internal::FontSystem::Font::allocateText2D() {
    return new Text2D( this );
}

Graphics::SDL2::GLES2::Internal::FontSystem::Text2D::Text2D( Graphics::SDL2::GLES2::Internal::FontSystem::Font *font_r ) {
    this->font_r = font_r;
    this->buffer_p = nullptr;
    this->amount_of_characters = 0;
    this->max_amount_of_characters = 0;
    this->vertex_buffer_object = 0;

    setPenColor( DEFAULT_COLOR );
    setPenPosition( glm::vec2( 0, 0 ) );

    // font_r should never be null!
    assert( this->font_r != nullptr );
    // font_r->font_resource_r should never be null either!
    assert( this->font_r->font_resource_r != nullptr );
}

Graphics::SDL2::GLES2::Internal::FontSystem::Text2D::~Text2D() {
    if( this->buffer_p != nullptr )
    {
        delete [] this->buffer_p;
        glDeleteBuffers( 1, &vertex_buffer_object );
    }
}

void Graphics::SDL2::GLES2::Internal::FontSystem::Text2D::setPenColor( const uint8_t *pen_color ) {
    this->pen_color = *reinterpret_cast<const uint32_t*>( pen_color );
}

void Graphics::SDL2::GLES2::Internal::FontSystem::Text2D::setPenPosition( const glm::vec2 &pen_position ) {
    this->pen_position = pen_position;
}

bool Graphics::SDL2::GLES2::Internal::FontSystem::Text2D::stealPen( const Text2D *const pen_r )
{
    if( pen_r != nullptr )
    {
        this->pen_color = pen_r->pen_color;
        this->pen_position = pen_r->pen_position;
        return true;
    }
    else
        return false;
}

int Graphics::SDL2::GLES2::Internal::FontSystem::Text2D::setTextMax( size_t max_text ) {
    // You really should have believed me when I wrote, "This operation
    // is actually expensive." However, it will take linear time, plus
    // the time it would take to allocate memory.
    if( max_text == amount_of_characters )
        return max_text; // Silently, return a success.
    else
    if( max_text < amount_of_characters )
        return 0;
    else
    {
        // Attempt to allocate data.
        size_t max_text_buffer_size = max_text * sizeof( TextVertex ) * 6;
        auto new_buffer_p = new uint8_t [ max_text_buffer_size ];

        if( new_buffer_p == nullptr ) {
            // Memory allocation had failed.
            // Probably due to a ram issue.
            return -1;
        }
        else // If every test passes
        {
            glGenBuffers( 1, &this->vertex_buffer_object );
            glBindBuffer( GL_ARRAY_BUFFER, this->vertex_buffer_object );
            glBufferData( GL_ARRAY_BUFFER, max_text_buffer_size, nullptr, GL_DYNAMIC_DRAW );

            // Handle the memory expansion case.
            if( this->amount_of_characters != 0 )
            {
                for( size_t i = 0; i < this->amount_of_characters * 6; i++ ) {
                     reinterpret_cast<TextVertex*>(new_buffer_p)[ i ] =
                        reinterpret_cast<TextVertex*>(this->buffer_p)[ i ];
                }
                // If the this->amount_of_characters is a nonzero,
                // then these two values are allocated.
                delete [] this->buffer_p;

                // Post the copied data to OpenGL for good measure.
                glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( TextVertex ) * this->amount_of_characters * 6, new_buffer_p );
            }

            // Finally, we can allocate this->text_p and this->buffer_p
            this->buffer_p = new_buffer_p;
            this->max_amount_of_characters = max_text;

            // This indicates success.
            return max_text;
        }
    }
}

int Graphics::SDL2::GLES2::Internal::FontSystem::Text2D::addText( const std::string &text ) {
    return addText( text.c_str() );
}

int Graphics::SDL2::GLES2::Internal::FontSystem::Text2D::addText( const char *const text ) {
    size_t appended_size = 0;
    auto character_buffer = reinterpret_cast<TextVertex*>(this->buffer_p);
    glm::vec2 lower_font, higher_font;
    glm::vec2 texture_low, texture_high;

    if( this->max_amount_of_characters != 0 ) {
        glBindBuffer( GL_ARRAY_BUFFER, vertex_buffer_object );

        for( const char * character = text; *character != '\0' && (appended_size + this->amount_of_characters) != this->max_amount_of_characters; character++ )
        {

            auto glyph_r = this->font_r->font_resource_r->getGlyph( character[0] );

            assert( this->font_r->font_resource_r != nullptr );

            if( glyph_r != nullptr && glyph_r->getGlyph() == character[0] )
            {
                // Since the glypth is found it can now be appart of the text
                appended_size++;

                lower_font.x = pen_position.x + static_cast<float>(  glyph_r->getOffset().x );
                higher_font.x =  lower_font.x + static_cast<float>(  glyph_r->getWidth() );
                lower_font.y =-pen_position.y + static_cast<float>( -glyph_r->getOffset().y );
                higher_font.y =  lower_font.y + static_cast<float>( -glyph_r->getHeight());
                texture_low.x  = static_cast<float>( glyph_r->getLeft()   ) / static_cast<float>( font_r->texture_scale.x );
                texture_low.y  = static_cast<float>( glyph_r->getTop()    ) / static_cast<float>( font_r->texture_scale.y );
                texture_high.x = static_cast<float>( glyph_r->getRight()  ) / static_cast<float>( font_r->texture_scale.x );
                texture_high.y = static_cast<float>( glyph_r->getBottom() ) / static_cast<float>( font_r->texture_scale.y );

                pen_position.x += static_cast<float>( glyph_r->getXAdvance() );

                character_buffer[0].set(  lower_font.x,  lower_font.y,  texture_low.x,  texture_low.y, pen_color );
                character_buffer[1].set( higher_font.x,  lower_font.y, texture_high.x,  texture_low.y, pen_color );
                character_buffer[2].set( higher_font.x, higher_font.y, texture_high.x, texture_high.y, pen_color );
                character_buffer[3].set( higher_font.x, higher_font.y, texture_high.x, texture_high.y, pen_color );
                character_buffer[4].set(  lower_font.x, higher_font.y,  texture_low.x, texture_high.y, pen_color );
                character_buffer[5].set(  lower_font.x,  lower_font.y,  texture_low.x,  texture_low.y, pen_color );

                character_buffer += 6;
            }
            else
                std::cout << "invalid = " << character[0] << std::endl;
        }

        glBufferSubData( GL_ARRAY_BUFFER, sizeof( TextVertex ) * this->amount_of_characters * 6, sizeof( TextVertex ) * (appended_size + this->amount_of_characters) * 6, this->buffer_p);

        this->amount_of_characters += appended_size;

        if( appended_size != 0 )
            return appended_size;
        else
            return -1;
    }
    else
        return -2; // There is no memory allocated yet.
}

bool Graphics::SDL2::GLES2::Internal::FontSystem::Text2D::clearText( Font *font_r ) {
    if( font_r != nullptr )
        this->font_r = font_r;

    if( max_amount_of_characters == 0 )
        return false;
    else
    {
        amount_of_characters = 0;
        return true;
    }
}

void Graphics::SDL2::GLES2::Internal::FontSystem::Text2D::draw( const VertexAttributeArray &vertex_array ) const {
    glBindBuffer( GL_ARRAY_BUFFER, vertex_buffer_object );
    vertex_array.bind();
    glDrawArrays( GL_TRIANGLES, 0, amount_of_characters * 6 );
}

Graphics::SDL2::GLES2::Internal::FontSystem::FontSystem( const std::vector<Data::Mission::FontResource*> &font_resources ) {
    this->font_bank.reserve( font_resources.size() );

    for( int i = 0; i < font_resources.size(); i++ )
    {
        this->font_bank.push_back( Font() );

        assert( font_resources[i] != nullptr );

        // Set the pointer to
        this->font_bank.back().font_resource_r = font_resources[i];

        const Utilities::ImageData *const font_image = this->font_bank.back().font_resource_r->getImage();

        this->font_bank.back().texture_scale.x = 1;
        this->font_bank.back().texture_scale.y = 1;

        // These loops ensure that the texture is a power by two.
        while( this->font_bank.back().texture_scale.x < font_image->getWidth() ) {
            this->font_bank.back().texture_scale.x *= 2;
        }
        while( this->font_bank.back().texture_scale.y < font_image->getHeight() ) {
            this->font_bank.back().texture_scale.y *= 2;
        }

        Utilities::ImageData image(
            this->font_bank.back().texture_scale.x,
            this->font_bank.back().texture_scale.y,
            Utilities::ImageData::Type::BLACK_WHITE, 1 );

        auto state = image.inscribeSubImage( 0, 0, *font_image );

        if( state == false )
        {
            std::cout << "Failed to inscribe!" << std::endl;
            std::cout << "font Width = " << font_image->getWidth() << std::endl;
            std::cout << "font height = " << font_image->getHeight() << std::endl;
            std::cout << "new Width = " << image.getWidth() << std::endl;
            std::cout << "new height = " << image.getHeight() << std::endl;
        }

        assert( state == true );
        this->font_bank.back().texture.setFilters( 0, GL_NEAREST, GL_LINEAR );
        this->font_bank.back().texture.setImage( 0, 0, GL_LUMINANCE, image.getWidth(), image.getHeight(), 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, image.getRawImageData() );
    }
}

Graphics::SDL2::GLES2::Internal::FontSystem::~FontSystem() {
    for( size_t i = 0; i < font_bank.size(); i++ ) {
        font_bank[i].font_resource_r = nullptr;
    }
}

int Graphics::SDL2::GLES2::Internal::FontSystem::getNumFonts() const {
    return font_bank.size();
}

Graphics::SDL2::GLES2::Internal::FontSystem::Font* Graphics::SDL2::GLES2::Internal::FontSystem::accessFont( unsigned int index ) {
    if( font_bank.size() > index )
        return &font_bank[ index ];
    else
        return nullptr;
}

void Graphics::SDL2::GLES2::Internal::FontSystem::setVertexShader( const GLchar *const shader_source ) {
    vertex_shader.setShader( Shader::TYPE::VERTEX, shader_source );
}

int Graphics::SDL2::GLES2::Internal::FontSystem::loadVertexShader( const char *const file_path ) {
    return vertex_shader.loadShader( Shader::TYPE::VERTEX, file_path );
}

void Graphics::SDL2::GLES2::Internal::FontSystem::setFragmentShader( const GLchar *const shader_source ) {
    fragment_shader.setShader( Shader::TYPE::FRAGMENT, shader_source );
}

int Graphics::SDL2::GLES2::Internal::FontSystem::loadFragmentShader( const char *const file_path ) {
    return fragment_shader.loadShader( Shader::TYPE::FRAGMENT, file_path );
}

size_t Graphics::SDL2::GLES2::Internal::FontSystem::getVertexSize() {
    return sizeof( TextVertex );
}

int Graphics::SDL2::GLES2::Internal::FontSystem::compileProgram() {
    // The two shaders should be allocated first.
    if( vertex_shader.getType() == Shader::TYPE::VERTEX && fragment_shader.getType() == Shader::TYPE::FRAGMENT ) {

        // Allocate the opengl program for the map.
        program.allocate();

        // Give the program these two shaders.
        program.setVertexShader( &vertex_shader );
        program.setFragmentShader( &fragment_shader );

        // Link the shader
        program.link();

        // Setup the uniforms for the map.
        texture_uniform_id = glGetUniformLocation( program.getProgramID(), "Texture" );
        matrix_uniform_id = glGetUniformLocation( program.getProgramID(), "Transform" );

        vertex_array.addAttribute( "POSITION",   2, GL_FLOAT,         false, sizeof( TextVertex ), 0 );
        vertex_array.addAttribute( "TEXCOORD_0", 2, GL_FLOAT,         false, sizeof( TextVertex ), reinterpret_cast<void*>(2 * alignof( TextVertex )) );
        vertex_array.addAttribute( "COLOR_0",    4, GL_UNSIGNED_BYTE, true,  sizeof( TextVertex ), reinterpret_cast<void*>(4 * alignof( TextVertex )) );

        vertex_array.allocate( program );
    
        vertex_array.cullUnfound( &std::cout );

        return 1;
    }
    else
    {
        return 0; // Not every shader was loaded.
    }
}

void Graphics::SDL2::GLES2::Internal::FontSystem::draw( const glm::mat4 &projection, const std::vector<Graphics::SDL2::GLES2::Internal::FontSystem::Text2D*> &text_2d_array ) {
    // Use the text shader.
    program.use();

    // We can now send the matrix to the program.
    glUniformMatrix4fv( matrix_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &projection ) );

    for( auto i = text_2d_array.begin(); i != text_2d_array.end(); i++ )
    {
        (*i)->getFont()->texture.bind( 0, texture_uniform_id );
        (*i)->draw( vertex_array );
    }
}
