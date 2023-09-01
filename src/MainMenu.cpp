#include "MainMenu.h"

#include <glm/vec4.hpp>

#include "MainProgram.h"

#include "MapSelectorMenu.h"
#include "OptionsMenu.h"

#include "PrimaryGame.h"
#include "ModelViewer.h"

MainMenu MainMenu::main_menu;

namespace {
void menuDone( MainProgram &main_program, Menu* menu_r, Menu::Item* ) {
    main_program.menu_r->unload( main_program );
    main_program.menu_r = nullptr;
}

void mapSpectator( MainProgram &main_program, Menu* menu_r, Menu::Item* ) {
    MapSelectorMenu::map_selector_menu.name = "Map Spectator";

    main_program.menu_r->unload( main_program );
    main_program.menu_r = &MapSelectorMenu::map_selector_menu;
    MapSelectorMenu::map_selector_menu.game_r = &PrimaryGame::primary_game;
    main_program.menu_r->load( main_program );
}

void viewGameModels( MainProgram &main_program, Menu* menu_r, Menu::Item* ) {
    MapSelectorMenu::map_selector_menu.name = "Model Viewer";

    main_program.menu_r->unload( main_program );
    main_program.menu_r = &MapSelectorMenu::map_selector_menu;
    MapSelectorMenu::map_selector_menu.game_r = &ModelViewer::model_viewer;
    main_program.menu_r->load( main_program );
}
void options( MainProgram &main_program, Menu*, Menu::Item* ) {
    main_program.menu_r->unload( main_program );
    main_program.menu_r = &OptionsMenu::options_menu;
    main_program.menu_r->load( main_program );
}
void exitGame( MainProgram &main_program, Menu*, Menu::Item* ) {
    main_program.play_loop = false;
}
}

MainMenu::~MainMenu() {

}

void MainMenu::load( MainProgram &main_program ) {
    Menu::load( main_program );

    glm::u32vec2 scale = main_program.getWindowScale();
    uint32_t center = scale.x / 2;

    this->items.clear();

    if( !this->is_game_on ) {
        this->items.emplace_back( new Menu::Item( "Map Spectator",    glm::vec2( center,  0 ), 3, 0, 1, 0, mapSpectator ) );
        this->items.emplace_back( new Menu::Item( "View Game Models", glm::vec2( center, 24 ), 0, 1, 2, 1, viewGameModels ) );
        this->items.emplace_back( new Menu::Item( "Options",          glm::vec2( center, 48 ), 1, 2, 3, 2, options ) );
        this->items.emplace_back( new Menu::Item( "Exit to OS",       glm::vec2( center, 72 ), 2, 3, 0, 3, exitGame ) );
        this->current_item_index = 0;
    }
    else {
        this->items.emplace_back( new Menu::Item( "Back to Session",  glm::vec2( center,  0 ), 4, 0, 1, 0, menuDone ) );
        this->items.emplace_back( new Menu::Item( "Map Spectator",    glm::vec2( center, 24 ), 0, 1, 2, 1, mapSpectator ) );
        this->items.emplace_back( new Menu::Item( "View Game Models", glm::vec2( center, 48 ), 1, 2, 3, 2, viewGameModels ) );
        this->items.emplace_back( new Menu::Item( "Options",          glm::vec2( center, 72 ), 2, 3, 4, 3, options ) );
        this->items.emplace_back( new Menu::Item( "Exit to OS",       glm::vec2( center, 96 ), 3, 4, 0, 4, exitGame ) );
        this->current_item_index = 0;
    }
}

void MainMenu::unload( MainProgram &main_program ) {

}

void MainMenu::display( MainProgram &main_program ) {
    for( size_t i = 0; i < this->items.size(); i++ ) {
        if( this->current_item_index != i )
            this->items[i]->drawNeutral( main_program );
        else
            this->items[i]->drawSelected( main_program );
    }
}
