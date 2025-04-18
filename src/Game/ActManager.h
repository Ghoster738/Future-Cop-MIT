#ifndef FC_GAME_ACT_MANAGER_H
#define FC_GAME_ACT_MANAGER_H

#include "../Graphics/Environment.h"
#include "../Data/Accessor.h"

#include <chrono>
#include <vector>

#include "ACT/Aircraft.h"
#include "ACT/Elevator.h"
#include "ACT/DCSQuad.h"
#include "ACT/DynamicProp.h"
#include "ACT/ItemPickup.h"
#include "ACT/MoveableProp.h"
#include "ACT/NeutralTurret.h"
#include "ACT/PathedActor.h"
#include "ACT/PathedTurret.h"
#include "ACT/Prop.h"
#include "ACT/StationaryActor.h"
#include "ACT/SkyCaptain.h"
#include "ACT/Turret.h"
#include "ACT/WalkableProp.h"
#include "ACT/X1Alpha.h"

namespace Game {

class ActManager {
public:
    template <class ActorClass>
    struct SpawnableActor {
        struct Spawner {
            Data::Mission::ACTResource::tSAC_chunk timings;
            ActorClass actor;
            std::chrono::microseconds time;
            std::vector<ActorClass> current_actors;
        };
        std::vector<ActorClass> actors;
        std::vector<Spawner> spawners;
    };

private:
    Utilities::Random random;

    SpawnableActor<ACT::Aircraft>        aircraft;
    SpawnableActor<ACT::Elevator>        elevator;
    SpawnableActor<ACT::DCSQuad>         dcs_quad;
    SpawnableActor<ACT::DynamicProp>     dynamic_props;
    SpawnableActor<ACT::ItemPickup>      item_pickups;
    SpawnableActor<ACT::MoveableProp>    moveable_props;
    SpawnableActor<ACT::NeutralTurret>   neutral_turrets;
    SpawnableActor<ACT::PathedActor>     pathed_actor;
    SpawnableActor<ACT::PathedTurret>    pathed_turrets;
    SpawnableActor<ACT::Prop>            props;
    SpawnableActor<ACT::StationaryActor> stationaries;
    SpawnableActor<ACT::SkyCaptain>      sky_captains;
    SpawnableActor<ACT::Turret>          turrets;
    SpawnableActor<ACT::WalkableProp>    walkable_props;
    SpawnableActor<ACT::X1Alpha>         x1_alphas;

public:
    ActManager( const Data::Accessor& accessor, Utilities::Random random );
    virtual ~ActManager();

    void initialize( MainProgram &main_program );

    void update( MainProgram &main_program, std::chrono::microseconds delta );
};

}

#endif // FC_MENU_H
