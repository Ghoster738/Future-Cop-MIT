#ifndef FC_GAME_ACT_DYNAMIC_PROP
#define FC_GAME_ACT_DYNAMIC_PROP

#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/DynamicProp.h"

#include "BaseEntity.h"

namespace Game::ACT {

class DynamicProp : public BaseEntity {
private:
    glm::quat rotation;

    uint32_t alive_id; bool alive;
    Graphics::ModelInstance *alive_p;

public:
    DynamicProp( Utilities::Random &random, const Data::Accessor& accessor, const Data::Mission::ACT::DynamicProp& obj );
    DynamicProp( const DynamicProp& obj );
    virtual ~DynamicProp();

    virtual Actor* duplicate( const Actor &original ) const;

    virtual void resetGraphics( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

}

#endif // FC_GAME_ACT_DYNAMIC_PROP
