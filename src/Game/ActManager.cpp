#include "ActManager.h"

#include "../Data/Mission/FUNResource.h"

namespace {

template<class data_act, class game_act>
Game::ActManager::SpawnableActor<game_act> initializeActors( const Data::Accessor& accessor, const std::vector<data_act*>& actor_array_r ) {
    Game::ActManager::SpawnableActor<game_act> game_actors;

    for( const data_act *const actor_r : actor_array_r ) {
        if( !actor_r->getSpawnChunk().exists )
            game_actors.actors.push_back( game_act( accessor, *actor_r ) );
        else {
            game_actors.spawners.push_back( {actor_r->getSpawnChunk(), game_act( accessor, *actor_r )} );
            game_actors.spawners.back().time = std::chrono::microseconds( (unsigned)(game_actors.spawners.back().timings.getSpawnTime() * 1000000.0) );
        }
    }

    return game_actors;
}

template<class game_act>
void updateGraphics( MainProgram &main_program, Game::ActManager::SpawnableActor<game_act> &game_actors ) {
    for( game_act& actor : game_actors.actors )
        actor.resetGraphics( main_program );
    for( auto spawner : game_actors.spawners ) {
        for( game_act& actor : spawner.current_actors )
            actor.resetGraphics( main_program );
    }
}

template<class game_act>
void updateSpawn( MainProgram &main_program, Game::ActManager::SpawnableActor<game_act> &game_actors, std::chrono::microseconds delta ) {
    for( auto &spawner : game_actors.spawners ) {
        if( spawner.timings.isAutomatic() && spawner.current_actors.size() < spawner.timings.spawn_limit ) {
            spawner.time -= delta;

            if( spawner.time.count() <= 0 ) {
                spawner.time = std::chrono::microseconds( (unsigned)(spawner.timings.getSpawnTime() * 1000000.0) );
                spawner.current_actors.push_back( spawner.actor );
                spawner.current_actors.back().resetGraphics( main_program );
            }
        }
    }
}

}

namespace Game {

ActManager::ActManager( const Data::Mission::IFF& resource, const Data::Accessor& accessor ) {
    auto actor_array_r = Data::Mission::ACTResource::getVector( resource );

    item_pickups    = initializeActors<Data::Mission::ACT::ItemPickup,    ACT::ItemPickup>(    accessor, Data::Mission::ACT::ItemPickup::getVector(    actor_array_r ) );
    base_turrets    = initializeActors<Data::Mission::ACT::BaseTurret,    ACT::BaseTurret>(    accessor, Data::Mission::ACT::BaseTurret::getVector(    actor_array_r ) );
    neutral_turrets = initializeActors<Data::Mission::ACT::NeutralTurret, ACT::NeutralTurret>( accessor, Data::Mission::ACT::NeutralTurret::getVector( actor_array_r ) );
    props           = initializeActors<Data::Mission::ACT::Prop,          ACT::Prop>(          accessor, Data::Mission::ACT::Prop::getVector(          actor_array_r ) );
    sky_captains    = initializeActors<Data::Mission::ACT::SkyCaptain,    ACT::SkyCaptain>(    accessor, Data::Mission::ACT::SkyCaptain::getVector(    actor_array_r ) );
}

ActManager::~ActManager() {
}

void ActManager::initialize( MainProgram &main_program ) {
    updateGraphics<ACT::ItemPickup>(    main_program,    item_pickups );
    updateGraphics<ACT::BaseTurret>(    main_program,    base_turrets );
    updateGraphics<ACT::NeutralTurret>( main_program, neutral_turrets );
    updateGraphics<ACT::Prop>(          main_program,           props );
    updateGraphics<ACT::SkyCaptain>(    main_program,    sky_captains );

    auto Cfun = main_program.accessor.getAllFUN();

    if(Cfun.size() != 0) {
        if(Cfun[0]->getSpawnAllNeutralTurrets()) {
            for( auto &spawner : neutral_turrets.spawners ) {
                if( spawner.timings.isAutomatic() && spawner.current_actors.size() < spawner.timings.spawn_limit ) {
                    spawner.time = std::chrono::microseconds(0);
                }
            }
        }

        auto spawn_ids = Cfun[0]->getSpawnActorsNow();
        for( auto &identifer : spawn_ids ) {
            for( auto &spawner : base_turrets.spawners ) {
                if(spawner.actor.getID() == identifer)
                    spawner.time = std::chrono::microseconds(0);
            }
            for( auto &spawner : item_pickups.spawners ) {
                if(spawner.actor.getID() == identifer)
                    spawner.time = std::chrono::microseconds(0);
            }
        }
    }
}

void ActManager::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    updateSpawn<ACT::ItemPickup>(    main_program,    item_pickups, delta );
    updateSpawn<ACT::BaseTurret>(    main_program,    base_turrets, delta );
    updateSpawn<ACT::NeutralTurret>( main_program, neutral_turrets, delta );
    updateSpawn<ACT::Prop>(          main_program,           props, delta );
    updateSpawn<ACT::SkyCaptain>(    main_program,    sky_captains, delta );
}

}
