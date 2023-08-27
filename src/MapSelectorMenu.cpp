#include "MapSelectorMenu.h"

#include <glm/vec4.hpp>

#include "MainProgram.h"

#include <iostream>

namespace {
void nullPress( MainProgram &main_program, Menu*, Menu::Item* ) {
    // Nothing.
}
void mapSelect( MainProgram &main_program, Menu* menu_r, Menu::Item* item_r ) {
    std::cout << "Map selected = " << item_r->name << std::endl;
    if( main_program.primary_game_r != nullptr )
        main_program.primary_game_r->unload( main_program );

    main_program.primary_game_r = dynamic_cast<MapSelectorMenu*>(menu_r)->game_r;
    main_program.primary_game_r->load( main_program );
    main_program.menu_r = nullptr;
    main_program.transitionToResource( item_r->name, main_program.platform );
}
}

MapSelectorMenu MapSelectorMenu::map_selector_menu;

MapSelectorMenu::~MapSelectorMenu() {
}

void MapSelectorMenu::load( MainProgram &main_program ) {
    Menu::load( main_program );

    this->items.resize( Data::Manager::AMOUNT_OF_IFF_IDS + 1 );

    this->items[ Data::Manager::AMOUNT_OF_IFF_IDS ] = Menu::Item( this->name, glm::vec2( 0, 0 ), nullptr, nullptr, nullptr, nullptr, nullPress );

    this->items[0] = Menu::Item( *Data::Manager::map_iffs[0], glm::vec2( 0, 24 ), &items[ Data::Manager::AMOUNT_OF_IFF_IDS - 1 ], nullptr, &items[ 1 ], nullptr, mapSelect );
    for( size_t i = 1; i < Data::Manager::AMOUNT_OF_IFF_IDS; i++ )
        this->items[i] = Menu::Item( *Data::Manager::map_iffs[i], glm::vec2( 0, (i + 1) * 24 ), &items[ (i - 1) % Data::Manager::AMOUNT_OF_IFF_IDS ], nullptr, &items[ (i + 1) % Data::Manager::AMOUNT_OF_IFF_IDS ], nullptr, mapSelect );

    this->current_item_r = &items[0];
}

void MapSelectorMenu::unload( MainProgram &main_program ) {

}

void MapSelectorMenu::display( MainProgram &main_program ) {
    for( size_t i = 0; i < this->items.size(); i++ ) {
        drawButton( main_program, this->items[i] );
    }
}
