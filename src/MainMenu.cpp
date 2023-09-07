#include "MainMenu.h"

#include <glm/vec4.hpp>

#include "MainProgram.h"

#include "MapSelectorMenu.h"
#include "OptionsMenu.h"

#include "PrimaryGame.h"
#include "ModelViewer.h"

#include "Config.h"

MainMenu MainMenu::main_menu;
Menu::ItemClickSwitchMenu MainMenu::item_click_main_menu( &MainMenu::main_menu );

namespace {
class ItemClickToGame : public Menu::ItemClick {
private:
    std::string name;
    GameState *game_r;

public:
    ItemClickToGame( std::string p_name, GameState *p_game_r ) : name( p_name ), game_r( p_game_r ) {}

    virtual void onPress( MainProgram &main_program, Menu* menu_r, Menu::Item* ) {
        MapSelectorMenu::map_selector_menu.name = name;

        main_program.menu_r->unload( main_program );

        if( main_program.resource_identifier != MainProgram::CUSTOM_IDENTIFIER ) {
            main_program.menu_r = &MapSelectorMenu::map_selector_menu;
            MapSelectorMenu::map_selector_menu.game_r = game_r;
            main_program.menu_r->load( main_program );
        }
        else {
            if( main_program.primary_game_r != nullptr )
                main_program.primary_game_r->unload( main_program );

            main_program.primary_game_r = game_r;
            main_program.primary_game_r->load( main_program );
            main_program.menu_r = nullptr;
            main_program.transitionToResource( MainProgram::CUSTOM_IDENTIFIER, main_program.platform );
        }
    }
}
item_click_map_spectator(   "Map Spectator", &PrimaryGame::primary_game ),
item_click_view_game_models( "Model Viewer", &ModelViewer::model_viewer );


class ItemClickExitGame : public Menu::ItemClick {
public:
    virtual void onPress( MainProgram &main_program, Menu* menu_r, Menu::Item* ) {
        main_program.play_loop = false;
    }
} item_click_exit_game;

Menu::ItemClickSwitchMenu item_click_menu_done( nullptr );
Menu::ItemClickSwitchMenu item_click_options( &OptionsMenu::options_menu );

}

MainMenu::~MainMenu() {

}

void MainMenu::load( MainProgram &main_program ) {
    Menu::load( main_program );

    glm::u32vec2 scale = main_program.getWindowScale();
    uint32_t center = scale.x / 2;

    const Graphics::Text2DBuffer::CenterMode right_mode = Graphics::Text2DBuffer::CenterMode::RIGHT;

    Graphics::Text2DBuffer::Font title_font;
    Graphics::Text2DBuffer::Font prime_font;
    Graphics::Text2DBuffer::Font selected_font;
    Graphics::Text2DBuffer::Font spec_detail_font;

    const unsigned step = 2. / 30. * static_cast<float>( scale.y );

    if( !main_program.text_2d_buffer_r->selectFont( title_font, 1.5 * step, 2. * step ) )
        title_font = {1, 4.0};

    if( !main_program.text_2d_buffer_r->selectFont( prime_font, 0.8 * step, 0.9 * step - 1 ) )
        prime_font = {1, 2.0};

    if( !main_program.text_2d_buffer_r->selectFont( selected_font, 0.9 * step, step ) )
        selected_font = {1, 2.25};

    if( !main_program.text_2d_buffer_r->selectFont( spec_detail_font, 1, 14 ) )
        spec_detail_font = 1;

    this->items.clear();

    if( !this->is_game_on ) {
        this->items.emplace_back( new Menu::TextButton( "Map Spectator",    glm::vec2( center, scale.y - 6 * step ), 3, 0, 1, 0, &item_click_map_spectator,    prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "View Game Models", glm::vec2( center, scale.y - 5 * step ), 0, 1, 2, 1, &item_click_view_game_models, prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "Options",          glm::vec2( center, scale.y - 4 * step ), 1, 2, 3, 2, &item_click_options,          prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "Exit to OS",       glm::vec2( center, scale.y - 3 * step ), 2, 3, 0, 3, &item_click_exit_game,        prime_font, selected_font ) );

        if( main_program.text_2d_buffer_r->getLineLength( title_font, "Future Cop: MIT" ) > scale.x ) {
            this->items.emplace_back( new Menu::TextButton( "Future Cop:",  glm::vec2( center, 3 * step ), 0, 0, 0, 0, &Menu::null_item_click, title_font, title_font ) );
            this->items.emplace_back( new Menu::TextButton( "MIT",          glm::vec2( center, 5 * step ), 0, 0, 0, 0, &Menu::null_item_click, title_font, title_font ) );
        }
        else
            this->items.emplace_back( new Menu::TextButton( "Future Cop: MIT",  glm::vec2( center, 3 * step ), 0, 0, 0, 0, &Menu::null_item_click, title_font, title_font ) );

        // If any resource is missing I am sure the user would want to know about it.
        if( main_program.global_r == nullptr || main_program.resource_r == nullptr )
            this->items.emplace_back( new Menu::TextButton( "Not all resources are found",  glm::vec2( center, 7 * step ), 0, 0, 0, 0, &Menu::null_item_click, prime_font, prime_font ) );

        this->items.emplace_back( new Menu::TextButton( FUTURE_COP_MIT_VERSION, glm::vec2( scale.x, scale.y - 14), 0, 0, 0, 0, &Menu::null_item_click, spec_detail_font, spec_detail_font, right_mode ) );
        this->current_item_index = 0;
    }
    else {
        this->items.emplace_back( new Menu::TextButton( "Back to Session",  glm::vec2( center, scale.y - 8 * step ), 4, 0, 1, 0, &item_click_menu_done,        prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "Map Spectator",    glm::vec2( center, scale.y - 6 * step ), 0, 1, 2, 1, &item_click_map_spectator,    prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "View Game Models", glm::vec2( center, scale.y - 5 * step ), 1, 2, 3, 2, &item_click_view_game_models, prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "Options",          glm::vec2( center, scale.y - 4 * step ), 2, 3, 4, 3, &item_click_options,          prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "Exit to OS",       glm::vec2( center, scale.y - 3 * step ), 3, 4, 0, 4, &item_click_exit_game,        prime_font, selected_font ) );
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
