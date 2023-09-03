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

    Graphics::Text2DBuffer::Font title_font;
    Graphics::Text2DBuffer::Font prime_font;
    Graphics::Text2DBuffer::Font selected_font;

    const unsigned step = 2. / 30. * static_cast<float>( scale.y );

    if( !main_program.text_2d_buffer_r->selectFont( title_font, 1.5 * step, 2. * step ) )
        title_font = {1, 4.0};

    if( !main_program.text_2d_buffer_r->selectFont( prime_font, 0.8 * step, 0.9 * step - 1 ) )
        prime_font = {1, 2.0};

    if( !main_program.text_2d_buffer_r->selectFont( selected_font, 0.9 * step, step ) )
        selected_font = {1, 2.25};

    this->items.clear();

    if( !this->is_game_on ) {
        this->items.emplace_back( new Menu::TextButton( "Map Spectator",    glm::vec2( center, scale.y - 6 * step ), 3, 0, 1, 0, mapSpectator,   prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "View Game Models", glm::vec2( center, scale.y - 5 * step ), 0, 1, 2, 1, viewGameModels, prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "Options",          glm::vec2( center, scale.y - 4 * step ), 1, 2, 3, 2, options,        prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "Exit to OS",       glm::vec2( center, scale.y - 3 * step ), 2, 3, 0, 3, exitGame,       prime_font, selected_font ) );

        this->items.emplace_back( new Menu::TextButton( "Future Cop:",  glm::vec2( center, 3 * step ), 0, 0, 0, 0, mapSpectator, title_font, title_font ) );
        this->items.emplace_back( new Menu::TextButton( "MIT",          glm::vec2( center, 5 * step ), 0, 0, 0, 0, mapSpectator, title_font, title_font ) );
        this->current_item_index = 0;
    }
    else {
        this->items.emplace_back( new Menu::TextButton( "Back to Session",  glm::vec2( center, scale.y - 8 * step ), 4, 0, 1, 0, menuDone,       prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "Map Spectator",    glm::vec2( center, scale.y - 6 * step ), 0, 1, 2, 1, mapSpectator,   prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "View Game Models", glm::vec2( center, scale.y - 5 * step ), 1, 2, 3, 2, viewGameModels, prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "Options",          glm::vec2( center, scale.y - 4 * step ), 2, 3, 4, 3, options,        prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "Exit to OS",       glm::vec2( center, scale.y - 3 * step ), 3, 4, 0, 4, exitGame,       prime_font, selected_font ) );
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
