#include "OptionsMenu.h"

#include <glm/vec4.hpp>

#include "MainProgram.h"
#include "MainMenu.h"
#include "InputMenu.h"

#include <iostream>

OptionsMenu OptionsMenu::options_menu;

namespace {
const int32_t resolutions[][2] = { {320, 240}, {640, 480}, {800, 600}, {1024, 768}, {1280, 720}, {1280, 800}, {1280, 1024}, {1440, 900}, {1536, 864}, {1600, 900}, {1600, 1200}, {1680, 1050}, {1920, 1080}, {1920, 1200}, {2048, 1152}, {2048, 1536}, {2560, 1080}, {2560, 1440}, {2560, 1600}, {3440, 1440}, {3840, 2160}, {7680, 4320} };
const size_t RESOLUTION_AMOUNT = sizeof(resolutions) / (2 * sizeof(uint32_t));

const std::string WINDOWS[2]     = { "Windows",     "WIN" };
const std::string MACINTOSH[2]   = { "Macintosh",   "MAC" };
const std::string PLAYSTATION[2] = { "Playstation", "PS1" };

std::string windowStatusName( MainProgram &main_program ) {
    if( main_program.options.getVideoFullscreen() )
        return "Fullscreen";
    else
        return "Windowed";
}

void updatePlatfromStatus( MainProgram &main_program, bool shorten, Menu::Item& windows, Menu::Item& mac, Menu::Item& playstation ) {
    Data::Manager::Platform platform = Data::Manager::getPlatformFromString( main_program.options.getCurrentPlatform() );

    windows.name     = WINDOWS[shorten];
    mac.name         = MACINTOSH[shorten];
    playstation.name = PLAYSTATION[shorten];

    switch( platform ) {
        case Data::Manager::Platform::WINDOWS:
            windows.name = "*" + windows.name + "*";
            break;
        case Data::Manager::Platform::MACINTOSH:
            mac.name = "*" + mac.name + "*";
            break;
        case Data::Manager::Platform::PLAYSTATION:
            playstation.name = "*" + playstation.name + "*";
            break;
        default:
            break;
    }
}

std::string updateResolutionStatus( MainProgram &main_program ) {
    std::string video_name = std::to_string( main_program.options.getVideoWidth() );
    video_name += "x";
    video_name += std::to_string( main_program.options.getVideoHeight() );

    return video_name;
}

class ItemClickSaveAndExit : public Menu::ItemClick {
public:
    virtual void onPress( MainProgram &main_program, Menu* menu_r, Menu::Item* item_r ) {
        main_program.options.saveOptions();
        MainMenu::item_click_main_menu.onPress( main_program, menu_r, item_r );
    }
} item_click_save_and_exit;

class ItemClickReconfigureControls : public Menu::ItemClickSwitchMenu {
public:
    ItemClickReconfigureControls() : ItemClickSwitchMenu( &InputMenu::input_menu ) {}

    virtual void onPress( MainProgram &main_program, Menu* menu_r, Menu::Item* item_r) {
        main_program.control_system_p->clearAllInputSets();

        InputMenu::input_menu.next_menu_r  = &MainMenu::main_menu;
        InputMenu::input_menu.next_state_r = nullptr;

        ItemClickSwitchMenu::onPress( main_program, menu_r, item_r );
    }
} item_click_reconfigure_controls;

class ItemClickWindowStatus : public Menu::ItemClick {
public:
    virtual void onPress( MainProgram &main_program, Menu* menu_r, Menu::Item* item_r ) {
        main_program.options.setVideoFullscreen( !main_program.options.getVideoFullscreen() );

        item_r->name = windowStatusName( main_program );
    }
} item_click_window_status;

class ItemClickSwitchToPlatform : public Menu::ItemClick {
private:
    std::string name;
public:
    ItemClickSwitchToPlatform( std::string p_name ) : name( p_name ) {}

    virtual void onPress( MainProgram &main_program, Menu* menu_r, Menu::Item* ) {
        main_program.options.setCurrentPlatform( name );
    }
}
item_click_switch_to_windows(       "windows" ),
item_click_switch_to_macintosh(    "macintosh" ),
item_click_switch_to_playstation( "playstation" );

class ItemClickIncResolution : public Menu::ItemClick {
private:
    int inc;
public:
    ItemClickIncResolution( int p_inc ) : inc( p_inc ) {}

