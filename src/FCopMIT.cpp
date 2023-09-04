#include "MainProgram.h"

#include "PrimaryGame.h"
#include "ModelViewer.h"

#include "MainMenu.h"
#include "InputMenu.h"

int main(int argc, char** argv)
{
    MainProgram main_program( argc, argv );

    MainMenu::main_menu.is_game_on = false;

    PrimaryGame::primary_game.load( main_program );

    InputMenu::input_menu.name = main_program.paths.getConfigDirPath() + "controls";
    InputMenu::input_menu.menu_r = &MainMenu::main_menu;

    if( main_program.control_system_p->read( InputMenu::input_menu.name ) > 0 ) {
        MainMenu::main_menu.load( main_program );
        main_program.menu_r = &MainMenu::main_menu;
    } else {
        InputMenu::input_menu.load( main_program );
        main_program.menu_r = &InputMenu::input_menu;
    }
    main_program.primary_game_r = &PrimaryGame::primary_game;

    main_program.displayLoop();

    if( main_program.primary_game_r != nullptr )
        main_program.primary_game_r->unload( main_program );

    if( main_program.menu_r != nullptr )
        main_program.menu_r->unload( main_program );

    return 0;
}
