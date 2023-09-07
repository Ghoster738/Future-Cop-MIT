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

void updateResolutionStatus( MainProgram &main_program, Menu::Item& resolution ) {
    resolution.name = std::to_string( main_program.options.getVideoWidth() );
    resolution.name += "x";
    resolution.name += std::to_string( main_program.options.getVideoHeight() );
}

class ItemClickExit : public Menu::ItemClick {
public:
    virtual void onPress( MainProgram &main_program, Menu* menu_r, Menu::Item* ) {
        if( main_program.menu_r != nullptr )
            main_program.menu_r->unload( main_program );

        main_program.menu_r = &MainMenu::main_menu;
        main_program.menu_r->load( main_program );
    }
} item_click_exit;

class ItemClickSaveAndExit : public Menu::ItemClick {
public:
    virtual void onPress( MainProgram &main_program, Menu* menu_r, Menu::Item* item_r ) {
        main_program.options.saveOptions();
        item_click_exit.onPress( main_program, menu_r, item_r );
    }
} item_click_save_and_exit;

class ItemClickReconfigureControls : public Menu::ItemClick {
public:
    virtual void onPress( MainProgram &main_program, Menu* menu_r, Menu::Item* ) {
        main_program.control_system_p->clearAllInputSets();

        if( main_program.menu_r != nullptr )
            main_program.menu_r->unload( main_program );

        InputMenu::input_menu.load( main_program );
        main_program.menu_r = &InputMenu::input_menu;
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

const unsigned id_resolution           =  0;
const unsigned id_window_status        =  1;
const unsigned id_reconfigure_controls =  2;
const unsigned id_current_platform     =  3;
const unsigned id_save_exit            =  4;
const unsigned id_exit                 =  5;

const unsigned id_windows              =  6;
const unsigned id_mac                  =  7;
const unsigned id_playstation          =  8;

const unsigned id_dec_res              =  9;
const unsigned id_add_res              = 10;
const unsigned id_display_res          = 11;
}
OptionsMenu::~OptionsMenu() {

}

void OptionsMenu::load( MainProgram &main_program ) {
    Menu::load( main_program );

    glm::u32vec2 scale = main_program.getWindowScale();
    uint32_t center = scale.x / 2;

    Graphics::Text2DBuffer::Font prime_font;
    Graphics::Text2DBuffer::Font selected_font;
    Graphics::Text2DBuffer::Font title_font;

    const unsigned smaller_step = 0.05 * static_cast<float>( scale.y );
    const unsigned step = 0.055 * static_cast<float>( scale.y );

    if( !main_program.text_2d_buffer_r->selectFont( prime_font, 0.9 * smaller_step, smaller_step ) ) {
        prime_font = 1;

        main_program.text_2d_buffer_r->scaleFont( prime_font, smaller_step );
    }

    if( !main_program.text_2d_buffer_r->selectFont( selected_font, smaller_step, step ) ) {
        selected_font = 1;

        main_program.text_2d_buffer_r->scaleFont( selected_font, step - 1 );
    }

    if( !main_program.text_2d_buffer_r->selectFont( title_font, step, 2 * step ) ) {
        selected_font = 1;

        main_program.text_2d_buffer_r->scaleFont( title_font, 2 * step - 1 );
    }

    const Graphics::Text2DBuffer::CenterMode left_mode  = Graphics::Text2DBuffer::CenterMode::LEFT;
    const Graphics::Text2DBuffer::CenterMode right_mode = Graphics::Text2DBuffer::CenterMode::RIGHT;

    this->items.clear();

    this->items.emplace_back( new Menu::TextButton( "Resolution: ",                   glm::vec2( 0,       2 * smaller_step ),          id_resolution,           id_resolution,           id_resolution,       id_resolution,       &Menu::null_item_click,        prime_font, selected_font, left_mode ) );
    this->items.emplace_back( new Menu::TextButton( windowStatusName( main_program ), glm::vec2( scale.x, 3 * smaller_step ),             id_dec_res,        id_window_status, id_reconfigure_controls,              id_window_status,    &item_click_window_status,    prime_font, selected_font, right_mode ) );
    this->items.emplace_back( new Menu::TextButton( "Reconfigure Controls",           glm::vec2( 0,       4 * smaller_step ),       id_window_status, id_reconfigure_controls,                  id_mac,       id_reconfigure_controls,    &item_click_reconfigure_controls,    prime_font, selected_font, left_mode ) );
    this->items.emplace_back( new Menu::TextButton( "Platform: ",                     glm::vec2( 0,       5 * smaller_step ),    id_current_platform,     id_current_platform,     id_current_platform, id_current_platform, &Menu::null_item_click,       prime_font, selected_font, left_mode ) );
    this->items.emplace_back( new Menu::TextButton( "Save to Config File and Exit",  glm::vec2( center,  scale.y - 3 * smaller_step ),        id_mac,            id_save_exit,                 id_exit,             id_save_exit,        &item_click_save_and_exit, prime_font, selected_font ) );
    this->items.emplace_back( new Menu::TextButton( "Exit without Saving",           glm::vec2( center,  scale.y - 2 * smaller_step ),   id_save_exit,                id_exit,              id_dec_res,          id_exit,             &item_click_exit,        prime_font, selected_font ) );

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

    if( main_program.text_2d_buffer_r->getLineLength( prime_font, this->items[id_current_platform]->name + "*" + WINDOWS[this->shorten_platform] + "**" + MACINTOSH[this->shorten_platform] + "**" + PLAYSTATION[this->shorten_platform] + "*" ) > scale.x )
        this->shorten_platform = true;

    // For the Platform selection use
    auto line_length = main_program.text_2d_buffer_r->getLineLength( prime_font, this->items[id_current_platform]->name );

    this->items.emplace_back( new Menu::TextButton( WINDOWS[this->shorten_platform],     glm::vec2( line_length, this->items[id_current_platform]->position.y ), id_window_status, id_mac,         id_save_exit, id_windows,  &item_click_switch_to_windows,    prime_font, selected_font, left_mode ) );

    line_length += main_program.text_2d_buffer_r->getLineLength( prime_font, WINDOWS[this->shorten_platform] );
    auto end_length = scale.x - main_program.text_2d_buffer_r->getLineLength( prime_font, PLAYSTATION[this->shorten_platform] );

    this->items.emplace_back( new Menu::TextButton( MACINTOSH[this->shorten_platform],   glm::vec2( (end_length + line_length) / 2, this->items[id_current_platform]->position.y ), id_window_status, id_playstation, id_save_exit, id_windows, &item_click_switch_to_macintosh,   prime_font, selected_font ) );

    this->items.emplace_back( new Menu::TextButton( PLAYSTATION[this->shorten_platform], glm::vec2( scale.x, this->items[id_current_platform]->position.y ), id_window_status, id_playstation, id_save_exit, id_mac,     &item_click_switch_to_playstation, prime_font, selected_font, right_mode ) );

    line_length = main_program.text_2d_buffer_r->getLineLength( prime_font, this->items[id_resolution]->name );

    this->items.emplace_back( new Menu::TextButton( "Dec",       glm::vec2( line_length, this->items[id_resolution]->position.y ), id_exit,        id_add_res,     id_window_status, id_dec_res,     &item_click_decrement_resolution, prime_font, selected_font, left_mode ) );
    line_length += main_program.text_2d_buffer_r->getLineLength( selected_font, this->items[id_dec_res]->name );

    this->items.emplace_back( new Menu::TextButton( "Add",       glm::vec2( scale.x, this->items[id_resolution]->position.y ), id_exit,        id_add_res,     id_window_status, id_dec_res,     &item_click_increment_resolution, prime_font, selected_font, right_mode ) );

    end_length = scale.x - main_program.text_2d_buffer_r->getLineLength( prime_font, "Add" );

    this->items.emplace_back( new Menu::TextButton( "????x????", glm::vec2( (end_length + line_length) / 2, this->items[id_resolution]->position.y), id_display_res, id_display_res, id_display_res,   id_display_res, &Menu::null_item_click,           prime_font, selected_font ) );

    updatePlatfromStatus( main_program, this->shorten_platform, *this->items[id_windows], *this->items[id_mac], *this->items[id_playstation] );

    this->items.emplace_back( new Menu::TextButton( "Options", glm::vec2( center, 0 ), id_resolution, id_resolution, id_resolution, id_resolution, &Menu::null_item_click, title_font, title_font ) );
    this->items.emplace_back( new Menu::TextButton( "Current Window Status:", glm::vec2( 0, this->items[id_window_status]->position.y ), id_resolution, id_resolution, id_resolution, id_resolution, &Menu::null_item_click, prime_font, prime_font, left_mode ) );

    this->current_item_index = id_dec_res;

    updateResolutionStatus( main_program, *this->items[id_display_res] );
}

void OptionsMenu::unload( MainProgram &main_program ) {

}

void OptionsMenu::display( MainProgram &main_program ) {
    updatePlatfromStatus( main_program, this->shorten_platform, *this->items[id_windows], *this->items[id_mac], *this->items[id_playstation] );

    updateResolutionStatus( main_program, *this->items[id_display_res] );

    for( size_t i = 0; i < this->items.size(); i++ ) {
        if( this->current_item_index != i )
            this->items[i]->drawNeutral( main_program );
        else
            this->items[i]->drawSelected( main_program );
    }
}
