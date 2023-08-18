#ifndef FC_GAME_PROGRAM_H
#define FC_GAME_PROGRAM_H

#include <chrono>

class MainProgram;

class GameState {
public:
    GameState();
    virtual ~GameState();

    virtual void grabControls( MainProgram &main_program ) = 0;
    virtual void applyTime( MainProgram &main_program, std::chrono::microseconds delta ) = 0;
    virtual void display( MainProgram &main_program ) = 0;
};

#endif // FC_GAME_PROGRAM_H
