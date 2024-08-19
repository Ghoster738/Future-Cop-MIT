#include "MapSelectorMenu.h"

#include <glm/vec4.hpp>

#include "MainProgram.h"
#include "ParticleViewer.h"
#include "MainMenu.h"

namespace {
class ItemClickMapSelect : public Menu::ItemClick {
public:
    virtual void onPress( MainProgram &main_program, Menu* menu_r, Menu::Item* item_r ) {
        auto entry = main_program.manager.getIFFEntry( item_r->name );

        if( !Utilities::Options::Tools::isFile( entry.getPath( main_program.platform ) ) ) {
            auto menu_select_r = dynamic_cast<MapSelectorMenu*>(menu_r);

            menu_select_r->missing_resource = main_program.text_2d_buffer_r->splitText( menu_select_r->error_font, entry.getPath( main_program.platform ), menu_select_r->missing_line_length );

            entry = main_program.manager.getIFFEntry( Data::Manager::global );

            if( !Utilities::Options::Tools::isFile( entry.getPath( main_program.platform ) ) )
                menu_select_r->missing_global = main_program.text_2d_buffer_r->splitText( menu_select_r->error_font, entry.getPath( main_program.platform ), menu_select_r->missing_line_length );
        }
        else {
            main_program.switchMenu( nullptr );
            main_program.switchPrimaryGame( dynamic_cast<MapSelectorMenu*>(menu_r)->game_r );

            main_program.transitionToResource( item_r->name, main_program.platform );
        }
    }
} item_click_map_select;
}

MapSelectorMenu MapSelectorMenu::map_selector_menu;

MapSelectorMenu::~MapSelectorMenu() {
}

