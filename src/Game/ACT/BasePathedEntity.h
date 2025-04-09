#ifndef FC_GAME_ACT_BASE_PATHED_ENTITY_TURRET
#define FC_GAME_ACT_BASE_PATHED_ENTITY_TURRET

#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/BasePathedEntity.h"

#include "BaseShooter.h"

namespace Game::ACT {

class BasePathedEntity : public BaseShooter {
public:
    BasePathedEntity( const Data::Mission::ACT::BasePathedEntity& obj ) : BaseShooter( obj ) {}
    BasePathedEntity( const BasePathedEntity& obj ) : BaseShooter( obj )  {}
    virtual ~BasePathedEntity() {}
};

}

#endif // FC_GAME_ACT_BASE_PATHED_ENTITY_TURRET
