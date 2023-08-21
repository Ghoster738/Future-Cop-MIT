#include "MainProgram.h"

#include "PrimaryGame.h"
#include "ModelViewer.h"

#include "MainMenu.h"

int main(int argc, char** argv)
{
    MainProgram main_program( argc, argv );
    PrimaryGame primary_game;
    ModelViewer model_viewer;
    MainMenu main_menu;

    main_menu.primary_game_r = &primary_game;
    main_menu.model_viewer_r = &model_viewer;

    main_menu.load( main_program );
    primary_game.load( main_program );

    main_program.menu_r = &main_menu;
    main_program.primary_game_r = &primary_game;

    main_program.displayLoop();

    primary_game.unload( main_program );
    main_menu.unload( main_program );

    return 0;
}
