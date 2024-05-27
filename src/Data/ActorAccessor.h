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

    void emplaceActorConstant( const Mission::ACTResource *resource_r );
    void emplaceActor( Mission::ACTResource *resource_r );
    void clear();

    Mission::ACT::Aircraft* getAircraft( uint32_t resource_id );
    const Mission::ACT::Aircraft* getConstAircraft( uint32_t resource_id ) const;
    std::vector<Mission::ACT::Aircraft*> getAllAircraft();
    std::vector<const Mission::ACT::Aircraft*> getAllConstAircraft() const;
};

} // Data

#endif // DATA_ACCESSOR_H
