#ifndef FC_GAME_ACT_ITEM_PICKUP_HEADER
#define FC_GAME_ACT_ITEM_PICKUP_HEADER

#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/ItemPickup.h"
#include "../../Data/Mission/ObjResource.h"
#include "../../Data/Mission/IFF.h"

#include "Actor.h"

namespace Game {

namespace ACT {

class ItemPickup : Actor {
private:
    float speed_per_second_radians;
    float rotation_radians;
    uint32_t model_id; bool model;
    Graphics::ModelInstance *model_p;

public:
    ItemPickup( const Data::Accessor& accessor, const Data::Mission::ACT::ItemPickup& obj );
    ItemPickup( const ItemPickup& obj );
    virtual ~ItemPickup();

    virtual Actor* duplicate( const Actor &original ) const;

    virtual void resetGraphics( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

}

}

#endif // FC_GAME_ACT_NEUTRAL_TURRET_HEADER
