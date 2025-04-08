#ifndef FC_GAME_ACT_WALKABLE_PROP
#define FC_GAME_ACT_WALKABLE_PROP

#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/WalkableProp.h"

#include "BaseEntity.h"

namespace Game::ACT {

class WalkableProp : public BaseEntity {
private:
    glm::quat rotation;

    uint32_t alive_id; bool alive;
    Graphics::ModelInstance *alive_p;

public:
    WalkableProp( const Data::Accessor& accessor, const Data::Mission::ACT::WalkableProp& obj );
    WalkableProp( const WalkableProp& obj );
    virtual ~WalkableProp();

    virtual Actor* duplicate( const Actor &original ) const;

    virtual void resetGraphics( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

}

#endif // FC_GAME_ACT_WALKABLE_PROP
