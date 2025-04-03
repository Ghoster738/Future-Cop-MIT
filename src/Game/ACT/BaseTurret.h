#ifndef FC_GAME_ACT_BASE_TURRET
#define FC_GAME_ACT_BASE_TURRET

#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/BaseTurret.h"
#include "../../Data/Mission/ObjResource.h"

#include "BaseShooter.h"

namespace Game::ACT {

class BaseTurret : public BaseShooter {
protected:
    glm::quat gun_rotation;

    uint32_t alive_gun_id; bool alive_gun;
    uint32_t  dead_gun_id; bool  dead_gun;

    const Data::Mission::ObjResource *alive_gun_cobj_r;
    const Data::Mission::ObjResource  *dead_gun_cobj_r;

    Graphics::ModelInstance *gun_p;

public:
    BaseTurret( const Data::Accessor& accessor, const Data::Mission::ACT::BaseTurret& obj );
    BaseTurret( const BaseTurret& obj ) :
        BaseShooter( obj ),
        gun_rotation( obj.gun_rotation ),
        alive_gun_id( obj.alive_gun_id ), alive_gun( obj.alive_gun ),
        dead_gun_id( obj.dead_gun_id ), dead_gun( obj.dead_gun ),
        alive_gun_cobj_r( obj.alive_gun_cobj_r ), dead_gun_cobj_r( obj.dead_gun_cobj_r ),
        gun_p( nullptr )  {}
    virtual ~BaseTurret();
};

}

#endif // FC_GAME_ACT_BASE_TURRET
