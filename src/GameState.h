#ifndef FC_GAME_PROGRAM_H
#define FC_GAME_PROGRAM_H

#include <chrono>

class MainProgram;

class GameState {
public:
    virtual ~GameState() {}

    virtual void load( MainProgram &main_program ) = 0;
    virtual void unload( MainProgram &main_program ) = 0;

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta ) = 0;
};

#endif // FC_GAME_PROGRAM_H
