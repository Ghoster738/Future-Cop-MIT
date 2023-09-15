#ifndef FC_GAME_ACT_ACTOR_HEADER
#define FC_GAME_ACT_ACTOR_HEADER

#include "../../MainProgram.h"
#include <chrono>

namespace Game {

namespace ACT {

class Actor {
protected:
    glm::vec3 position;

public:
    Actor() : position( 0, 0, 0 ) {}
    Actor( const Actor& obj ) : position( obj.position ) {}
    virtual ~Actor() {}

    virtual Actor* duplicate( const Actor &original ) const = 0;

    virtual void resetGraphics( MainProgram &main_program ) {}

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta ) = 0;
};

}

}

#endif // FC_GAME_ACT_BASE_TURRET_HEADER
