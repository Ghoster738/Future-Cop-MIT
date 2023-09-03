#include "MapSelectorMenu.h"

#include <glm/vec4.hpp>

#include "MainProgram.h"
#include "MainMenu.h"

namespace {
void nullPress( MainProgram &main_program, Menu*, Menu::Item* ) {
    // Nothing.
}
void exitMapSelector( MainProgram &main_program, Menu* menu_r, Menu::Item* item_r ) {
    if( main_program.menu_r != nullptr )
        main_program.menu_r->unload( main_program );

    main_program.menu_r = &MainMenu::main_menu;
    main_program.menu_r->load( main_program );
}
void mapSelect( MainProgram &main_program, Menu* menu_r, Menu::Item* item_r ) {
    if( !Utilities::Options::Tools::isFile( main_program.manager.getIFFEntry( item_r->name ).getPath( main_program.platform ) ) )
        dynamic_cast<MapSelectorMenu*>(menu_r)->failed_map_name = item_r->name;
    else {
        if( main_program.primary_game_r != nullptr )
            main_program.primary_game_r->unload( main_program );

        main_program.primary_game_r = dynamic_cast<MapSelectorMenu*>(menu_r)->game_r;
        main_program.primary_game_r->load( main_program );
        main_program.menu_r = nullptr;
        main_program.transitionToResource( item_r->name, main_program.platform );
    }
}
}

MapSelectorMenu MapSelectorMenu::map_selector_menu;

MapSelectorMenu::~MapSelectorMenu() {
}

void MapSelectorMenu::load( MainProgram &main_program ) {
    Menu::load( main_program );

    failed_map_name = "";

    glm::u32vec2 scale = main_program.getWindowScale();
    uint32_t center = scale.x / 2;

    this->items.clear();

    const unsigned back  = Data::Manager::AMOUNT_OF_IFF_IDS + 0;
    const unsigned title = Data::Manager::AMOUNT_OF_IFF_IDS + 1;

    const unsigned step = static_cast<float>( scale.y ) / static_cast<float>( Data::Manager::AMOUNT_OF_IFF_IDS + 2);

    Graphics::Text2DBuffer::Font prime_font;
    Graphics::Text2DBuffer::Font selected_font;

    if( !main_program.text_2d_buffer_r->selectFont( prime_font, 0.8 * step, 0.9 * step - 1 ) )
        prime_font = {1, 2.0};

    if( !main_program.text_2d_buffer_r->selectFont( selected_font, 0.9 * step, step - 1 ) )
        selected_font = {1, 2.25};

    this->items.emplace_back( new Menu::TextButton( *Data::Manager::map_iffs[0], glm::vec2( center, step ), Data::Manager::AMOUNT_OF_IFF_IDS, 0, 1, 0, mapSelect, prime_font, selected_font ) );
    for( size_t i = 1; i < Data::Manager::AMOUNT_OF_IFF_IDS - 1; i++ )
        this->items.emplace_back( new Menu::TextButton( *Data::Manager::map_iffs[i], glm::vec2( center, (i + 1) * step ), (i - 1) % Data::Manager::AMOUNT_OF_IFF_IDS, i, (i + 1) % Data::Manager::AMOUNT_OF_IFF_IDS, i, mapSelect, prime_font, selected_font ) );
    this->items.emplace_back( new Menu::TextButton( *Data::Manager::map_iffs[ Data::Manager::AMOUNT_OF_IFF_IDS - 1 ], glm::vec2( center, Data::Manager::AMOUNT_OF_IFF_IDS * step ), Data::Manager::AMOUNT_OF_IFF_IDS - 2, Data::Manager::AMOUNT_OF_IFF_IDS - 1, Data::Manager::AMOUNT_OF_IFF_IDS, Data::Manager::AMOUNT_OF_IFF_IDS - 1, mapSelect, prime_font, selected_font ) );

    this->items.emplace_back( new Menu::TextButton( "Back",     glm::vec2( center, (Data::Manager::AMOUNT_OF_IFF_IDS + 1) * step ), Data::Manager::AMOUNT_OF_IFF_IDS - 1, back, 0, back, exitMapSelector, prime_font, selected_font ) );
    this->items.emplace_back( new Menu::TextButton( this->name, glm::vec2( center, 0 ), title, title, title, title, nullPress, prime_font, selected_font ) );

    this->current_item_index = 0;
}

void MapSelectorMenu::unload( MainProgram &main_program ) {

}

void MapSelectorMenu::display( MainProgram &main_program ) {
    const auto text_2d_buffer_r = main_program.text_2d_buffer_r;

    if( !failed_map_name.empty() ) {
        text_2d_buffer_r->setFont( 2 );
        text_2d_buffer_r->setColor( glm::vec4( 0.7, 0, 0.5, 1 ) );
        text_2d_buffer_r->setPosition( glm::vec2( 150, 3 ) );
        text_2d_buffer_r->print( "Game files are not found!" );
        text_2d_buffer_r->setColor( glm::vec4( 1, 1, 1, 1 ) );
        text_2d_buffer_r->setPosition( glm::vec2( 150, 13 ) );
        text_2d_buffer_r->print( "This project requires these game files to run." );
        text_2d_buffer_r->setPosition( glm::vec2( 150, 35 ) );
        text_2d_buffer_r->print( "This is the path that this program expects" );
        text_2d_buffer_r->setPosition( glm::vec2( 150, 47 ) );
        text_2d_buffer_r->print( "relative to the file location of this program." );

        auto entry = main_program.manager.getIFFEntry( failed_map_name );
        text_2d_buffer_r->setColor( glm::vec4( 0.5, 1, 0.5, 1 ) );
        text_2d_buffer_r->setPosition( glm::vec2( 150, 59 ) );
        text_2d_buffer_r->print( entry.getPath( main_program.platform ) );

        entry = main_program.manager.getIFFEntry( Data::Manager::global );

        if( entry.getIFF( main_program.platform ) == nullptr ) {
            text_2d_buffer_r->setColor( glm::vec4( 1, 1, 1, 1 ) );
            text_2d_buffer_r->setPosition( glm::vec2( 150, 83 ) );
            text_2d_buffer_r->print( "The globals file is also missing." );
            text_2d_buffer_r->setColor( glm::vec4( 0.5, 1, 0.5, 1 ) );
            text_2d_buffer_r->setPosition( glm::vec2( 150, 95 ) );
            text_2d_buffer_r->print( entry.getPath( main_program.platform ) );
        }
    }

    for( size_t i = 0; i < this->items.size(); i++ ) {
        if( this->current_item_index != i )
            this->items[i]->drawNeutral( main_program );
        else
            this->items[i]->drawSelected( main_program );
    }
}
