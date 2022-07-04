#include "../../Text2DBuffer.h" // Include the interface class
#include "Text2DBuffer.h" // Include the internal class
#include "../../Environment.h" // Include the interface Environment class
#include "Environment.h" // Include the internal Environment class
#include "cassert"

Graphics::Text2DBuffer::Text2DBuffer( unsigned int buffer_size_per_font_KiB ) {
    auto internal_data_p = new Graphics::SDL2::GLES2::Text2DBufferInternalData;
    const size_t KIBIBYTE_TO_BYTE = 1024;
    const size_t VERTICES_PER_CHARACTER = 6;

    // To be set by the environment when this buffer gets attached.
    internal_data_p->font_system_r = nullptr;
    internal_data_p->current_text_2D_r = nullptr;
    internal_data_p->buffer_size_per_font_KiB = buffer_size_per_font_KiB;
    internal_data_p->text_2D_expand_factor = (buffer_size_per_font_KiB * KIBIBYTE_TO_BYTE) / (Graphics::SDL2::GLES2::Internal::FontSystem::getVertexSize() * VERTICES_PER_CHARACTER);

    // To be sure that the text_2D_expand_factor will not be zero.
    if( internal_data_p->text_2D_expand_factor < 0x100 )
        internal_data_p->text_2D_expand_factor = 0x100; // Clamp to 256 because any lower than this could really affect the speed of execution.

    text_2d_buffer_internal_data_p = reinterpret_cast<void*>( internal_data_p );
}

Graphics::Text2DBuffer::~Text2DBuffer() {
    auto internal_data_p = reinterpret_cast<Graphics::SDL2::GLES2::Text2DBufferInternalData*>( text_2d_buffer_internal_data_p );

    if( internal_data_p != nullptr )
    {
        for( auto i = internal_data_p->text_data.begin(); i != internal_data_p->text_data.end(); i++ )
            delete (*i);

        delete internal_data_p;
    }
}

bool Graphics::Text2DBuffer::loadFontLibrary( Graphics::Environment &environment )
{
    auto internal_data_p = reinterpret_cast<Graphics::SDL2::GLES2::Text2DBufferInternalData *>( text_2d_buffer_internal_data_p );
    auto environment_internal_data_r = reinterpret_cast<Graphics::SDL2::GLES2::EnvironmentInternalData *>( environment.getInternalData() );

    // Make sure there is text.
    if( environment_internal_data_r->text_draw_routine != nullptr )
    {
        internal_data_p->font_system_r = environment_internal_data_r->text_draw_routine;

        if( internal_data_p->font_system_r->getNumFonts() > 0 )
        {
            internal_data_p->text_data.reserve( internal_data_p->font_system_r->getNumFonts() );

            for( int i = 0; i < internal_data_p->font_system_r->getNumFonts(); i++ )
            {
                auto font_r = internal_data_p->font_system_r->accessFont( i );

                assert( font_r != nullptr );

                internal_data_p->text_data.push_back( font_r->allocateText2D() );

                assert( font_r == internal_data_p->text_data[i]->getFont() );
            }

            return true;
        }
        else
            return false;
    }
    else
        return false;
}

int Graphics::Text2DBuffer::setFont( unsigned index ) {
    auto internal_data_r = reinterpret_cast<Graphics::SDL2::GLES2::Text2DBufferInternalData*>( text_2d_buffer_internal_data_p );
    auto last_text_2D_r = internal_data_r->current_text_2D_r;

    if( internal_data_r->font_system_r != nullptr )
    {
        if( internal_data_r->text_data.size() > index )
        {
            // Set the current text.
            internal_data_r->current_text_2D_r = internal_data_r->text_data[ index ];

            // Steal the pen position and color from the last pen if available.
            internal_data_r->current_text_2D_r->stealPen( last_text_2D_r );

            // Successfully set the current font.
            return 1;
        }
        else
            return -3; // aborted because the index is out of bounds.
    }
    else
        return -1;
}

int Graphics::Text2DBuffer::setPosition( const glm::vec2 &position ) {
    auto internal_data_r = reinterpret_cast<Graphics::SDL2::GLES2::Text2DBufferInternalData*>( text_2d_buffer_internal_data_p );

    if( internal_data_r->font_system_r != nullptr )
    {
        if( internal_data_r->current_text_2D_r != nullptr )
        {
            // The pen position is to be set.
            internal_data_r->current_text_2D_r->setPenPosition( position );

            // For the success of changing the color.
            return 1;
        }
        else
            return -2; // aborted current_text_2d_r is not selected.
    }
    else
        return -1;
}

int Graphics::Text2DBuffer::setColor( const glm::vec4 &color ) {
    auto internal_data_r = reinterpret_cast<Graphics::SDL2::GLES2::Text2DBufferInternalData*>( text_2d_buffer_internal_data_p );
    uint8_t color_rgba[4];

    if( internal_data_r->font_system_r != nullptr )
    {
        if( internal_data_r->current_text_2D_r != nullptr )
        {
            color_rgba[ 0 ] = color.x * 255.0;
            color_rgba[ 1 ] = color.y * 255.0;
            color_rgba[ 2 ] = color.z * 255.0;
            color_rgba[ 3 ] = color.w * 255.0;

            internal_data_r->current_text_2D_r->setPenColor( color_rgba );

            // For the success of changing the color.
            return 1;
        }
        else
            return -2; // aborted current_text_2d_r is not selected.
    }
    else
        return -1;
}

int Graphics::Text2DBuffer::print( const std::string &text ) {
    auto internal_data_r = reinterpret_cast<Graphics::SDL2::GLES2::Text2DBufferInternalData*>( text_2d_buffer_internal_data_p );
    size_t expand_amount;
    size_t expand_sum;
    int add_text_state;

    if( internal_data_r->font_system_r != nullptr )
    {
        if( internal_data_r->current_text_2D_r != nullptr )
        {
            // Try to add the text.
            add_text_state = internal_data_r->current_text_2D_r->addText( text );

            // Just in case of errors.
            if( add_text_state == -1 || add_text_state == -2 )
            {
                // This is a formula used to dynamically expand the text.
                expand_amount = (text.size() / internal_data_r->text_2D_expand_factor);

                // If there is a remained then expand this number further by one.
                if((text.size() % internal_data_r->text_2D_expand_factor) > 0)
                    expand_amount++;

                // Convert to character amount.
                expand_amount *= internal_data_r->text_2D_expand_factor;

                // we get the expand sum.
                expand_sum = internal_data_r->current_text_2D_r->getCharAmount() + expand_amount;

                // The text must be expanded
                add_text_state = internal_data_r->current_text_2D_r->setTextMax( expand_sum );

                // Check to see if there was an expansion.
                if( add_text_state > 0 )
                {
                    // Attempt to add the text again.
                    add_text_state = internal_data_r->current_text_2D_r->addText( text );

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

int Graphics::Text2DBuffer::reset() {
    auto internal_data_r = reinterpret_cast<Graphics::SDL2::GLES2::Text2DBufferInternalData*>( text_2d_buffer_internal_data_p );
    int problematic_font = 0;

    if( internal_data_r->font_system_r != nullptr )
    {
        if( internal_data_r->text_data.size() != 0 )
        {
            for( auto i = internal_data_r->text_data.begin(); i != internal_data_r->text_data.end(); i++ )
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
