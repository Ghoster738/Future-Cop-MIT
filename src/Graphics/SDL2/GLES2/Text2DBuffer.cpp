#include "Text2DBuffer.h" // Include the interface class
#include "../../Environment.h" // Include the interface Environment class
#include "Environment.h" // Include the internal Environment class
#include "cassert"

Graphics::SDL2::GLES2::Text2DBuffer::Text2DBuffer( Environment &env ) :
    Graphics::Text2DBuffer( env )
{
    const size_t KIBIBYTE_TO_BYTE = 1024;
    const size_t VERTICES_PER_CHARACTER = 6;
    
    unsigned int buffer_size_per_font_KiB = 128;

    // To be set by the environment when this buffer gets attached.
    font_system_r = nullptr;
    current_text_2D_r = nullptr;
    buffer_size_per_font_KiB = buffer_size_per_font_KiB;
    text_2D_expand_factor = (buffer_size_per_font_KiB * KIBIBYTE_TO_BYTE) / (Graphics::SDL2::GLES2::Internal::FontSystem::getVertexSize() * VERTICES_PER_CHARACTER);

    // To be sure that the text_2D_expand_factor will not be zero.
    if( text_2D_expand_factor < 0x100 )
        text_2D_expand_factor = 0x100; // Clamp to 256 because any lower than this could really affect the speed of execution.
    
    loadFontLibrary( env );
}

Graphics::SDL2::GLES2::Text2DBuffer::~Text2DBuffer() {
    for( auto i = text_data.begin(); i != text_data.end(); i++ )
        delete (*i);
}

bool Graphics::SDL2::GLES2::Text2DBuffer::loadFontLibrary( Graphics::Environment &environment )
{
    auto environment_internal_data_r = reinterpret_cast<Graphics::SDL2::GLES2::EnvironmentInternalData *>( environment.getInternalData() );

    // Make sure there is text.
    if( environment_internal_data_r->text_draw_routine != nullptr )
    {
        font_system_r = environment_internal_data_r->text_draw_routine;

        if( font_system_r->getNumFonts() > 0 )
        {
            text_data.reserve( font_system_r->getNumFonts() );

            for( int i = 0; i < font_system_r->getNumFonts(); i++ )
            {
                auto font_r = font_system_r->accessFont( i );

                assert( font_r != nullptr );

                text_data.push_back( font_r->allocateText2D() );

                assert( font_r == text_data[i]->getFont() );
            }

            return true;
        }
        else
            return false;
    }
    else
        return false;
}

int Graphics::SDL2::GLES2::Text2DBuffer::setFont( unsigned index ) {
    auto last_text_2D_r = current_text_2D_r;

    if( font_system_r != nullptr )
    {
        if( text_data.size() > index )
        {
            // Set the current text.
            current_text_2D_r = text_data[ index ];

            // Steal the pen position and color from the last pen if available.
            current_text_2D_r->stealPen( last_text_2D_r );

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

int Graphics::SDL2::GLES2::Text2DBuffer::print( const std::string &text ) {
    size_t expand_amount;
    size_t expand_sum;
    int add_text_state;

    if( font_system_r != nullptr )
    {
        if( current_text_2D_r != nullptr )
        {
            // Try to add the text.
            add_text_state = current_text_2D_r->addText( text );

            // Just in case of errors.
            if( add_text_state == -1 || add_text_state == -2 )
            {
                // This is a formula used to dynamically expand the text.
                expand_amount = (text.size() / text_2D_expand_factor);

                // If there is a remained then expand this number further by one.
                if((text.size() % text_2D_expand_factor) > 0)
                    expand_amount++;

                // Convert to character amount.
                expand_amount *= text_2D_expand_factor;

                // we get the expand sum.
                expand_sum = current_text_2D_r->getCharAmount() + expand_amount;

                // The text must be expanded
                add_text_state = current_text_2D_r->setTextMax( expand_sum );

                // Check to see if there was an expansion.
                if( add_text_state > 0 )
                {
                    // Attempt to add the text again.
                    add_text_state = current_text_2D_r->addText( text );

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
    int problematic_font = 0;

    if( font_system_r != nullptr )
    {
        if( text_data.size() != 0 )
        {
            for( auto i = text_data.begin(); i != text_data.end(); i++ )
            {
                if( (*i)->clearText( (*i)->getFont() ) != 1 )
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
