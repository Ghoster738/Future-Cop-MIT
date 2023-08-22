#include "MainProgram.h"

#include "PrimaryGame.h"
#include "ModelViewer.h"

#include "MainMenu.h"

int main(int argc, char** argv)
{
    MainProgram main_program( argc, argv );

    MainMenu::main_menu.is_game_on = false;
    MainMenu::main_menu.load( main_program );
    PrimaryGame::primary_game.load( main_program );

    main_program.menu_r = &MainMenu::main_menu;
    main_program.primary_game_r = &PrimaryGame::primary_game;

    main_program.displayLoop();

    if( main_program.primary_game_r != nullptr )
        main_program.primary_game_r->unload( main_program );

    if( main_program.menu_r != nullptr )
        main_program.menu_r->unload( main_program );

    return 0;
}