    virtual void onPress( MainProgram &main_program, Menu* menu_r, Menu::Item* ) {
        auto converted_menu_r = dynamic_cast<OptionsMenu*>(menu_r);

        if( converted_menu_r->selected_resolution != 0 || inc > 0)
            converted_menu_r->selected_resolution = (converted_menu_r->selected_resolution + inc) % RESOLUTION_AMOUNT;
        else
            converted_menu_r->selected_resolution = RESOLUTION_AMOUNT - 1;

        main_program.options.setVideoWidth(  resolutions[ converted_menu_r->selected_resolution ][ 0 ] );
        main_program.options.setVideoHeight( resolutions[ converted_menu_r->selected_resolution ][ 1 ] );
    }
}
item_click_increment_resolution(  1 ),
item_click_decrement_resolution( -1 );

const unsigned id_window_status        = 0;
const unsigned id_reconfigure_controls = 1;
const unsigned id_save_exit            = 2;
const unsigned id_exit                 = 3;

const unsigned id_windows              = 4;
const unsigned id_mac                  = 5;
const unsigned id_playstation          = 6;

const unsigned id_dec_res              = 7;
const unsigned id_add_res              = 8;
}

const std::string OptionsMenu::RESOLUTION_NAME = "Resolution:";
const std::string OptionsMenu::PLATFORM_NAME   = "Platform:";

OptionsMenu::~OptionsMenu() {

}

void OptionsMenu::load( MainProgram &main_program ) {
    Menu::load( main_program );

    glm::u32vec2 scale = main_program.getWindowScale();
    uint32_t center = scale.x / 2;

    Graphics::Text2DBuffer::Font selected_font;

    const unsigned smaller_step = 0.05 * static_cast<float>( scale.y );
    const unsigned step = 0.055 * static_cast<float>( scale.y );

    if( !main_program.text_2d_buffer_r->selectFont( this->prime_font, 0.9 * smaller_step, smaller_step ) ) {
        this->prime_font = 1;

        main_program.text_2d_buffer_r->scaleFont( this->prime_font, smaller_step );
    }

    if( !main_program.text_2d_buffer_r->selectFont( selected_font, smaller_step, step ) ) {
        selected_font = 1;

        main_program.text_2d_buffer_r->scaleFont( selected_font, step - 1 );
    }

    if( !main_program.text_2d_buffer_r->selectFont( this->title_font, step, 2 * step ) ) {
        selected_font = 1;

        main_program.text_2d_buffer_r->scaleFont( this->title_font, 2 * step - 1 );
    }

    const auto left_mode  = Graphics::Text2DBuffer::CenterMode::LEFT;
    const auto right_mode = Graphics::Text2DBuffer::CenterMode::RIGHT;

    this->items.clear();

    this-> resolution_position = glm::vec2( 0, 2 * smaller_step );

    this->items.emplace_back( new Menu::TextButton( windowStatusName( main_program ), glm::vec2( scale.x, 3 * smaller_step ),             id_dec_res,        id_window_status, id_reconfigure_controls,              id_window_status,    &item_click_window_status,       this->prime_font, selected_font, right_mode ) );
    this->items.emplace_back( new Menu::TextButton( "Reconfigure Controls",           glm::vec2( 0,       4 * smaller_step ),       id_window_status, id_reconfigure_controls,                  id_mac,       id_reconfigure_controls,    &item_click_reconfigure_controls,    this->prime_font, selected_font, left_mode ) );

    this->platform_position = glm::vec2( 0, 5 * smaller_step );

    this->items.emplace_back( new Menu::TextButton( "Save to Config File and Exit",  glm::vec2( center,  scale.y - 3 * smaller_step ),        id_mac,            id_save_exit,                 id_exit,             id_save_exit,        &item_click_save_and_exit,       this->prime_font, selected_font ) );
    this->items.emplace_back( new Menu::TextButton( "Exit without Saving",           glm::vec2( center,  scale.y - 2 * smaller_step ),   id_save_exit,                id_exit,              id_dec_res,          id_exit,             &MainMenu::item_click_main_menu, this->prime_font, selected_font ) );

    this->selected_resolution = 0;

    for( size_t i = 0; i < RESOLUTION_AMOUNT; i++ ) {
        if( resolutions[ i ][ 0 ] == main_program.options.getVideoWidth() &&
            resolutions[ i ][ 1 ] == main_program.options.getVideoHeight()
        ) {
            this->selected_resolution = i;
            i = RESOLUTION_AMOUNT;
        }
    }

    // This code is to shorten the platform names. This is so the Playstation version of the game fits in the screen.
    this->shorten_platform = false;

    if( main_program.text_2d_buffer_r->getLineLength( this->prime_font, PLATFORM_NAME + "*" + WINDOWS[this->shorten_platform] + "**" + MACINTOSH[this->shorten_platform] + "**" + PLAYSTATION[this->shorten_platform] + "*" ) > scale.x )
        this->shorten_platform = true;

    // For the Platform selection use
    auto line_length = main_program.text_2d_buffer_r->getLineLength( this->prime_font, PLATFORM_NAME );

    this->items.emplace_back( new Menu::TextButton( WINDOWS[this->shorten_platform],     glm::vec2( line_length, this->platform_position.y ), id_window_status, id_mac,         id_save_exit, id_windows,  &item_click_switch_to_windows,    this->prime_font, selected_font, left_mode ) );

    line_length += main_program.text_2d_buffer_r->getLineLength( this->prime_font, WINDOWS[this->shorten_platform] );
    auto end_length = scale.x - main_program.text_2d_buffer_r->getLineLength( this->prime_font, PLAYSTATION[this->shorten_platform] );

    this->items.emplace_back( new Menu::TextButton( MACINTOSH[this->shorten_platform],   glm::vec2( (end_length + line_length) / 2, this->platform_position.y ), id_window_status, id_playstation, id_save_exit, id_windows, &item_click_switch_to_macintosh,   this->prime_font, selected_font ) );

    this->items.emplace_back( new Menu::TextButton( PLAYSTATION[this->shorten_platform], glm::vec2( scale.x, this->platform_position.y ), id_window_status, id_playstation, id_save_exit, id_mac,     &item_click_switch_to_playstation, this->prime_font, selected_font, right_mode ) );

    line_length = main_program.text_2d_buffer_r->getLineLength( this->prime_font, RESOLUTION_NAME );

    this->items.emplace_back( new Menu::TextButton( "Dec",       glm::vec2( line_length, resolution_position.y ), id_exit,        id_add_res,     id_window_status, id_dec_res,     &item_click_decrement_resolution, this->prime_font, selected_font, left_mode ) );
    line_length += main_program.text_2d_buffer_r->getLineLength( selected_font, this->items[id_dec_res]->name );

    this->items.emplace_back( new Menu::TextButton( "Add",       glm::vec2( scale.x, resolution_position.y ), id_exit,        id_add_res,     id_window_status, id_dec_res,     &item_click_increment_resolution, this->prime_font, selected_font, right_mode ) );

    end_length = scale.x - main_program.text_2d_buffer_r->getLineLength( this->prime_font, "Add" );

    resolution_display_position = glm::vec2( (end_length + line_length) / 2, resolution_position.y);

    updatePlatfromStatus( main_program, this->shorten_platform, *this->items[id_windows], *this->items[id_mac], *this->items[id_playstation] );

    title_position = glm::vec2( center, 0 );

    window_status_position = glm::vec2( 0, this->items[id_window_status]->position.y );

    this->current_item_index = id_dec_res;

    updateResolutionStatus( main_program );
}

