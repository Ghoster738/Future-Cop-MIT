#include "Text2DBuffer.h" // Include the interface class
#include "../../Environment.h" // Include the interface Environment class
#include "Environment.h" // Include the internal Environment class
#include "cassert"
#include <iostream>

Graphics::SDL2::GLES2::Text2DBuffer::Text2DBuffer( Graphics::Environment &environment ) :
    Graphics::Text2DBuffer()
{
    gl_environment_r = dynamic_cast<Graphics::SDL2::GLES2::Environment*>(&environment);

    assert( gl_environment_r != nullptr ); // Graphics::SDL2::GLES2::Environment is expected here!

    const size_t KIBIBYTE_TO_BYTE = 1024;
    const size_t VERTICES_PER_CHARACTER = 6;
    
    unsigned int buffer_size_per_font_KiB = 128;

    // To be set by the environment when this buffer gets attached.
    current_text_2D_r = nullptr;
    buffer_size_per_font_KiB = buffer_size_per_font_KiB;
    text_2D_expand_factor = (buffer_size_per_font_KiB * KIBIBYTE_TO_BYTE) / (Graphics::SDL2::GLES2::Internal::FontSystem::getVertexSize() * VERTICES_PER_CHARACTER);

    // To be sure that the text_2D_expand_factor will not be zero.
    if( text_2D_expand_factor < 0x100 )
        text_2D_expand_factor = 0x100; // Clamp to 256 because any lower than this could really affect the speed of execution.
    
    text_data_p = gl_environment_r->text_draw_routine_p->getText2D();

    this->scale_font = 1.0f;
}

Graphics::SDL2::GLES2::Text2DBuffer::~Text2DBuffer() {
    for( auto i = text_data_p.begin(); i != text_data_p.end(); i++ )
        delete (*i).second;
}


int Graphics::SDL2::GLES2::Text2DBuffer::loadFonts( Graphics::Environment &environment, const std::vector<Data::Mission::IFF*> &data ) {
    auto gl_environment_r = dynamic_cast<Graphics::SDL2::GLES2::Environment*>(&environment);

    assert( gl_environment_r != nullptr ); // Graphics::SDL2::GLES2::Environment is expected here!
    
    if( gl_environment_r->text_draw_routine_p != nullptr )
        delete gl_environment_r->text_draw_routine_p;
    
    std::vector<Data::Mission::FontResource*> fonts_r;
    bool has_resource_id_1 = false;
    bool has_resource_id_2 = false;

    for( auto i = data.begin(); i != data.end(); i++ ) {
        auto font_resources = Data::Mission::FontResource::getVector( *(*i) );
        
        for( auto f = font_resources.begin(); f != font_resources.end(); f++ ) {
            fonts_r.push_back( (*f) );

            if( (*f)->getResourceID() == 1 )
                has_resource_id_1 = true;
            else
            if( (*f)->getResourceID() == 2 )
                has_resource_id_2 = true;
        }
    }

    // If no fonts are found then add one.
    if( !has_resource_id_1 ) {
        fonts_r.push_back( Data::Mission::FontResource::getPlaystation( Utilities::logger ) );
    }
    if( !has_resource_id_2 ) {
        fonts_r.push_back( Data::Mission::FontResource::getWindows( Utilities::logger ) );
    }

    gl_environment_r->text_draw_routine_p = new Graphics::SDL2::GLES2::Internal::FontSystem( fonts_r );
    gl_environment_r->text_draw_routine_p->setVertexShader();
    gl_environment_r->text_draw_routine_p->setFragmentShader();
    gl_environment_r->text_draw_routine_p->compileProgram();
    
    return fonts_r.size();
}

bool Graphics::SDL2::GLES2::Text2DBuffer::selectFont( Font &font, unsigned minium_height, unsigned maxiuim_height ) const {
    return false;
}

void Graphics::SDL2::GLES2::Text2DBuffer::draw( const glm::mat4 &projection ) const {
    auto font_system_r = gl_environment_r->text_draw_routine_p;
    
    assert( font_system_r != nullptr );
    assert( text_data_p.size() != 0 );

    font_system_r->draw( projection, text_data_p );
}

int Graphics::SDL2::GLES2::Text2DBuffer::setFont( const Font &font ) {
    auto font_system_r = gl_environment_r->text_draw_routine_p;
    
    auto last_text_2D_r = current_text_2D_r;

    if( font_system_r != nullptr )
    {
        if( text_data_p.find( font.resource_id ) != text_data_p.end() )
        {
            // Set the current text.
            current_text_2D_r = text_data_p[ font.resource_id ];

            // Steal the pen position and color from the last pen if available.
            current_text_2D_r->stealPen( last_text_2D_r );

            this->scale_font = font.scale;

            // Successfully set the current font.
            return 1;
        }
        else
            return -3; // aborted because the index is out of bounds.
    }
    else
        return -1;
}

