#include "MainMenu.h"

#include <glm/vec4.hpp>

#include "MainProgram.h"

#include <iostream>

MainMenu MainMenu::main_menu;

namespace {
void mapSpectator( MainProgram &main_program, Menu* menu_r, Menu::Item* ) {
    std::cout << "Map Spectator" << std::endl;
    main_program.menu_r->unload( main_program );
    main_program.menu_r = &dynamic_cast<MainMenu*>(menu_r)->map_selector_menu;
    dynamic_cast<MapSelectorMenu*>(main_program.menu_r)->name = "Map Spectator Menu";
    dynamic_cast<MapSelectorMenu*>(main_program.menu_r)->game_r = dynamic_cast<MainMenu*>(menu_r)->primary_game_r;
    main_program.menu_r->load( main_program );
}
void viewGameModels( MainProgram &main_program, Menu* menu_r, Menu::Item* ) {
    std::cout << "View Game Models" << std::endl;
    main_program.menu_r->unload( main_program );
    main_program.menu_r = &dynamic_cast<MainMenu*>(menu_r)->map_selector_menu;
    dynamic_cast<MapSelectorMenu*>(main_program.menu_r)->name = "Game Model Menu";
    dynamic_cast<MapSelectorMenu*>(main_program.menu_r)->game_r = dynamic_cast<MainMenu*>(menu_r)->model_viewer_r;
    main_program.menu_r->load( main_program );
}
void options( MainProgram &main_program, Menu*, Menu::Item* ) {
    std::cout << "Options not supported yet" << std::endl;
}
void exitGame( MainProgram &main_program, Menu*, Menu::Item* ) {
    main_program.play_loop = false;
}
}
MainMenu::~MainMenu() {

}

void MainMenu::load( MainProgram &main_program ) {
    Menu::load( main_program );
    this->items.resize( 4 );
    this->items[0] = Menu::Item( "Map Spectator",    glm::vec2( 0,  0 ), &items[3], nullptr, &items[1], nullptr, mapSpectator );
    this->items[1] = Menu::Item( "View Game Models", glm::vec2( 0, 24 ), &items[0], nullptr, &items[2], nullptr, viewGameModels );
    this->items[2] = Menu::Item( "Options",          glm::vec2( 0, 48 ), &items[1], nullptr, &items[3], nullptr, options );
    this->items[3] = Menu::Item( "Exit to OS",       glm::vec2( 0, 72 ), &items[2], nullptr, &items[0], nullptr, exitGame );
    this->current_item_r = &items[0];
}

void MainMenu::unload( MainProgram &main_program ) {

}

void MainMenu::display( MainProgram &main_program ) {
    const auto text_2d_buffer_r = main_program.text_2d_buffer_r;

    for( size_t i = 0; i < this->items.size(); i++ ) {
        drawButton( main_program, this->items[i] );
    }
}
