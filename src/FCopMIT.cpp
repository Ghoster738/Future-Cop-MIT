#include "MainProgram.h"
#include "PrimaryGame.h"

int main(int argc, char** argv)
{
    MainProgram main_program( argc, argv );
    PrimaryGame primary_game;

    primary_game.load( main_program );

    main_program.primary_game_r = &primary_game;

    main_program.displayLoop();

    primary_game.unload( main_program );

    return 0;
}
