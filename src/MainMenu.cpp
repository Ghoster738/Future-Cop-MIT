#include "MainMenu.h"

#include <glm/vec4.hpp>

#include "MainProgram.h"

#include <iostream>

namespace {
void mapSpectator( Menu*, Menu::Item* ) {
    std::cout << "Map Spectator" << std::endl;
}
void viewGameModels( Menu*, Menu::Item* ) {
    std::cout << "View Game Models" << std::endl;
}
void options( Menu*, Menu::Item* ) {
    std::cout << "Options" << std::endl;
}
void exitGame( Menu*, Menu::Item* ) {
    std::cout << "Exits" << std::endl;
}
}
MainMenu::~MainMenu() {

}

void MainMenu::load( MainProgram &main_program ) {
    Menu::load( main_program );
    this->items.resize( 4 );
    this->items[0] = Menu::Item( "Map Spectator",    glm::vec2( 0,  0 ), &items[3], nullptr, &items[1], nullptr, mapSpectator );
    this->items[1] = Menu::Item( "View Game Models", glm::vec2( 0, 16 ), &items[0], nullptr, &items[2], nullptr, viewGameModels );
    this->items[2] = Menu::Item( "Options",          glm::vec2( 0, 32 ), &items[1], nullptr, &items[3], nullptr, options );
    this->items[3] = Menu::Item( "Exits",            glm::vec2( 0, 48 ), &items[2], nullptr, &items[0], nullptr, exitGame );
    this->current_item_r = &items[0];
}

void MainMenu::unload( MainProgram &main_program ) {

}

void MainMenu::display( MainProgram &main_program ) {
    const auto text_2d_buffer_r = main_program.text_2d_buffer_r;

    if( text_2d_buffer_r->setFont( 5 ) == -3 )
        text_2d_buffer_r->setFont( 2 );

    for( size_t i = 0; i < this->items.size(); i++ ) {
        if( &this->items[ i ] != this->current_item_r )
            text_2d_buffer_r->setColor( glm::vec4( 1, 1, 1, 1 ) );
        else
            text_2d_buffer_r->setColor( glm::vec4( 0, 1, 1, 1 ) );

        text_2d_buffer_r->setPosition( this->items[i].position );
        text_2d_buffer_r->print( this->items[i].name );
    }
}
