#include "Text2DBuffer.h" // Include the interface class
#include "Environment.h" // Include the internal Environment class
#include "cassert"
#include <iostream>

Graphics::Text2DBuffer* Graphics::SDL2::GLES2::Environment::allocateText2DBuffer() {
    return new Graphics::SDL2::GLES2::Text2DBuffer( *this );
}

Graphics::SDL2::GLES2::Text2DBuffer::Text2DBuffer( Graphics::SDL2::GLES2::Environment &gl_environment ) :
    Graphics::Text2DBuffer()
{
    this->gl_environment_r = &gl_environment;

    assert( this->gl_environment_r != nullptr ); // Graphics::SDL2::GLES2::Environment is expected here!

    const size_t KIBIBYTE_TO_BYTE = 1024;
    const size_t VERTICES_PER_CHARACTER = 6;
    
    unsigned int buffer_size_per_font_KiB = 128;

    // To be set by the environment when this buffer gets attached.
    current_text_2D_r = nullptr;
    buffer_size_per_font_KiB = buffer_size_per_font_KiB;
    text_2D_expand_factor = (buffer_size_per_font_KiB * KIBIBYTE_TO_BYTE) / (Graphics::SDL2::GLES2::Internal::Draw2D::getVertexSize() * VERTICES_PER_CHARACTER);

    // To be sure that the text_2D_expand_factor will not be zero.
    if( text_2D_expand_factor < 0x100 )
        text_2D_expand_factor = 0x100; // Clamp to 256 because any lower than this could really affect the speed of execution.
    
    text_data_p = this->gl_environment_r->draw_2d_routine.text_draw_routine_p->getText2D();

    this->gl_environment_r->draw_2d_routine.text_2d_buffers.insert(this);

    this->scale_font = 1.0f;
}

Graphics::SDL2::GLES2::Text2DBuffer::~Text2DBuffer() {
    for( auto i = text_data_p.begin(); i != text_data_p.end(); i++ )
        delete (*i).second;
    this->gl_environment_r->draw_2d_routine.text_2d_buffers.erase(this);
}

std::vector<std::string> Graphics::SDL2::GLES2::Text2DBuffer::splitText( const Font &font, const std::string &unsplit_text, float line_length ) const {
    auto accessor = this->text_data_p.find( font.resource_id );

    if( accessor == this->text_data_p.end() )
        return {};

    auto font_resource_r = (*accessor).second->getFont()->font_resource_r;

    if( font_resource_r == nullptr )
        return {};

    std::string filtered_text;

    font_resource_r->filterText( unsplit_text, &filtered_text );

    std::vector<std::string> split_text;

    float current_line_length = 0;
    std::string current_line;

    for( auto i : filtered_text ) {
        std::string single_char;
        single_char += i;

        float char_length = font.scale * static_cast<float>(font_resource_r->getLineLength( single_char ));

        if( current_line_length + char_length < line_length ) {
            current_line_length += char_length;
            current_line += single_char;
        }
        else {
            if( !current_line.empty() )
                split_text.push_back( current_line );

            current_line.clear();
            current_line_length = 0;

            current_line_length += char_length;
            current_line += single_char;
        }
    }

    if( !current_line.empty() )
        split_text.push_back( current_line );

    return split_text;
}

