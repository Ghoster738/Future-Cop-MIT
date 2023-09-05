#include "OptionsMenu.h"

#include <glm/vec4.hpp>

#include "MainProgram.h"
#include "MainMenu.h"
#include "InputMenu.h"

#include <iostream>

OptionsMenu OptionsMenu::options_menu;

namespace {
std::string windowStatusName( MainProgram &main_program ) {
    if( main_program.options.getVideoFullscreen() )
        return "Fullscreen";
    else
        return "Windowed";
}

void nullPress( MainProgram &main_program, Menu*, Menu::Item* ) {
    // Nothing.
}

void menuExit( MainProgram &main_program, Menu* menu_r, Menu::Item* ) {
    if( main_program.menu_r != nullptr )
        main_program.menu_r->unload( main_program );

    main_program.menu_r = &MainMenu::main_menu;
    main_program.menu_r->load( main_program );
}

void menuSaveAndExit( MainProgram &main_program, Menu* menu_r, Menu::Item* item_r ) {
    main_program.options.saveOptions();
    menuExit( main_program, menu_r, item_r );
}

void reconfigureControls( MainProgram &main_program, Menu* menu_r, Menu::Item* ) {
    main_program.control_system_p->clearAllInputSets();

    if( main_program.menu_r != nullptr )
        main_program.menu_r->unload( main_program );

    InputMenu::input_menu.load( main_program );
    main_program.menu_r = &InputMenu::input_menu;
}

void windowStatus( MainProgram &main_program, Menu* menu_r, Menu::Item* item_r ) {
    main_program.options.setVideoFullscreen( !main_program.options.getVideoFullscreen() );

    item_r->name = windowStatusName( main_program );
}

const std::string WINDOWS[2]     = { "Windows",     "WIN" };
const std::string MACINTOSH[2]   = { "Macintosh",   "MAC" };
const std::string PLAYSTATION[2] = { "Playstation", "PS1" };

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

void switchToWindows( MainProgram &main_program, Menu* menu_r, Menu::Item* item_r ) {
    main_program.options.setCurrentPlatform( "windows" );
}
void switchToMacintosh( MainProgram &main_program, Menu* menu_r, Menu::Item* item_r ) {
    main_program.options.setCurrentPlatform( "macintosh" );
}
void switchToPlaystation( MainProgram &main_program, Menu* menu_r, Menu::Item* item_r ) {
    main_program.options.setCurrentPlatform( "playstation" );
}

const int32_t resolutions[][2] = { {320, 240}, {640, 480}, {800, 600}, {1024, 768}, {1280, 720}, {1280, 800}, {1280, 1024}, {1440, 900}, {1536, 864}, {1600, 900}, {1600, 1200}, {1680, 1050}, {1920, 1080}, {1920, 1200}, {2048, 1152}, {2048, 1536}, {2560, 1080}, {2560, 1440}, {2560, 1600}, {3440, 1440}, {3840, 2160}, {7680, 4320} };
const size_t RESOLUTION_AMOUNT = sizeof(resolutions) / (2 * sizeof(uint32_t));

void updateResolutionStatus( MainProgram &main_program, Menu::Item& resolution ) {
    resolution.name = std::to_string( main_program.options.getVideoWidth() );
    resolution.name += "x";
    resolution.name += std::to_string( main_program.options.getVideoHeight() );
}
void incrementResolution( MainProgram &main_program, Menu* menu_r, Menu::Item* item_r ) {
    auto converted_menu_r = dynamic_cast<OptionsMenu*>(menu_r);

    converted_menu_r->selected_resolution = (converted_menu_r->selected_resolution + 1) % RESOLUTION_AMOUNT;

    main_program.options.setVideoWidth(  resolutions[ converted_menu_r->selected_resolution ][ 0 ] );
    main_program.options.setVideoHeight( resolutions[ converted_menu_r->selected_resolution ][ 1 ] );
}
void decrementResolution( MainProgram &main_program, Menu* menu_r, Menu::Item* item_r ) {
    auto converted_menu_r = dynamic_cast<OptionsMenu*>(menu_r);

    if( converted_menu_r->selected_resolution != 0 )
        converted_menu_r->selected_resolution = (converted_menu_r->selected_resolution - 1) % RESOLUTION_AMOUNT;
    else
        converted_menu_r->selected_resolution = RESOLUTION_AMOUNT - 1;

    main_program.options.setVideoWidth(  resolutions[ converted_menu_r->selected_resolution ][ 0 ] );
    main_program.options.setVideoHeight( resolutions[ converted_menu_r->selected_resolution ][ 1 ] );
}

}
OptionsMenu::~OptionsMenu() {

}

