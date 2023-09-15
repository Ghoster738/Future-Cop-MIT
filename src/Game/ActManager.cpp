#include "ActManager.h"

namespace Game {

ActManager::ActManager( const Data::Mission::IFF& resource ) {
    auto actor_array_r = Data::Mission::ACTResource::getVector( resource );
    Data::Mission::ACTManager actor_manager( actor_array_r );

    auto base_turret_array_r = Data::Mission::ACT::BaseTurret::getVector( actor_array_r );

    for( const Data::Mission::ACT::BaseTurret *const base_turret_r : base_turret_array_r )
        base_turrets.actors.push_back( ACT::BaseTurret( resource, *base_turret_r ) );
}

ActManager::~ActManager() {
}

void ActManager::initialize( MainProgram &main_program ) {
    for( ACT::BaseTurret& base_turret : base_turrets.actors )
        base_turret.resetGraphics( main_program );
    for( SpawnableActor<ACT::BaseTurret>::Spawner& spawner : base_turrets.spawners ) {
        for( ACT::BaseTurret& base_turret : spawner.current_actors )
            base_turret.resetGraphics( main_program );
    }
}

void ActManager::update( MainProgram &main_program, std::chrono::microseconds delta ) {}

}
