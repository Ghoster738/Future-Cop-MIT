#ifndef FC_GAME_ACT_STATIONARY_ACTOR_HEADER
#define FC_GAME_ACT_STATIONARY_ACTOR_HEADER

#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/StationaryActor.h"

#include "BaseTurret.h"

namespace Game::ACT {

class StationaryActor : public BaseTurret {
protected:

public:
    StationaryActor( Utilities::Random &random, const Data::Accessor& accessor, const Data::Mission::ACT::StationaryActor& obj );
    StationaryActor( const StationaryActor& obj );
    virtual ~StationaryActor();

    virtual Actor* duplicate( const Actor &original ) const;

    virtual void resetGraphics( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

}

#endif // FC_GAME_ACT_STATIONARY_ACTOR_HEADER
