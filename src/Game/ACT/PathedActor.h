#ifndef FC_GAME_ACT_BASE_PATHED_ACTOR_HEADER
#define FC_GAME_ACT_BASE_PATHED_ACTOR_HEADER

#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/PathedActor.h"

#include "BasePathedEntity.h"

namespace Game::ACT {

class PathedActor : public BasePathedEntity {
protected:
    uint32_t alive_id; bool alive_base;
    uint32_t  dead_id; bool  dead_base;

    const Data::Mission::ObjResource *alive_cobj_r;
    const Data::Mission::ObjResource  *dead_cobj_r;

    Graphics::ModelInstance *alive_p;

public:
    PathedActor( const Data::Accessor& accessor, const Data::Mission::ACT::PathedActor& obj );
    PathedActor( const PathedActor& obj );
    virtual ~PathedActor();

    virtual Actor* duplicate( const Actor &original ) const;

    virtual void resetGraphics( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

}

#endif // FC_GAME_ACT_BASE_PATHED_ACTOR_HEADER
