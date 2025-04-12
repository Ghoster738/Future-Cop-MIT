#ifndef DATA_ACTOR_ACCESSOR_H
#define DATA_ACTOR_ACCESSOR_H

#include "Mission/IFF.h"

#include <vector>
#include <map>

namespace Data {

// These are prototypes, so I do not have to include each of them. This is to save compilation time.
namespace Mission {
    class ACTResource;

namespace ACT {
    class Aircraft;
    class Elevator;
    class DCSQuad;
    class DynamicProp;
    class ItemPickup;
    class MoveableProp;
    class NeutralTurret;
    class PathedActor;
    class Prop;
    class SkyCaptain;
    class Trigger;
    class Turret;
    class WalkableProp;
    class X1Alpha; // A.K.A the Player.
}
}

class ActorAccessor {
public:
    struct SearchValue {
        uint32_t actor_type;
        uint32_t resource_id;

        bool operator< ( const SearchValue & operand ) const;
    };
    struct DataValue {
        Mission::ACTResource*       changable_r;
        const Mission::ACTResource* constant_r;
    };

private:
    std::map<SearchValue, DataValue> search;

public:
    ActorAccessor();
    virtual ~ActorAccessor();

    // These three methods should only be called by Accessor.
    void emplaceActorConstant( const Mission::ACTResource *resource_r );
    void emplaceActor( Mission::ACTResource *resource_r );
    void clear();

    std::vector<Mission::ACTResource*> getAll();
    std::vector<const Mission::ACTResource*> getAllConst() const;

    std::vector<Mission::ACTResource*> getAllTypeID( uint_fast8_t type_id );
    std::vector<const Mission::ACTResource*> getAllConstTypeID( uint_fast8_t type_id ) const;

    Mission::ACT::Aircraft* getAircraft( uint32_t resource_id );
    const Mission::ACT::Aircraft* getConstAircraft( uint32_t resource_id ) const;
    std::vector<Mission::ACT::Aircraft*> getAllAircraft();
    std::vector<const Mission::ACT::Aircraft*> getAllConstAircraft() const;

    Mission::ACT::Elevator* getElevator( uint32_t resource_id );
    const Mission::ACT::Elevator* getConstElevator( uint32_t resource_id ) const;
    std::vector<Mission::ACT::Elevator*> getAllElevator();
    std::vector<const Mission::ACT::Elevator*> getAllConstElevator() const;

    Mission::ACT::DCSQuad* getDCSQuad( uint32_t resource_id );
    const Mission::ACT::DCSQuad* getConstDCSQuad( uint32_t resource_id ) const;
    std::vector<Mission::ACT::DCSQuad*> getAllDCSQuad();
    std::vector<const Mission::ACT::DCSQuad*> getAllConstDCSQuad() const;

    Mission::ACT::DynamicProp* getDynamicProp( uint32_t resource_id );
    const Mission::ACT::DynamicProp* getConstDynamicProp( uint32_t resource_id ) const;
    std::vector<Mission::ACT::DynamicProp*> getAllDynamicProp();
    std::vector<const Mission::ACT::DynamicProp*> getAllConstDynamicProp() const;

    Mission::ACT::Turret* getTurret( uint32_t resource_id );
    const Mission::ACT::Turret* getConstTurret( uint32_t resource_id ) const;
    std::vector<Mission::ACT::Turret*> getAllTurret();
    std::vector<const Mission::ACT::Turret*> getAllConstTurret() const;

    Mission::ACT::ItemPickup* getItemPickup( uint32_t resource_id );
    const Mission::ACT::ItemPickup* getConstItemPickup( uint32_t resource_id ) const;
    std::vector<Mission::ACT::ItemPickup*> getAllItemPickup();
    std::vector<const Mission::ACT::ItemPickup*> getAllConstItemPickup() const;

    Mission::ACT::MoveableProp* getMoveableProp( uint32_t resource_id );
    const Mission::ACT::MoveableProp* getConstMoveableProp( uint32_t resource_id ) const;
    std::vector<Mission::ACT::MoveableProp*> getAllMoveableProp();
    std::vector<const Mission::ACT::MoveableProp*> getAllConstMoveableProp() const;

    Mission::ACT::NeutralTurret* getNeutralTurret( uint32_t resource_id );
    const Mission::ACT::NeutralTurret* getConstNeutralTurret( uint32_t resource_id ) const;
    std::vector<Mission::ACT::NeutralTurret*> getAllNeutralTurret();
    std::vector<const Mission::ACT::NeutralTurret*> getAllConstNeutralTurret() const;

    Mission::ACT::PathedActor* getPathedActor( uint32_t resource_id );
    const Mission::ACT::PathedActor* getConstPathedActor( uint32_t resource_id ) const;
    std::vector<Mission::ACT::PathedActor*> getAllPathedActor();
    std::vector<const Mission::ACT::PathedActor*> getAllConstPathedActor() const;

    Mission::ACT::Prop* getProp( uint32_t resource_id );
    const Mission::ACT::Prop* getConstProp( uint32_t resource_id ) const;
    std::vector<Mission::ACT::Prop*> getAllProp();
    std::vector<const Mission::ACT::Prop*> getAllConstProp() const;

    Mission::ACT::SkyCaptain* getSkyCaptain( uint32_t resource_id );
    const Mission::ACT::SkyCaptain* getConstSkyCaptain( uint32_t resource_id ) const;
    std::vector<Mission::ACT::SkyCaptain*> getAllSkyCaptain();
    std::vector<const Mission::ACT::SkyCaptain*> getAllConstSkyCaptain() const;

    Mission::ACT::Trigger* getTrigger( uint32_t resource_id );
    const Mission::ACT::Trigger* getConstTrigger( uint32_t resource_id ) const;
    std::vector<Mission::ACT::Trigger*> getAllTrigger();
    std::vector<const Mission::ACT::Trigger*> getAllConstTrigger() const;

    Mission::ACT::WalkableProp* getWalkableProp( uint32_t resource_id );
    const Mission::ACT::WalkableProp* getConstWalkableProp( uint32_t resource_id ) const;
    std::vector<Mission::ACT::WalkableProp*> getAllWalkableProp();
    std::vector<const Mission::ACT::WalkableProp*> getAllConstWalkableProp() const;

    Mission::ACT::X1Alpha* getX1Alpha( uint32_t resource_id );
    const Mission::ACT::X1Alpha* getConstX1Alpha( uint32_t resource_id ) const;
    std::vector<Mission::ACT::X1Alpha*> getAllX1Alpha();
    std::vector<const Mission::ACT::X1Alpha*> getAllConstX1Alpha() const;
};

} // Data

#endif // DATA_ACCESSOR_H
