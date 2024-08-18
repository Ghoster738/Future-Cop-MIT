#ifndef FC_PRIMARY_GAME_H
#define FC_PRIMARY_GAME_H

#include "GameState.h"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include "Data/Mission/TilResource.h"
#include "Graphics/ModelInstance.h"
#include "Graphics/Text2DBuffer.h"

#include "Game/ActManager.h"


class PrimaryGame : public GameState {
public:
    static PrimaryGame primary_game;
private:
    bool      is_camera_moving;
    glm::vec3 camera_position_transform;
    glm::vec2 camera_rotation_transform;
    float     camera_distance_transform;

    std::vector<Graphics::ParticleInstance*> particle_instances_p;

    std::vector<const Data::Mission::TilResource*> til_resources;
    int current_tile_selected;
    unsigned til_polygon_type_selected;

    std::chrono::microseconds counter;
    unsigned map_index;
    unsigned platform_index;

    Graphics::Text2DBuffer::Font font;
    unsigned font_height;

    Game::ActManager *act_manager_p;

public:
    PrimaryGame();
    virtual ~PrimaryGame();

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

#endif // FC_PRIMARY_GAME_H
