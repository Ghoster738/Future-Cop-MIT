#ifndef FC_PRIMARY_GAME_H
#define FC_PRIMARY_GAME_H

#include "GameState.h"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

class PrimaryGame : public GameState {
private:
    glm::vec3 camera_position_transform;
    glm::vec2 camera_rotation_transform;
    float     camera_distance_transform;

    int current_tile_selected;
    unsigned til_polygon_type_selected;

public:
    PrimaryGame();
    virtual ~PrimaryGame();

    virtual void load();
    virtual void unload();

    virtual void grabControls( MainProgram &main_program );
    virtual void applyTime( MainProgram &main_program, std::chrono::microseconds delta );
    virtual void display( MainProgram &main_program );
};

#endif // FC_PRIMARY_GAME_H
