#ifndef FC_GAME_ACT_ELEVATOR
#define FC_GAME_ACT_ELEVATOR

#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/Elevator.h"

#include "BaseEntity.h"

namespace Game::ACT {

class Elevator : public BaseEntity {
private:
    glm::quat rotation;

    uint32_t model_id; bool has_model;
    Graphics::ModelInstance *model_p;

public:
    Elevator( Utilities::Random &random, const Data::Accessor& accessor, const Data::Mission::ACT::Elevator& obj );
    Elevator( const Elevator& obj );
    virtual ~Elevator();

    virtual Actor* duplicate( const Actor &original ) const;

    virtual void resetGraphics( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

}

#endif // FC_GAME_ACT_ELEVATOR
