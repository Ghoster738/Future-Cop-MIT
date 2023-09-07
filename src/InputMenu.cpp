#include "InputMenu.h"

#include "MainProgram.h"

#include "Graphics/Environment.h"
#include "Graphics/Text2DBuffer.h"

#include <chrono>
#include <thread>

InputMenu InputMenu::input_menu;

void InputMenu::load( MainProgram &main_program ) {
    this->input_set_index = 0;
    this->input_set_r = main_program.control_system_p->getInputSet( input_set_index );
    this->input_index = 0;

    glm::u32vec2 scale = main_program.getWindowScale();
    this->center = scale / glm::u32vec2( 2 );
    this->step = 0.05 * static_cast<float>( scale.y );

    if( !main_program.text_2d_buffer_r->selectFont( this->input_set_font, 0.9 * step, step ) ) {
        this->input_set_font = 1;

        main_program.text_2d_buffer_r->scaleFont( this->input_set_font, step );
    }
}

void InputMenu::unload( MainProgram &main_program ) {
    this->input_set_r = nullptr;
}

void InputMenu::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    if( main_program.control_system_p->isOrderedToExit() )
        main_program.play_loop = false;

    int status = main_program.control_system_p->pollEventForInputSet( this->input_set_index, this->input_index );

    if( status > 0 ) {
        this->input_index++;

        if( this->input_set_r->getInput( this->input_index ) == nullptr ) {
            this->input_index = 0;
            this->input_set_index++;

            if( this->input_set_index >= main_program.control_system_p->amountOfInputSets() ) {
                main_program.control_system_p->write( name );

                main_program.switchMenu( this->menu_r );
            }
        }
    }

    Graphics::Text2DBuffer *text_2d_buffer_r = main_program.text_2d_buffer_r;

    text_2d_buffer_r->setFont( this->input_set_font );
    text_2d_buffer_r->setCenterMode( Graphics::Text2DBuffer::CenterMode::MIDDLE );
    text_2d_buffer_r->setColor( glm::vec4( 1, 1, 1, 1 ) );
    text_2d_buffer_r->setPosition( glm::vec2( this->center.x, this->center.y - 2 * this->step ) );
    text_2d_buffer_r->print( "Input Set: " + this->input_set_r->getName() );

    text_2d_buffer_r->setColor( glm::vec4( 1, 0.25, 0.25, 1 ) );
    text_2d_buffer_r->setPosition( this->center );
    text_2d_buffer_r->print( "Enter a key for Input" );

    text_2d_buffer_r->setColor( glm::vec4( 1, 1, 0.25, 1 ) );
    text_2d_buffer_r->setPosition( glm::vec2( this->center.x, this->center.y + 1 * this->step ) );
    text_2d_buffer_r->print( this->input_set_r->getInput( this->input_index )->getName() );
}
