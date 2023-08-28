#include "MainMenu.h"

#include <glm/vec4.hpp>

#include "MainProgram.h"

#include "MapSelectorMenu.h"

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
    auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
    log.output << "Options not supported yet";
}
void exitGame( MainProgram &main_program, Menu*, Menu::Item* ) {
    main_program.play_loop = false;
}
}
MainMenu::~MainMenu() {

}

void MainMenu::load( MainProgram &main_program ) {
    Menu::load( main_program );
    if( !this->is_game_on ) {
        this->items.resize( 4 );
        this->items[0] = Menu::Item( "Map Spectator",    glm::vec2( 0,  0 ), &items[3], nullptr, &items[1], nullptr, mapSpectator );
        this->items[1] = Menu::Item( "View Game Models", glm::vec2( 0, 24 ), &items[0], nullptr, &items[2], nullptr, viewGameModels );
        this->items[2] = Menu::Item( "Options",          glm::vec2( 0, 48 ), &items[1], nullptr, &items[3], nullptr, options );
        this->items[3] = Menu::Item( "Exit to OS",       glm::vec2( 0, 72 ), &items[2], nullptr, &items[0], nullptr, exitGame );
        this->current_item_r = &items[0];
    }
    else {
        this->items.resize( 5 );
        this->items[0] = Menu::Item( "Back to Session",  glm::vec2( 0,  0 ), &items[4], nullptr, &items[1], nullptr, menuDone );
        this->items[1] = Menu::Item( "Map Spectator",    glm::vec2( 0, 24 ), &items[0], nullptr, &items[2], nullptr, mapSpectator );
        this->items[2] = Menu::Item( "View Game Models", glm::vec2( 0, 48 ), &items[1], nullptr, &items[3], nullptr, viewGameModels );
        this->items[3] = Menu::Item( "Options",          glm::vec2( 0, 72 ), &items[2], nullptr, &items[4], nullptr, options );
        this->items[4] = Menu::Item( "Exit to OS",       glm::vec2( 0, 96 ), &items[3], nullptr, &items[0], nullptr, exitGame );
        this->current_item_r = &items[0];
    }
}

void MainMenu::unload( MainProgram &main_program ) {

}

void MainMenu::display( MainProgram &main_program ) {
    for( size_t i = 0; i < this->items.size(); i++ ) {
        drawButton( main_program, this->items[i] );
    }
}