int Graphics::SDL2::GLES2::Text2DBuffer::setPosition( const glm::vec2 &position ) {
    auto font_system_r = gl_environment_r->text_draw_routine_p;
    
    if( font_system_r != nullptr )
    {
        if( current_text_2D_r != nullptr )
        {
            // The pen position is to be set.
            current_text_2D_r->setPenPosition( position );

            // For the success of changing the color.
            return 1;
        }
        else
            return -2; // aborted current_text_2d_r is not selected.
    }
    else
        return -1;
}

int Graphics::SDL2::GLES2::Text2DBuffer::setColor( const glm::vec4 &color ) {
    auto font_system_r = gl_environment_r->text_draw_routine_p;
    uint8_t color_rgba[4];

    if( font_system_r != nullptr )
    {
        if( current_text_2D_r != nullptr )
        {
            color_rgba[ 0 ] = color.x * 255.0;
            color_rgba[ 1 ] = color.y * 255.0;
            color_rgba[ 2 ] = color.z * 255.0;
            color_rgba[ 3 ] = color.w * 255.0;

            current_text_2D_r->setPenColor( color_rgba );

            // For the success of changing the color.
            return 1;
        }
        else
            return -2; // aborted current_text_2d_r is not selected.
    }
    else
        return -1;
}

int Graphics::SDL2::GLES2::Text2DBuffer::setCenterMode( enum CenterMode center_mode ) {

    switch( center_mode ) {
        case Graphics::Text2DBuffer::CenterMode::LEFT:
            this->center_mode = 'l';
            break;
        case Graphics::Text2DBuffer::CenterMode::MIDDLE:
            this->center_mode = 'm';
            break;
        case Graphics::Text2DBuffer::CenterMode::RIGHT:
            this->center_mode = 'r';
            break;
        default:
            this->center_mode = 'l';
    }

    return 1;
}

int Graphics::SDL2::GLES2::Text2DBuffer::print( const std::string &text ) {
    auto font_system_r = gl_environment_r->text_draw_routine_p;
    size_t expand_amount;
    size_t expand_sum;
    int add_text_state;

    if( font_system_r != nullptr )
    {
        if( current_text_2D_r != nullptr )
        {
            std::string filtered_text;

            this->current_text_2D_r->getFont()->font_resource_r->filterText( text, &filtered_text );

            // Try to add the filtered_text.
            add_text_state = this->current_text_2D_r->addText( filtered_text, this->scale_font, this->center_mode );

            // Just in case of errors.
            if( add_text_state == -1 || add_text_state == -2 )
            {
                // This is a formula used to dynamically expand the filtered_text.
                expand_amount = (filtered_text.size() / text_2D_expand_factor);

                // If there is a remained then expand this number further by one.
                if((filtered_text.size() % text_2D_expand_factor) > 0)
                    expand_amount++;

                // Convert to character amount.
                expand_amount *= text_2D_expand_factor;

                // we get the expand sum.
                expand_sum = this->current_text_2D_r->getCharAmount() + expand_amount;

                // The filtered_text must be expanded
                add_text_state = this->current_text_2D_r->setTextMax( expand_sum );

                // Check to see if there was an expansion.
                if( add_text_state > 0 )
                {
                    // Attempt to add the filtered_text again.
                    add_text_state = this->current_text_2D_r->addText( filtered_text, this->scale_font );

                    if( add_text_state >= 0 )
                        return add_text_state;
                    else
                    if( add_text_state == -1 )
                        return -4; // There is still not enough space.
                    else
                    if( add_text_state == -2 )
                        return -5; // Zero space. This error should actually be almost impossible.
                }
                else
                    return -6; // The reallocation had failed.
            }

            // For the success of changing the color.
            return add_text_state;
        }
        else
            return -2; // aborted current_text_2d_r is not selected.
    }
    else
        return -1;
}

int Graphics::SDL2::GLES2::Text2DBuffer::reset() {
    auto font_system_r = gl_environment_r->text_draw_routine_p;
    int problematic_font = 0;

    if( font_system_r != nullptr )
    {
        if( text_data_p.size() != 0 )
        {
            for( auto i = text_data_p.begin(); i != text_data_p.end(); i++ )
            {
                if( (*i).second->clearText( (*i).second->getFont() ) != 1 )
                    problematic_font++;
            }

            return problematic_font;
        }
        else
            return -7; // There is no font to clear.
    }
    else
        return -1;
}