bool Graphics::SDL2::GLES2::Text2DBuffer::selectFont( Font &font, unsigned minimum_height, unsigned maxiuim_height ) const {
    const Data::Mission::FontResource *selected_font_resource_r = nullptr;
    float scale = 1.0f;
    unsigned priority = std::numeric_limits<unsigned>::max();

    assert( maxiuim_height >= minimum_height );

    if( this->text_data_p.empty() )
        return false;

    for( auto i = this->text_data_p.begin(); i != this->text_data_p.end(); i++ ) {
        auto font_resource_r = (*i).second->getFont()->font_resource_r;

        if( font_resource_r->getHeight() >= minimum_height && font_resource_r->getHeight() <= maxiuim_height ) {
            unsigned new_priority = maxiuim_height - font_resource_r->getHeight();

            if( new_priority < priority ) {
                priority = new_priority;
                selected_font_resource_r = font_resource_r;
            }
        }
    }

    if( selected_font_resource_r == nullptr ) {
        unsigned priority = std::numeric_limits<unsigned>::max();

        for( auto i = this->text_data_p.begin(); i != this->text_data_p.end(); i++ ) {
            auto font_resource_r = (*i).second->getFont()->font_resource_r;

            if( font_resource_r->getHeight() <= maxiuim_height ) {
                unsigned new_priority = maxiuim_height - font_resource_r->getHeight();

                if( new_priority < priority ) {
                    priority = new_priority;
                    selected_font_resource_r = font_resource_r;
                }
            }
        }

        if( selected_font_resource_r != nullptr )
            scale = static_cast<float>(maxiuim_height) / static_cast<float>(selected_font_resource_r->getHeight());
    }

    if( selected_font_resource_r == nullptr ) {
        return false;
    }
    else {
        font.resource_id = selected_font_resource_r->getResourceID();
        font.scale = scale;
        return true;
    }
}

bool Graphics::SDL2::GLES2::Text2DBuffer::scaleFont( Font &font, unsigned height ) const {
    auto accessor = this->text_data_p.find( font.resource_id );

    if( accessor == this->text_data_p.end() )
        return false;

    auto font_resource_r = (*accessor).second->getFont()->font_resource_r;

    if( font_resource_r == nullptr )
        return false;

    font.scale = static_cast<float>(height) / static_cast<float>(font_resource_r->getHeight());
    return true;
}

float Graphics::SDL2::GLES2::Text2DBuffer::getLineLength( const Font &font, const std::string &text ) const {
    auto accessor = this->text_data_p.find( font.resource_id );
    std::string filtered_text;

    if( accessor == this->text_data_p.end() )
        return 0.0f;

    auto font_resource_r = (*accessor).second->getFont()->font_resource_r;

    if( font_resource_r == nullptr )
        return 0.0f;

    font_resource_r->filterText( text, &filtered_text );

    return font.scale * static_cast<float>(font_resource_r->getLineLength( filtered_text ));
}

int Graphics::SDL2::GLES2::Text2DBuffer::setFont( const Font &font ) {
    auto font_system_r = gl_environment_r->draw_2d_routine.text_draw_routine_p;
    
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
    auto font_system_r = gl_environment_r->draw_2d_routine.text_draw_routine_p;
    
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
    auto font_system_r = gl_environment_r->draw_2d_routine.text_draw_routine_p;
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
    auto font_system_r = gl_environment_r->draw_2d_routine.text_draw_routine_p;
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

            this->start.x = std::min<float>(this->start.x, this->current_text_2D_r->addedTextStart().x );
            this->start.y = std::min<float>(this->start.y, this->current_text_2D_r->addedTextStart().y );
            this->end.x   = std::max<float>(this->end.x,   this->current_text_2D_r->addedTextEnd().x );
            this->end.y   = std::max<float>(this->end.y,   this->current_text_2D_r->addedTextEnd().y );

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

                    this->start.x = std::min<float>(this->start.x, this->current_text_2D_r->addedTextStart().x );
                    this->start.y = std::min<float>(this->start.y, this->current_text_2D_r->addedTextStart().y );
                    this->end.x   = std::max<float>(this->end.x,   this->current_text_2D_r->addedTextEnd().x );
                    this->end.y   = std::max<float>(this->end.y,   this->current_text_2D_r->addedTextEnd().y );

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

void Graphics::SDL2::GLES2::Text2DBuffer::beginBox() {
    this->start = glm::vec2( std::numeric_limits<float>::max());
    this->end   = glm::vec2(-std::numeric_limits<float>::max());
}

glm::vec2 Graphics::SDL2::GLES2::Text2DBuffer::getBoxStart() const {
    return this->start;
}

glm::vec2 Graphics::SDL2::GLES2::Text2DBuffer::getBoxEnd() const {
    return this->end;
}

int Graphics::SDL2::GLES2::Text2DBuffer::reset() {
    auto font_system_r = gl_environment_r->draw_2d_routine.text_draw_routine_p;
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
