#include "OptionsMenu.h"

#include <glm/vec4.hpp>

#include "MainProgram.h"
#include "MainMenu.h"

OptionsMenu OptionsMenu::options_menu;

namespace {
std::string windowStatusName( MainProgram &main_program ) {
    if( main_program.options.getVideoFullscreen() )
        return "Current Window Status: Fullscreen";
    else
        return "Current Window Status: Windowed";
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

void windowStatus( MainProgram &main_program, Menu* menu_r, Menu::Item* item_r ) {
    main_program.options.setVideoFullscreen( !main_program.options.getVideoFullscreen() );

    item_r->name = windowStatusName( main_program );
}

void updatePlatfromStatus( MainProgram &main_program, Menu::Item& windows, Menu::Item& mac, Menu::Item& playstation ) {
    Data::Manager::Platform platform = Data::Manager::getPlatformFromString( main_program.options.getCurrentPlatform() );

    windows.name     = "Windows";
    mac.name         = "Macintosh";
    playstation.name = "Playstation";

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

}
OptionsMenu::~OptionsMenu() {

}

void OptionsMenu::load( MainProgram &main_program ) {
    Menu::load( main_program );

    this->items.resize( 8 );

    Menu::Item &resolution       = this->items[0];
    Menu::Item &window_status    = this->items[1];
    Menu::Item &current_platform = this->items[2];
    Menu::Item &save_exit        = this->items[3];
    Menu::Item &exit             = this->items[4];
    Menu::Item &windows          = this->items[5];
    Menu::Item &mac              = this->items[6];
    Menu::Item &playstation      = this->items[7];

    resolution       = Menu::Item( "Resolution: ",                   glm::vec2( 0,   0 ), &exit,             nullptr,  &window_status,    nullptr, nullPress );
    window_status    = Menu::Item( windowStatusName( main_program ), glm::vec2( 0,  24 ), &resolution,       nullptr,  &current_platform, nullptr, windowStatus );
    current_platform = Menu::Item( "Current Platform: ",             glm::vec2( 0,  48 ), &window_status,    &windows, &save_exit,        nullptr, nullPress );
    save_exit        = Menu::Item( "Save and Exit",                  glm::vec2( 0,  96 ), &current_platform, nullptr,  &exit,             nullptr, menuSaveAndExit );
    exit             = Menu::Item( "Exit without Saving",            glm::vec2( 0, 120 ), &save_exit,        nullptr,  &resolution,       nullptr, menuExit );


    windows     = Menu::Item( "Windows",     glm::vec2( 190, 48 ), nullptr, &mac,         nullptr, &current_platform,  switchToWindows );
    mac         = Menu::Item( "Macintosh",   glm::vec2( 320, 48 ), nullptr, &playstation, nullptr, &windows,           switchToMacintosh );
    playstation = Menu::Item( "Playstation", glm::vec2( 460, 48 ), nullptr, nullptr,      nullptr, &mac,               switchToPlaystation );

    updatePlatfromStatus( main_program, windows, mac, playstation );

    this->current_item_r = &items[0];
}

void OptionsMenu::unload( MainProgram &main_program ) {

}

void OptionsMenu::display( MainProgram &main_program ) {
    Menu::Item &windows          = this->items[5];
    Menu::Item &mac              = this->items[6];
    Menu::Item &playstation      = this->items[7];
    updatePlatfromStatus( main_program, windows, mac, playstation );

    for( size_t i = 0; i < this->items.size(); i++ ) {
        drawButton( main_program, this->items[i] );
    }
}
