#include "ActManager.h"

#include <iostream>

namespace Game {

ActManager::ActManager( const Data::Mission::IFF& resource ) {
    auto actor_array_r = Data::Mission::ACTResource::getVector( resource );
    Data::Mission::ACTManager actor_manager( actor_array_r );

    auto item_pickup_array_r = Data::Mission::ACT::ItemPickup::getVector( actor_array_r );

    for( const Data::Mission::ACT::ItemPickup *const item_pickup_r : item_pickup_array_r ) {
        if( !item_pickup_r->getSpawnChunk().exists )
            item_pickups.actors.push_back( ACT::ItemPickup( resource, *item_pickup_r ) );
        else {
            item_pickups.spawners.push_back( {item_pickup_r->getSpawnChunk(), ACT::ItemPickup( resource, *item_pickup_r )} );
            item_pickups.spawners.back().time = std::chrono::microseconds( (unsigned)(item_pickups.spawners.back().timings.getSpawnTime() * 1000000.0) );
        }
    }

    auto base_turret_array_r = Data::Mission::ACT::BaseTurret::getVector( actor_array_r );

    for( const Data::Mission::ACT::BaseTurret *const base_turret_r : base_turret_array_r ) {
        if( !base_turret_r->getSpawnChunk().exists )
            base_turrets.actors.push_back( ACT::BaseTurret( resource, *base_turret_r ) );
        else {
            base_turrets.spawners.push_back( {base_turret_r->getSpawnChunk(), ACT::BaseTurret( resource, *base_turret_r )} );
            base_turrets.spawners.back().time = std::chrono::microseconds( (unsigned)(base_turrets.spawners.back().timings.getSpawnTime() * 1000000.0) );
        }
    }

    auto neutral_turret_array_r = Data::Mission::ACT::NeutralTurret::getVector( actor_array_r );

    for( const Data::Mission::ACT::NeutralTurret *const neutral_turret_r : neutral_turret_array_r ) {
        if( !neutral_turret_r->getSpawnChunk().exists )
            neutral_turrets.actors.push_back( ACT::NeutralTurret( resource, *neutral_turret_r ) );
        else {
            neutral_turrets.spawners.push_back( {neutral_turret_r->getSpawnChunk(), ACT::NeutralTurret( resource, *neutral_turret_r )} );
            neutral_turrets.spawners.back().time = std::chrono::microseconds( (unsigned)(neutral_turrets.spawners.back().timings.getSpawnTime() * 1000000.0) );
        }
    }
}

ActManager::~ActManager() {
}

void ActManager::initialize( MainProgram &main_program ) {
    for( ACT::ItemPickup& item_pickup : item_pickups.actors )
        item_pickup.resetGraphics( main_program );
    for( SpawnableActor<ACT::ItemPickup>::Spawner& spawner : item_pickups.spawners ) {
        for( ACT::ItemPickup& item_pickup : spawner.current_actors )
            item_pickup.resetGraphics( main_program );
    }

    for( ACT::BaseTurret& base_turret : base_turrets.actors )
        base_turret.resetGraphics( main_program );
    for( SpawnableActor<ACT::BaseTurret>::Spawner& spawner : base_turrets.spawners ) {
        for( ACT::BaseTurret& base_turret : spawner.current_actors )
            base_turret.resetGraphics( main_program );
    }

    for( ACT::NeutralTurret& neutral_turret : neutral_turrets.actors )
        neutral_turret.resetGraphics( main_program );
    for( SpawnableActor<ACT::NeutralTurret>::Spawner& spawner : neutral_turrets.spawners ) {
        for( ACT::NeutralTurret& neutral_turret : spawner.current_actors )
            neutral_turret.resetGraphics( main_program );
    }
}

void ActManager::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    for( SpawnableActor<ACT::ItemPickup>::Spawner& spawner : item_pickups.spawners ) {
        if( spawner.timings.isAutomatic() && spawner.current_actors.size() < spawner.timings.spawn_limit ) {
            spawner.time -= delta;

            if( spawner.time.count() <= 0 ) {
                spawner.time = std::chrono::microseconds( (unsigned)(spawner.timings.getSpawnTime() * 1000000.0) );
                spawner.current_actors.push_back( spawner.actor );
                spawner.current_actors.back().resetGraphics( main_program );
            }
        }
    }

    for( SpawnableActor<ACT::BaseTurret>::Spawner& spawner : base_turrets.spawners ) {
        if( spawner.timings.isAutomatic() && spawner.current_actors.size() < spawner.timings.spawn_limit ) {
            spawner.time -= delta;

            if( spawner.time.count() <= 0 ) {
                spawner.time = std::chrono::microseconds( (unsigned)(spawner.timings.getSpawnTime() * 1000000.0) );
                spawner.current_actors.push_back( spawner.actor );
                spawner.current_actors.back().resetGraphics( main_program );
            }
        }
    }

    for( SpawnableActor<ACT::NeutralTurret>::Spawner& spawner : neutral_turrets.spawners ) {
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
