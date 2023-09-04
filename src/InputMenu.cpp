#include "InputMenu.h"

#include "MainProgram.h"

#include <chrono>
#include <thread>

InputMenu InputMenu::input_menu;

void InputMenu::load( MainProgram &main_program ) {
    this->input_set_index = 0;
    this->input_set_r = main_program.control_system_p->getInputSet( input_set_index );
    this->input_index = 0;
}

void InputMenu::unload( MainProgram &main_program ) {
    this->input_set_r = nullptr;
}

void InputMenu::grabControls( MainProgram &main_program, std::chrono::microseconds delta ) {

    int status = main_program.control_system_p->pollEventForInputSet( this->input_set_index, this->input_index );

    if( status > 0 ) {
        this->input_index++;

        if( this->input_set_r->getInput( this->input_index ) == nullptr ) {
            this->input_index = 0;
            this->input_set_index++;

            if( this->input_set_index >= main_program.control_system_p->amountOfInputSets() ) {
                main_program.control_system_p->write( name );

                this->menu_r->load( main_program );
                main_program.menu_r = this->menu_r;
                this->unload( main_program );
            }
        }
    }
}

void InputMenu::display( MainProgram &main_program ) {

    Graphics::Text2DBuffer *text_2d_buffer_r = main_program.text_2d_buffer_r;

    if( text_2d_buffer_r->setFont( 3 ) == -3 )
        text_2d_buffer_r->setFont( 1 );
    text_2d_buffer_r->setColor( glm::vec4( 1, 1, 1, 1 ) );
    text_2d_buffer_r->setPosition( glm::vec2( 0, 0 ) );
    text_2d_buffer_r->print( "Input Set: \"" + this->input_set_r->getName() +"\"" );

    if( text_2d_buffer_r->setFont( 6 ) == -3 )
        text_2d_buffer_r->setFont( 1 );
    text_2d_buffer_r->setColor( glm::vec4( 1, 0.25, 0.25, 1 ) );
    text_2d_buffer_r->setPosition( glm::vec2( 0, 20 ) );
    text_2d_buffer_r->print( "Enter a key for Input, \"" + this->input_set_r->getInput( this->input_index )->getName() +"\"" );
}
