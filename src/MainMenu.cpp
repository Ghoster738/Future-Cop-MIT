#include "MainMenu.h"

#include <glm/vec4.hpp>

#include "MainProgram.h"

#include "MapSelectorMenu.h"
#include "OptionsMenu.h"

#include "AnnouncementPlayer.h"
#include "MediaPlayer.h"
#include "ModelViewer.h"
#include "ParticleViewer.h"
#include "PrimaryGame.h"
#include "SoundPlayer.h"

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
        if( main_program.resource_identifier != MainProgram::CUSTOM_IDENTIFIER ) {
            MapSelectorMenu::map_selector_menu.name = name;
            MapSelectorMenu::map_selector_menu.game_r = game_r;

            main_program.switchMenu( &MapSelectorMenu::map_selector_menu );
        }
        else {
            main_program.switchMenu( nullptr );
            main_program.switchPrimaryGame( game_r );

            main_program.transitionToResource( MainProgram::CUSTOM_IDENTIFIER, main_program.platform );
        }
    }
}
item_click_map_spectator(   "Map Spectator", &PrimaryGame::primary_game ),
item_click_view_game_models( "Model Viewer", &ModelViewer::model_viewer ),
item_click_view_particles( "Particle Viewer", &ParticleViewer::particle_viewer ),
item_click_player_announcement( "Announcement Player", &AnnouncementPlayer::announcement_player ),
item_click_player_sound( "Sound Player", &SoundPlayer::sound_player );


class ItemClickExitGame : public Menu::ItemClick {
public:
    virtual void onPress( MainProgram &main_program, Menu* menu_r, Menu::Item* ) {
        auto entry = main_program.manager.getIFFEntry( Data::Manager::global );
        MediaPlayer::media_player.appendMediaPaths( entry.getOutroMediaPaths( main_program.platform ) );

        MediaPlayer::media_player.next_menu_r = nullptr;
        MediaPlayer::media_player.next_state_r = nullptr;

        main_program.switchMenu( nullptr );
        main_program.switchPrimaryGame( &MediaPlayer::media_player );
    }
} item_click_exit_game;

Menu::ItemClickSwitchMenu item_click_menu_done( nullptr );
Menu::ItemClickSwitchMenu item_click_options( &OptionsMenu::options_menu );


}

MainMenu::MainMenu() {
    this->test_radar_p = nullptr;
    this->background_p = nullptr;
}

MainMenu::~MainMenu() {

}