void MapSelectorMenu::load( MainProgram &main_program ) {
    Menu::load( main_program );

    glm::u32vec2 scale = main_program.getWindowScale();
    uint32_t center = scale.x / 2;

    this->items.clear();

    const unsigned back  = Data::Manager::AMOUNT_OF_IFF_IDS + 0;
    const unsigned title = Data::Manager::AMOUNT_OF_IFF_IDS + 1;

    const unsigned line_height = static_cast<float>( scale.y ) / static_cast<float>( Data::Manager::AMOUNT_OF_IFF_IDS + 3);
    this->error_line_height = 0.025 * static_cast<float>( scale.y );

    Graphics::Text2DBuffer::Font prime_font;
    Graphics::Text2DBuffer::Font selected_font;

    if( !main_program.text_2d_buffer_r->selectFont( this->title_font, line_height, line_height * 2 - 1 ) ) {
        this->title_font = 1;

        main_program.text_2d_buffer_r->scaleFont( this->title_font, line_height - 1 );
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

    const auto left_mode  = Graphics::Text2DBuffer::CenterMode::LEFT;

    for( size_t i = 0; i < Data::Manager::AMOUNT_OF_IFF_IDS; i++ ) {
        const std::string map_name = *Data::Manager::map_iffs[i];
        const auto line_placement = glm::vec2( 0, (i + 2) * line_height );
        auto low_position = (i - 1) % Data::Manager::AMOUNT_OF_IFF_IDS;
        const auto high_position = (i + 1) % Data::Manager::AMOUNT_OF_IFF_IDS;
        const auto same_position = i;

        if( i == 0 )
            low_position = Data::Manager::AMOUNT_OF_IFF_IDS - 1;

        this->items.emplace_back( new Menu::TextButton( map_name, line_placement, low_position, same_position, high_position, same_position, &item_click_map_select, prime_font, selected_font, left_mode ) );
    }

    this->items.emplace_back( new Menu::TextButton( "Back",     glm::vec2( 0, (Data::Manager::AMOUNT_OF_IFF_IDS + 2) * line_height ), Data::Manager::AMOUNT_OF_IFF_IDS - 1, back, 0, back, &MainMenu::item_click_main_menu, prime_font, selected_font, left_mode ) );

    // Attach the back button to the rest of the interface.
    this->items.at( 0 )->up_index = this->items.size() - 1;
    this->items.at( this->items.size() - 2 )->down_index = this->items.size() - 1;

    this->placement.x = 0;
    this->placement.y = line_height * 2;
    for( size_t i = 0; i < this->items.size(); i++ ) {
        this->placement.x = std::max( this->placement.x, (unsigned)main_program.text_2d_buffer_r->getLineLength( prime_font, this->items[i]->name ) );
    }

    this->missing_line_length = scale.x - this->placement.x;
    this->missing_resource = {};
    this->missing_global   = {};

    this->title_position = glm::vec2( center, 0 );

    this->current_item_index = 0;
}

void MapSelectorMenu::unload( MainProgram &main_program ) {

}

void MapSelectorMenu::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    Menu::update( main_program, delta );

    const auto text_2d_buffer_r = main_program.text_2d_buffer_r;

    text_2d_buffer_r->setColor( glm::vec4( 1 ) );
    text_2d_buffer_r->setFont( this->title_font );
    text_2d_buffer_r->setCenterMode( Graphics::Text2DBuffer::CenterMode::MIDDLE );
    text_2d_buffer_r->setPosition( this->title_position );
    text_2d_buffer_r->print( this->name );

    if( !this->missing_resource.empty() ) {
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

        text_2d_buffer_r->setColor( glm::vec4( 0.5, 1, 0.5, 1 ) );
        for( size_t i = 0; i < this->missing_resource.size(); i++ ) {
            text_2d_buffer_r->setPosition( glm::vec2( this->placement.x, this->placement.y + (5 + i) * this->error_line_height) );
            text_2d_buffer_r->print( this->missing_resource[i] );
        }

        if( !this->missing_global.empty() ) {
            auto placement = this->placement.y + (5 + this->missing_resource.size()) * this->error_line_height;

            text_2d_buffer_r->setColor( glm::vec4( 1, 1, 1, 1 ) );
            text_2d_buffer_r->setPosition( glm::vec2( this->placement.x, placement + 1 * this->error_line_height) );
            text_2d_buffer_r->print( "The globals file is" );
            text_2d_buffer_r->setPosition( glm::vec2( this->placement.x, placement + 2 * this->error_line_height) );
            text_2d_buffer_r->print( "also missing." );
            text_2d_buffer_r->setColor( glm::vec4( 0.5, 1, 0.5, 1 ) );
            for( size_t i = 0; i < this->missing_global.size(); i++ ) {
                text_2d_buffer_r->setPosition( glm::vec2( this->placement.x, placement + (3 + i) * this->error_line_height) );
                text_2d_buffer_r->print( this->missing_global[i] );
            }
        }
    }
    else if( this->game_r == &ParticleViewer::particle_viewer ) {
        text_2d_buffer_r->setFont( this->error_font );
        text_2d_buffer_r->setCenterMode( Graphics::Text2DBuffer::CenterMode::LEFT );
        text_2d_buffer_r->setColor( glm::vec4( 1 ) );
        text_2d_buffer_r->setPosition( glm::vec2( this->placement.x, this->placement.y + 0 * this->error_line_height) );
        text_2d_buffer_r->print( "Particles from every unmodded" );
        text_2d_buffer_r->setPosition( glm::vec2( this->placement.x, this->placement.y + 1 * this->error_line_height) );
        text_2d_buffer_r->print( "map is the same. This is true" );
        text_2d_buffer_r->setPosition( glm::vec2( this->placement.x, this->placement.y + 2 * this->error_line_height) );
        text_2d_buffer_r->print( "for PC games where only one" );
        text_2d_buffer_r->setPosition( glm::vec2( this->placement.x, this->placement.y + 3 * this->error_line_height) );
        text_2d_buffer_r->print( "particle resource is in GlblData." );
        text_2d_buffer_r->setPosition( glm::vec2( this->placement.x, this->placement.y + 4 * this->error_line_height) );
        text_2d_buffer_r->print( "All known PS1 mission files" );
        text_2d_buffer_r->setPosition( glm::vec2( this->placement.x, this->placement.y + 5 * this->error_line_height) );
        text_2d_buffer_r->print( "have the same particles resource." );
    }

    drawAllItems( main_program );
}