void OptionsMenu::unload( MainProgram &main_program ) {

}

void OptionsMenu::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    Menu::update( main_program, delta );

    const auto text_2d_buffer_r = main_program.text_2d_buffer_r;

    text_2d_buffer_r->setColor( glm::vec4( 1 ) );
    text_2d_buffer_r->setFont( this->title_font );
    text_2d_buffer_r->setCenterMode( Graphics::Text2DBuffer::CenterMode::MIDDLE );
    text_2d_buffer_r->setPosition( this->title_position );
    text_2d_buffer_r->print( "Options" );

    text_2d_buffer_r->setFont( this->prime_font );
    text_2d_buffer_r->setPosition( this->resolution_display_position );
    text_2d_buffer_r->print( updateResolutionStatus( main_program ) );

    text_2d_buffer_r->setCenterMode( Graphics::Text2DBuffer::CenterMode::LEFT );

    text_2d_buffer_r->setPosition( this->window_status_position );
    text_2d_buffer_r->print( "Current Window Status:" );

    text_2d_buffer_r->setPosition( this->platform_position );
    text_2d_buffer_r->print( PLATFORM_NAME );

    text_2d_buffer_r->setPosition( this->resolution_position );
    text_2d_buffer_r->print( RESOLUTION_NAME );

    updatePlatfromStatus( main_program, this->shorten_platform, *this->items[id_windows], *this->items[id_mac], *this->items[id_playstation] );

    drawAllItems( main_program );
}
