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
    class BaseTurret;
    class ItemPickup;
    class NeutralTurret;
    class Prop;
    class SkyCaptain;
    class Trigger;
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

    Mission::ACT::BaseTurret* getBaseTurret( uint32_t resource_id );
    const Mission::ACT::BaseTurret* getConstBaseTurret( uint32_t resource_id ) const;
    std::vector<Mission::ACT::BaseTurret*> getAllBaseTurret();
    std::vector<const Mission::ACT::BaseTurret*> getAllConstBaseTurret() const;

    Mission::ACT::ItemPickup* getItemPickup( uint32_t resource_id );
    const Mission::ACT::ItemPickup* getConstItemPickup( uint32_t resource_id ) const;
    std::vector<Mission::ACT::ItemPickup*> getAllItemPickup();
    std::vector<const Mission::ACT::ItemPickup*> getAllConstItemPickup() const;

    Mission::ACT::NeutralTurret* getNeutralTurret( uint32_t resource_id );
    const Mission::ACT::NeutralTurret* getConstNeutralTurret( uint32_t resource_id ) const;
    std::vector<Mission::ACT::NeutralTurret*> getAllNeutralTurret();
    std::vector<const Mission::ACT::NeutralTurret*> getAllConstNeutralTurret() const;

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

    Mission::ACT::X1Alpha* getX1Alpha( uint32_t resource_id );
    const Mission::ACT::X1Alpha* getConstX1Alpha( uint32_t resource_id ) const;
    std::vector<Mission::ACT::X1Alpha*> getAllX1Alpha();
    std::vector<const Mission::ACT::X1Alpha*> getAllConstX1Alpha() const;
};

} // Data

#endif // DATA_ACCESSOR_H
