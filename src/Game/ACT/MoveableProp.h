#ifndef FC_GAME_ACT_MOVEABLE_PROP
#define FC_GAME_ACT_MOVEABLE_PROP

#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/MoveableProp.h"

#include "BaseEntity.h"

namespace Game::ACT {

class MoveableProp : public BaseEntity {
private:
    glm::quat rotation;

    uint32_t alive_id; bool alive;
    Graphics::ModelInstance *alive_p;

public:
    MoveableProp( const Data::Accessor& accessor, const Data::Mission::ACT::MoveableProp& obj );
    MoveableProp( const MoveableProp& obj );
    virtual ~MoveableProp();

    virtual Actor* duplicate( const Actor &original ) const;

    virtual void resetGraphics( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

}

#endif // FC_GAME_ACT_MOVEABLE_PROP
