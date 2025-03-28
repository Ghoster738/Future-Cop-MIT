#include "FontSystem.h"
#include <iostream>
#include <cassert>
#include "SDL.h"

#include "Draw2D.h"

Graphics::SDL2::GLES2::Internal::FontSystem::Text2D * Graphics::SDL2::GLES2::Internal::FontSystem::Font::allocateText2D() {
    return new Text2D( this );
}

Graphics::SDL2::GLES2::Internal::FontSystem::Text2D::Text2D( Graphics::SDL2::GLES2::Internal::FontSystem::Font *font_r ) {
    this->font_r = font_r;
    this->buffer_p = nullptr;
    this->amount_of_characters = 0;
    this->max_amount_of_characters = 0;
    this->vertex_buffer_object = 0;

    setPenColor( Draw2D::DEFAULT_COLOR );
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
        size_t max_text_buffer_size = max_text * sizeof( Draw2D::Vertex ) * 6;
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
                     reinterpret_cast<Draw2D::Vertex*>(new_buffer_p)[ i ] =
                        reinterpret_cast<Draw2D::Vertex*>(this->buffer_p)[ i ];
                }
                // If the this->amount_of_characters is a nonzero,
                // then these two values are allocated.
                delete [] this->buffer_p;

                // Post the copied data to OpenGL for good measure.
                glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( Draw2D::Vertex ) * this->amount_of_characters * 6, new_buffer_p );
            }

            // Finally, we can allocate this->text_p and this->buffer_p
            this->buffer_p = new_buffer_p;
            this->max_amount_of_characters = max_text;

            // This indicates success.
            return max_text;
        }
    }
}

