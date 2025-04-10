#include "ActManager.h"

#include "../Data/Mission/FUNResource.h"

namespace {

template<class data_act, class game_act>
Game::ActManager::SpawnableActor<game_act> initializeActors( Utilities::Random& random, const Data::Accessor& accessor, std::vector<const data_act*> actor_array_r ) {
    Game::ActManager::SpawnableActor<game_act> game_actors;

    for( const data_act *const actor_r : actor_array_r ) {
        if( !actor_r->getSpawnChunk().exists )
            game_actors.actors.push_back( game_act( random, accessor, *actor_r ) );
        else {
            game_actors.spawners.push_back( {actor_r->getSpawnChunk(), game_act( random, accessor, *actor_r )} );
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

template<class game_act>
void updateActors( MainProgram &main_program, Game::ActManager::SpawnableActor<game_act> &game_actors, std::chrono::microseconds delta ) {
    for( auto &actor : game_actors.actors ) {
        actor.update(main_program, delta);
    }
    for( auto &spawner : game_actors.spawners ) {
        for( auto &actor : spawner.current_actors ) {
            actor.update(main_program, delta);
        }
    }
}

}

namespace Game {

ActManager::ActManager( const Data::Accessor& accessor, Utilities::Random rand ) : random( rand ) {
    auto actor_array_r = accessor.getActorAccessor().getAllConst();

    aircraft        = initializeActors<Data::Mission::ACT::Aircraft,      ACT::Aircraft>(      rand, accessor, accessor.getActorAccessor().getAllConstAircraft() );
    elevator        = initializeActors<Data::Mission::ACT::Elevator,      ACT::Elevator>(      rand, accessor, accessor.getActorAccessor().getAllConstElevator() );
    dynamic_props   = initializeActors<Data::Mission::ACT::DynamicProp,   ACT::DynamicProp>(   rand, accessor, accessor.getActorAccessor().getAllConstDynamicProp() );
    item_pickups    = initializeActors<Data::Mission::ACT::ItemPickup,    ACT::ItemPickup>(    rand, accessor, accessor.getActorAccessor().getAllConstItemPickup() );
    moveable_props  = initializeActors<Data::Mission::ACT::MoveableProp,  ACT::MoveableProp>(  rand, accessor, accessor.getActorAccessor().getAllConstMoveableProp() );
    neutral_turrets = initializeActors<Data::Mission::ACT::NeutralTurret, ACT::NeutralTurret>( rand, accessor, accessor.getActorAccessor().getAllConstNeutralTurret() );
    pathed_actor    = initializeActors<Data::Mission::ACT::PathedActor,   ACT::PathedActor>(   rand, accessor, accessor.getActorAccessor().getAllConstPathedActor() );
    props           = initializeActors<Data::Mission::ACT::Prop,          ACT::Prop>(          rand, accessor, accessor.getActorAccessor().getAllConstProp() );
    sky_captains    = initializeActors<Data::Mission::ACT::SkyCaptain,    ACT::SkyCaptain>(    rand, accessor, accessor.getActorAccessor().getAllConstSkyCaptain() );
    turrets         = initializeActors<Data::Mission::ACT::Turret,        ACT::Turret>(        rand, accessor, accessor.getActorAccessor().getAllConstTurret() );
    walkable_props  = initializeActors<Data::Mission::ACT::WalkableProp,  ACT::WalkableProp>(  rand, accessor, accessor.getActorAccessor().getAllConstWalkableProp() );
    x1_alphas       = initializeActors<Data::Mission::ACT::X1Alpha,       ACT::X1Alpha>(       rand, accessor, accessor.getActorAccessor().getAllConstX1Alpha() );
}

ActManager::~ActManager() {
}

void ActManager::initialize( MainProgram &main_program ) {
    updateGraphics<ACT::Aircraft>(      main_program,        aircraft );
    updateGraphics<ACT::Elevator>(      main_program,        elevator );
    updateGraphics<ACT::DynamicProp>(   main_program,   dynamic_props );
    updateGraphics<ACT::ItemPickup>(    main_program,    item_pickups );
    updateGraphics<ACT::MoveableProp>(  main_program,  moveable_props );
    updateGraphics<ACT::NeutralTurret>( main_program, neutral_turrets );
    updateGraphics<ACT::PathedActor>(   main_program,    pathed_actor );
    updateGraphics<ACT::Prop>(          main_program,           props );
    updateGraphics<ACT::SkyCaptain>(    main_program,    sky_captains );
    updateGraphics<ACT::Turret>(        main_program,         turrets );
    updateGraphics<ACT::WalkableProp>(  main_program,  walkable_props );
    updateGraphics<ACT::X1Alpha>(       main_program,       x1_alphas );

    auto Cfun = main_program.accessor.getAllConstFUN();

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
            for( auto &spawner : turrets.spawners ) {
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
    updateSpawn<ACT::Aircraft>(      main_program,        aircraft, delta );
    updateSpawn<ACT::ItemPickup>(    main_program,    item_pickups, delta );
    updateSpawn<ACT::NeutralTurret>( main_program, neutral_turrets, delta );
    updateSpawn<ACT::SkyCaptain>(    main_program,    sky_captains, delta );
    updateSpawn<ACT::Turret>(        main_program,         turrets, delta );
    updateSpawn<ACT::X1Alpha>(       main_program,       x1_alphas, delta );

    updateActors<ACT::Aircraft>(      main_program,        aircraft, delta );
    updateActors<ACT::Elevator>(      main_program,        elevator, delta );
    updateActors<ACT::DynamicProp>(   main_program,   dynamic_props, delta );
    updateActors<ACT::ItemPickup>(    main_program,    item_pickups, delta );
    updateActors<ACT::MoveableProp>(  main_program,  moveable_props, delta );
    updateActors<ACT::NeutralTurret>( main_program, neutral_turrets, delta );
    updateActors<ACT::PathedActor>(   main_program,    pathed_actor, delta );
    updateActors<ACT::Prop>(          main_program,           props, delta );
    updateActors<ACT::SkyCaptain>(    main_program,    sky_captains, delta );
    updateActors<ACT::Turret>(        main_program,         turrets, delta );
    updateActors<ACT::WalkableProp>(  main_program,  walkable_props, delta );
    updateActors<ACT::X1Alpha>(       main_program,       x1_alphas, delta );
}

}
