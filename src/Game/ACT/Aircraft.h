#ifndef FC_GAME_ACT_AIRCRAFT
#define FC_GAME_ACT_AIRCRAFT

#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/Aircraft.h"

#include "Actor.h"

namespace Game::ACT {

class Aircraft : public Actor {
private:
    glm::vec2 texture_offset;

    uint32_t model_id; bool model;
    Graphics::ModelInstance *model_p;

public:
    Aircraft( const Data::Accessor& accessor, const Data::Mission::ACT::Aircraft& obj );
    Aircraft( const Aircraft& obj );
    virtual ~Aircraft();

    virtual Actor* duplicate( const Actor &original ) const;

    virtual void resetGraphics( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

}

#endif // FC_GAME_ACT_AIRCRAFT
