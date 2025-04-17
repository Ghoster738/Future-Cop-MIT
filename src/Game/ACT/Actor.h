#ifndef FC_GAME_ACT_ACTOR_HEADER
#define FC_GAME_ACT_ACTOR_HEADER

#include "../../MainProgram.h"
#include <chrono>

namespace Game::ACT {

class Actor {
protected:
    uint32_t actor_id;
    glm::vec3 position;

public:
    Actor( uint32_t p_actor_id ) : actor_id( p_actor_id ), position( 0, 0, 0 ) {}
    Actor( const Actor& obj ) : actor_id( obj.actor_id ), position( obj.position ) {}
    virtual ~Actor() {}

    virtual Actor* duplicate( const Actor &original ) const = 0;

    virtual void resetGraphics( MainProgram &main_program ) {}

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta ) = 0;

    uint32_t getID() const { return actor_id; }
};

}

#endif // FC_GAME_ACT_BASE_TURRET_HEADER
