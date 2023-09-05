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

    const unsigned line_height = static_cast<float>( scale.y ) / static_cast<float>( Data::Manager::AMOUNT_OF_IFF_IDS + 3);
    this->error_line_height = 0.025 * static_cast<float>( scale.y );

    Graphics::Text2DBuffer::Font title_font;
    Graphics::Text2DBuffer::Font prime_font;
    Graphics::Text2DBuffer::Font selected_font;

    if( !main_program.text_2d_buffer_r->selectFont( title_font, line_height, line_height * 2 - 1 ) ) {
        title_font = 1;

        main_program.text_2d_buffer_r->scaleFont( title_font, line_height - 1 );
    }

    if( !main_program.text_2d_buffer_r->selectFont( prime_font, 0.7 * line_height, 0.9 * line_height ) ) {
        prime_font = 1;

        main_program.text_2d_buffer_r->scaleFont( prime_font, 0.9 * line_height- 1 );
    }

    if( !main_program.text_2d_buffer_r->selectFont( selected_font, 0.9 * line_height, line_height - 1 ) ) {
        selected_font = 1;

        main_program.text_2d_buffer_r->scaleFont( selected_font, line_height - 1 );
    }

    if( !main_program.text_2d_buffer_r->selectFont( this->error_font, 0.75 * this->error_line_height, this->error_line_height ) ) {
        this->error_font = 1;

        main_program.text_2d_buffer_r->scaleFont( this->error_font, this->error_line_height );

        // Small bitmap font should not be shrunk.
        if( error_font.scale < 1 ) {
            this->error_line_height = this->error_line_height / this->error_font.scale;
            this->error_font.scale = 1;
        }
    }

    const Graphics::Text2DBuffer::CenterMode left_mode  = Graphics::Text2DBuffer::CenterMode::LEFT;

    this->items.emplace_back( new Menu::TextButton( *Data::Manager::map_iffs[0], glm::vec2( 0, 2 * line_height ), Data::Manager::AMOUNT_OF_IFF_IDS, 0, 1, 0, mapSelect, prime_font, selected_font, left_mode ) );
    for( size_t i = 1; i < Data::Manager::AMOUNT_OF_IFF_IDS - 1; i++ )
        this->items.emplace_back( new Menu::TextButton( *Data::Manager::map_iffs[i], glm::vec2( 0, (i + 2) * line_height ), (i - 1) % Data::Manager::AMOUNT_OF_IFF_IDS, i, (i + 1) % Data::Manager::AMOUNT_OF_IFF_IDS, i, mapSelect, prime_font, selected_font, left_mode ) );
    this->items.emplace_back( new Menu::TextButton( *Data::Manager::map_iffs[ Data::Manager::AMOUNT_OF_IFF_IDS - 1 ], glm::vec2( 0, (Data::Manager::AMOUNT_OF_IFF_IDS + 1) * line_height ), Data::Manager::AMOUNT_OF_IFF_IDS - 2, Data::Manager::AMOUNT_OF_IFF_IDS - 1, Data::Manager::AMOUNT_OF_IFF_IDS, Data::Manager::AMOUNT_OF_IFF_IDS - 1, mapSelect, prime_font, selected_font, left_mode ) );

    this->items.emplace_back( new Menu::TextButton( "Back",     glm::vec2( 0, (Data::Manager::AMOUNT_OF_IFF_IDS + 2) * line_height ), Data::Manager::AMOUNT_OF_IFF_IDS - 1, back, 0, back, exitMapSelector, prime_font, selected_font, left_mode ) );

    this->placement.x = 0;
    this->placement.y = line_height * 2;
    for( size_t i = 0; i < this->items.size(); i++ ) {
        this->placement.x = std::max( this->placement.x, (unsigned)main_program.text_2d_buffer_r->getLineLength( prime_font, this->items[i]->name ) );
    }

    this->items.emplace_back( new Menu::TextButton( this->name, glm::vec2( center, 0 ), title, title, title, title, nullPress, title_font, title_font ) );

    this->current_item_index = 0;
}

void MapSelectorMenu::unload( MainProgram &main_program ) {

}

void MapSelectorMenu::display( MainProgram &main_program ) {
    const auto text_2d_buffer_r = main_program.text_2d_buffer_r;

    if( !failed_map_name.empty() ) {
        text_2d_buffer_r->setFont( this->error_font );
        text_2d_buffer_r->setCenterMode( Graphics::Text2DBuffer::CenterMode::LEFT );
        text_2d_buffer_r->setColor( glm::vec4( 0.7, 0, 0.5, 1 ) );
        text_2d_buffer_r->setPosition( glm::vec2( this->placement.x, this->placement.y + 0 * this->error_line_height) );
        text_2d_buffer_r->print( "Game files are not found!" );
        text_2d_buffer_r->setColor( glm::vec4( 1, 1, 1, 1 ) );
        text_2d_buffer_r->setPosition( glm::vec2( this->placement.x, this->placement.y + 1 * this->error_line_height) );
        text_2d_buffer_r->print( "This project requires" );
        text_2d_buffer_r->setPosition( glm::vec2( this->placement.x, this->placement.y + 2 * this->error_line_height) );
        text_2d_buffer_r->print( "these game files to run." );
        text_2d_buffer_r->setPosition( glm::vec2( this->placement.x, this->placement.y + 3 * this->error_line_height) );
        text_2d_buffer_r->print( "This is the path that" );
        text_2d_buffer_r->setPosition( glm::vec2( this->placement.x, this->placement.y + 4 * this->error_line_height) );
        text_2d_buffer_r->print( "this program uses." );

        auto entry = main_program.manager.getIFFEntry( failed_map_name );
        text_2d_buffer_r->setColor( glm::vec4( 0.5, 1, 0.5, 1 ) );
        text_2d_buffer_r->setPosition( glm::vec2( this->placement.x, this->placement.y + 5 * this->error_line_height) );
        text_2d_buffer_r->print( entry.getPath( main_program.platform ) );

        entry = main_program.manager.getIFFEntry( Data::Manager::global );

        if( entry.getIFF( main_program.platform ) == nullptr ) {
            text_2d_buffer_r->setColor( glm::vec4( 1, 1, 1, 1 ) );
            text_2d_buffer_r->setPosition( glm::vec2( this->placement.x, this->placement.y + 6 * this->error_line_height) );
            text_2d_buffer_r->print( "The globals file is" );
            text_2d_buffer_r->setPosition( glm::vec2( this->placement.x, this->placement.y + 7 * this->error_line_height) );
            text_2d_buffer_r->print( "also missing." );
            text_2d_buffer_r->setColor( glm::vec4( 0.5, 1, 0.5, 1 ) );
            text_2d_buffer_r->setPosition( glm::vec2( this->placement.x, this->placement.y + 8 * this->error_line_height) );
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