void MainMenu::load( MainProgram &main_program ) {
    Menu::load( main_program );

    glm::u32vec2 scale = main_program.getWindowScale();
    uint32_t center = scale.x / 2;

    const Graphics::Text2DBuffer::CenterMode right_mode = Graphics::Text2DBuffer::CenterMode::RIGHT;

    Graphics::Text2DBuffer::Font selected_font;

    const unsigned step = 2. / 30. * static_cast<float>( scale.y );

    if( !main_program.text_2d_buffer_r->selectFont( this->title_font, 1.5 * step, 2. * step ) )
        this->title_font = {1, 4.0};

    if( !main_program.text_2d_buffer_r->selectFont( this->prime_font, 0.8 * step, 0.9 * step - 1 ) )
        this->prime_font = {1, 2.0};

    if( !main_program.text_2d_buffer_r->selectFont( selected_font, 0.9 * step, step ) )
        selected_font = {1, 2.25};

    if( !main_program.text_2d_buffer_r->selectFont( this->spec_detail_font, 1, 14 ) )
        this->spec_detail_font = 1;

    this->items.clear();

    if( !this->is_game_on ) {
        this->items.emplace_back( new Menu::TextButton( "Map Spectator",       glm::vec2( center, scale.y - 8 * step ), 6, 0, 1, 0, &item_click_map_spectator,       this->prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "View Game Models",    glm::vec2( center, scale.y - 7 * step ), 0, 1, 2, 1, &item_click_view_game_models,    this->prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "View Particles",      glm::vec2( center, scale.y - 6 * step ), 1, 2, 3, 2, &item_click_view_particles,      this->prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "Announcement Player", glm::vec2( center, scale.y - 5 * step ), 2, 3, 4, 3, &item_click_player_announcement, this->prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "Sound Player",        glm::vec2( center, scale.y - 4 * step ), 3, 4, 5, 4, &item_click_player_sound,        this->prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "Options",             glm::vec2( center, scale.y - 3 * step ), 4, 5, 6, 5, &item_click_options,             this->prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "Exit to OS",          glm::vec2( center, scale.y - 2 * step ), 5, 6, 0, 6, &item_click_exit_game,           this->prime_font, selected_font ) );

        this->top_title_position    = glm::vec2( center, 3 * step );
        this->bottom_title_position = glm::vec2( center, 2 * step );

        if( main_program.text_2d_buffer_r->getLineLength( this->title_font, "Future Cop: MIT" ) > scale.x )
            this->bottom_title_position = glm::vec2( center, 5 * step );

        // If any resource is missing I am sure the user would want to know about it.
        this->warning_position = glm::vec2( center, 6 * step );

        this->info_position = glm::vec2( scale.x, scale.y - 14);

        this->current_item_index = 0;
    }
    else {
        this->items.emplace_back( new Menu::TextButton( "Back to Session",     glm::vec2( center, scale.y - 10 * step ), 7, 0, 1, 0, &item_click_menu_done,           this->prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "Map Spectator",       glm::vec2( center, scale.y -  8 * step ), 0, 1, 2, 1, &item_click_map_spectator,       this->prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "View Game Models",    glm::vec2( center, scale.y -  7 * step ), 1, 2, 3, 2, &item_click_view_game_models,    this->prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "View Particles",      glm::vec2( center, scale.y -  6 * step ), 2, 3, 4, 3, &item_click_view_particles,      this->prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "Announcement Player", glm::vec2( center, scale.y -  5 * step ), 3, 4, 5, 4, &item_click_player_announcement, this->prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "Sound Player",        glm::vec2( center, scale.y -  4 * step ), 4, 5, 6, 5, &item_click_player_sound,        this->prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "Options",             glm::vec2( center, scale.y -  3 * step ), 5, 6, 7, 6, &item_click_options,             this->prime_font, selected_font ) );
        this->items.emplace_back( new Menu::TextButton( "Exit to OS",          glm::vec2( center, scale.y -  2 * step ), 6, 7, 0, 7, &item_click_exit_game,           this->prime_font, selected_font ) );
        this->current_item_index = 0;
    }

    if(this->test_radar_p != nullptr)
        delete this->test_radar_p;
    this->test_radar_p = main_program.environment_p->allocateImage();

    if(this->test_radar_p != nullptr) {
        this->test_radar_p->positions[0] = glm::vec2(   0.0,   0.0); // Origin
        this->test_radar_p->positions[1] = glm::vec2( 112.0, 112.0); // End
        this->test_radar_p->is_visable = true;
        this->test_radar_p->cbmp_id = 10;
        this->test_radar_p->texture_coords[0] = static_cast<float>(1. / 256.) * glm::vec2(199, 148);
        this->test_radar_p->texture_coords[1] = static_cast<float>(1. / 256.) * glm::vec2(255, 204);

        this->test_radar_p->update();
    }

    if(this->background_p != nullptr)
        delete this->background_p;

    if(this->test_radar_p != nullptr) {
        this->background_p = main_program.environment_p->allocateImage();
        this->background_p->positions[0] = glm::vec2( (1.0 * scale.x) / 8.0,    0.0); // Origin
        this->background_p->positions[1] = glm::vec2( (7.0 * scale.x) / 8.0, scale.y); // End
        this->background_p->is_visable = true;
        this->background_p->cbmp_id = 0;
        this->background_p->color = glm::vec4(0, 0, 0, 0.5);
        this->background_p->texture_coords[0] = glm::vec2(0, 0);
        this->background_p->texture_coords[1] = glm::vec2(0, 0);

        this->background_p->update();
    }
}

void MainMenu::unload( MainProgram &main_program ) {
    if(this->test_radar_p != nullptr)
        delete this->test_radar_p;
    this->test_radar_p = nullptr;

    if(this->background_p != nullptr)
        delete this->background_p;
    this->background_p = nullptr;
}

void MainMenu::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    Menu::update( main_program, delta );

    const auto text_2d_buffer_r = main_program.text_2d_buffer_r;

    if( !this->is_game_on ) {
        text_2d_buffer_r->setColor( glm::vec4( 1 ) );
        text_2d_buffer_r->setFont( this->title_font );
        text_2d_buffer_r->setCenterMode( Graphics::Text2DBuffer::CenterMode::MIDDLE );
        text_2d_buffer_r->setPosition( this->top_title_position );

        if( this->top_title_position.y > this->bottom_title_position.y )
            text_2d_buffer_r->print( "Future Cop: MIT" );
        else {
            text_2d_buffer_r->print( "Future Cop:" );

            text_2d_buffer_r->setPosition( this->bottom_title_position );
            text_2d_buffer_r->print( "MIT" );
        }

        text_2d_buffer_r->setFont( this->spec_detail_font );
        text_2d_buffer_r->setPosition( this->info_position );
        text_2d_buffer_r->setCenterMode( Graphics::Text2DBuffer::CenterMode::RIGHT );
        text_2d_buffer_r->print( FUTURE_COP_MIT_VERSION );

        if( main_program.global_r == nullptr || main_program.resource_r == nullptr ) {
            text_2d_buffer_r->setFont( this->prime_font );
            text_2d_buffer_r->setPosition( this->warning_position );
            text_2d_buffer_r->setCenterMode( Graphics::Text2DBuffer::CenterMode::MIDDLE );
            text_2d_buffer_r->setColor( glm::vec4( 1, 0.125, 0.125, 1 ) );
            text_2d_buffer_r->print( "Not all resources are found!" );
        }
    }

    drawAllItems( main_program );
}