int Graphics::SDL2::GLES2::Internal::FontSystem::Text2D::addText( const std::string &text, float scale, char centering ) {
    this->pen_span_max = glm::vec2(-std::numeric_limits<float>::max());
    this->pen_span_min = glm::vec2( std::numeric_limits<float>::max());

    size_t appended_size = 0;
    auto text_vertex_buffer_r = reinterpret_cast<Draw2D::Vertex*>(this->buffer_p);
    auto char_vertex_buffer_r = text_vertex_buffer_r;
    glm::vec2 lower_font, higher_font;
    glm::vec2 texture_low, texture_high;

    float last_line_pos = pen_position.x;

    if( this->max_amount_of_characters != 0 ) {
        glBindBuffer( GL_ARRAY_BUFFER, vertex_buffer_object );

        assert( this->font_r->font_resource_r != nullptr );

        for( const uint8_t *character = reinterpret_cast<const uint8_t*>( text.c_str() ); *character != '\0' && (appended_size + this->amount_of_characters) != this->max_amount_of_characters; character++ )
        {

            auto glyph_r = this->font_r->font_resource_r->getGlyph( character[0] );

            if( glyph_r != nullptr && glyph_r->glyphID == character[0] )
            {
                // Since the glyph is found it can now be appart of the text
                appended_size++;

                lower_font.x = pen_position.x +  static_cast<float>( glyph_r->offset.x );
                higher_font.x =  lower_font.x +  static_cast<float>( glyph_r->width ) * scale;
                lower_font.y =-pen_position.y + -static_cast<float>( glyph_r->offset.y ) * scale;
                higher_font.y =  lower_font.y + -static_cast<float>( glyph_r->height ) * scale;
                texture_low.x  = static_cast<float>( glyph_r->left   ) / static_cast<float>( font_r->texture_scale.x );
                texture_low.y  = static_cast<float>( glyph_r->top    ) / static_cast<float>( font_r->texture_scale.y );
                texture_high.x = static_cast<float>( glyph_r->left + glyph_r->width ) / static_cast<float>( font_r->texture_scale.x );
                texture_high.y = static_cast<float>( glyph_r->top + glyph_r->height ) / static_cast<float>( font_r->texture_scale.y );

                pen_position.x += static_cast<float>( glyph_r->x_advance ) * scale;

                char_vertex_buffer_r[0].set(  lower_font.x,  lower_font.y,  texture_low.x,  texture_low.y, pen_color );
                char_vertex_buffer_r[1].set(  lower_font.x, higher_font.y,  texture_low.x, texture_high.y, pen_color );
                char_vertex_buffer_r[2].set( higher_font.x, higher_font.y, texture_high.x, texture_high.y, pen_color );
                char_vertex_buffer_r[3].set( higher_font.x, higher_font.y, texture_high.x, texture_high.y, pen_color );
                char_vertex_buffer_r[4].set( higher_font.x,  lower_font.y, texture_high.x,  texture_low.y, pen_color );
                char_vertex_buffer_r[5].set(  lower_font.x,  lower_font.y,  texture_low.x,  texture_low.y, pen_color );

                char_vertex_buffer_r += 6;

                // TODO Detect if character is completly transparent rather than relying upon this method.
                // The issue of this method is that if the charset is different from ascii this would produce unpredictable results.
                if(!isspace(*character)) {
                    this->pen_span_max.x = std::max<float>(this->pen_span_max.x,  lower_font.x );
                    this->pen_span_max.y = std::max<float>(this->pen_span_max.y, -lower_font.y );
                    this->pen_span_max.x = std::max<float>(this->pen_span_max.x,  higher_font.x );
                    this->pen_span_max.y = std::max<float>(this->pen_span_max.y, -higher_font.y );
                    this->pen_span_min.x = std::min<float>(this->pen_span_min.x,  lower_font.x );
                    this->pen_span_min.y = std::min<float>(this->pen_span_min.y, -lower_font.y );
                    this->pen_span_min.x = std::min<float>(this->pen_span_min.x,  higher_font.x );
                    this->pen_span_min.y = std::min<float>(this->pen_span_min.y, -higher_font.y );
                }
            }
            else
                std::cout << "invalid = " << (uint32_t)character[0] << std::endl;
        }

        if( centering == 'm' ) {
            for( size_t x = 0; x < appended_size * 6; x++ ) {
                text_vertex_buffer_r[x].x -= (pen_position.x - last_line_pos) * 0.5;
            }
            this->pen_span_max.x -= (pen_position.x - last_line_pos) * 0.5;
            this->pen_span_min.x -= (pen_position.x - last_line_pos) * 0.5;
        }
        else
        if( centering == 'r' ) {
            for( size_t x = 0; x < appended_size * 6; x++ ) {
                text_vertex_buffer_r[x].x -= (pen_position.x - last_line_pos);
            }
            this->pen_span_max.x -= (pen_position.x - last_line_pos);
            this->pen_span_min.x -= (pen_position.x - last_line_pos);
        }

        glBufferSubData( GL_ARRAY_BUFFER, sizeof( Draw2D::Vertex ) * this->amount_of_characters * 6, sizeof( Draw2D::Vertex ) * (appended_size + this->amount_of_characters) * 6, this->buffer_p);

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

Graphics::SDL2::GLES2::Internal::FontSystem::FontSystem( const std::vector<const Data::Mission::FontResource*> &font_resources ) {
    this->font_bank.reserve( font_resources.size() );

    for( unsigned i = 0; i < font_resources.size(); i++ )
    {
        this->font_bank.push_back( Font() );

        assert( font_resources[i] != nullptr );
        
        this->font_bank.back().resource_id = font_resources[i]->getResourceID();

        // Set the pointer to
        this->font_bank.back().font_resource_r = font_resources[i];

        const Utilities::Image2D *const font_image = this->font_bank.back().font_resource_r->getImage();

        this->font_bank.back().texture_scale.x = 1;
        this->font_bank.back().texture_scale.y = 1;

        // These loops ensure that the texture is a power by two.
        while( this->font_bank.back().texture_scale.x < font_image->getWidth() ) {
            this->font_bank.back().texture_scale.x *= 2;
        }
        while( this->font_bank.back().texture_scale.y < font_image->getHeight() ) {
            this->font_bank.back().texture_scale.y *= 2;
        }
        
        Utilities::Image2D image(
            this->font_bank.back().texture_scale.x,
            this->font_bank.back().texture_scale.y,
            Utilities::PixelFormatColor_W8A8::linear );

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
        this->font_bank.back().texture.setImage( 0, 0, GL_LUMINANCE_ALPHA, image.getWidth(), image.getHeight(), 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, image.getDirectGridData() );
    }
}

Graphics::SDL2::GLES2::Internal::FontSystem::~FontSystem() {
    for( size_t i = 0; i < font_bank.size(); i++ ) {
        font_bank[i].font_resource_r = nullptr;
    }
}

std::map<uint32_t, Graphics::SDL2::GLES2::Internal::FontSystem::Text2D*> Graphics::SDL2::GLES2::Internal::FontSystem::getText2D() {
    std::map<uint32_t, Text2D*> font_map;
    
    for( auto a = font_bank.begin(); a != font_bank.end(); a++ ) {
        font_map[ (*a).resource_id ] = (*a).allocateText2D();
    }
    
    return font_map;
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

void Graphics::SDL2::GLES2::Internal::FontSystem::draw( const glm::mat4 &projection, GLuint texture_uniform_id, VertexAttributeArray &vertex_array, const std::map<uint32_t, Graphics::SDL2::GLES2::Internal::FontSystem::Text2D*> &text_2d_array ) {
    for( auto i = text_2d_array.begin(); i != text_2d_array.end(); i++ )
    {
        (*i).second->getFont()->texture.bind( 0, texture_uniform_id );
        (*i).second->draw( vertex_array );
    }
}