void OptionsMenu::load( MainProgram &main_program ) {
    Menu::load( main_program );

    const unsigned resolution       =  0;
    const unsigned window_status    =  1;
    const unsigned reconfigure_controls =  2;
    const unsigned current_platform =  3;
    const unsigned save_exit        =  4;
    const unsigned exit             =  5;

    const unsigned windows          =  6;
    const unsigned mac              =  7;
    const unsigned playstation      =  8;

    const unsigned dec_res          =  9;
    const unsigned add_res          = 10;
    const unsigned display_res      = 11;

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

    this->items.emplace_back( new Menu::TextButton( "Resolution: ",                   glm::vec2( 0,       2 * smaller_step ),          resolution,           resolution,           resolution,       resolution,       nullPress,        prime_font, selected_font, left_mode ) );
    this->items.emplace_back( new Menu::TextButton( windowStatusName( main_program ), glm::vec2( scale.x, 3 * smaller_step ),             dec_res,        window_status, reconfigure_controls,              window_status,    windowStatus,    prime_font, selected_font, right_mode ) );
    this->items.emplace_back( new Menu::TextButton( "Reconfigure Controls",           glm::vec2( 0,       4 * smaller_step ),       window_status, reconfigure_controls,                  mac,       reconfigure_controls,    reconfigureControls,    prime_font, selected_font, left_mode ) );
    this->items.emplace_back( new Menu::TextButton( "Platform: ",                     glm::vec2( 0,       5 * smaller_step ),    current_platform,     current_platform,     current_platform, current_platform, nullPress,       prime_font, selected_font, left_mode ) );
    this->items.emplace_back( new Menu::TextButton( "Save and Exit",                  glm::vec2( center,  scale.y - 3 * smaller_step ),       mac,            save_exit,                 exit,             save_exit,        menuSaveAndExit, prime_font, selected_font ) );
    this->items.emplace_back( new Menu::TextButton( "Exit without Saving",            glm::vec2( center,  scale.y - 2 * smaller_step ), save_exit,                 exit,              dec_res,          exit,             menuExit,        prime_font, selected_font ) );

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

    if( main_program.text_2d_buffer_r->getLineLength( prime_font, this->items[current_platform]->name + "*" + WINDOWS[this->shorten_platform] + "**" + MACINTOSH[this->shorten_platform] + "**" + PLAYSTATION[this->shorten_platform] + "*" ) > scale.x )
        this->shorten_platform = true;

    // For the Platform selection use
    auto line_length = main_program.text_2d_buffer_r->getLineLength( prime_font, this->items[current_platform]->name );

    this->items.emplace_back( new Menu::TextButton( WINDOWS[this->shorten_platform],     glm::vec2( line_length, this->items[current_platform]->position.y ), window_status, mac,         save_exit, windows,  switchToWindows,    prime_font, selected_font, left_mode ) );

    line_length += main_program.text_2d_buffer_r->getLineLength( prime_font, WINDOWS[this->shorten_platform] );
    auto end_length = scale.x - main_program.text_2d_buffer_r->getLineLength( prime_font, PLAYSTATION[this->shorten_platform] );

    this->items.emplace_back( new Menu::TextButton( MACINTOSH[this->shorten_platform],   glm::vec2( (end_length + line_length) / 2, this->items[current_platform]->position.y ), window_status, playstation, save_exit, windows, switchToMacintosh,   prime_font, selected_font ) );

    this->items.emplace_back( new Menu::TextButton( PLAYSTATION[this->shorten_platform], glm::vec2( scale.x, this->items[current_platform]->position.y ), window_status, playstation, save_exit, mac,     switchToPlaystation, prime_font, selected_font, right_mode ) );

    line_length = main_program.text_2d_buffer_r->getLineLength( prime_font, this->items[resolution]->name );

    this->items.emplace_back( new Menu::TextButton( "Dec",       glm::vec2( line_length, this->items[resolution]->position.y ), exit,        add_res,     window_status, dec_res,     decrementResolution, prime_font, selected_font, left_mode ) );
    line_length += main_program.text_2d_buffer_r->getLineLength( selected_font, this->items[dec_res]->name );

    this->items.emplace_back( new Menu::TextButton( "Add",       glm::vec2( scale.x, this->items[resolution]->position.y ), exit,        add_res,     window_status, dec_res,     incrementResolution, prime_font, selected_font, right_mode ) );

    end_length = scale.x - main_program.text_2d_buffer_r->getLineLength( prime_font, "Add" );

    this->items.emplace_back( new Menu::TextButton( "????x????", glm::vec2( (end_length + line_length) / 2, this->items[resolution]->position.y), display_res, display_res, display_res,   display_res, nullPress,           prime_font, selected_font ) );

    updatePlatfromStatus( main_program, this->shorten_platform, *this->items[windows], *this->items[mac], *this->items[playstation] );

    this->items.emplace_back( new Menu::TextButton( "Options", glm::vec2( center, 0 ), resolution, resolution, resolution, resolution, incrementResolution, title_font, title_font ) );
    this->items.emplace_back( new Menu::TextButton( "Current Window Status:", glm::vec2( 0, this->items[window_status]->position.y ), resolution, resolution, resolution, resolution, incrementResolution, prime_font, prime_font, left_mode ) );

    this->current_item_index = dec_res;

    updateResolutionStatus( main_program, *this->items[display_res] );
}

void OptionsMenu::unload( MainProgram &main_program ) {

}

void OptionsMenu::display( MainProgram &main_program ) {
    const unsigned windows     = 6;
    const unsigned mac         = 7;
    const unsigned playstation = 8;
    updatePlatfromStatus( main_program, this->shorten_platform, *this->items[windows], *this->items[mac], *this->items[playstation] );

    const unsigned display_res = 11;
    updateResolutionStatus( main_program, *this->items[display_res] );

    for( size_t i = 0; i < this->items.size(); i++ ) {
        if( this->current_item_index != i )
            this->items[i]->drawNeutral( main_program );
        else
            this->items[i]->drawSelected( main_program );
    }
}
