#ifndef FC_GAME_ACT_BASE_TURRET
#define FC_GAME_ACT_BASE_TURRET

#include "../../Data/Mission/ACT/BaseShooterTurret.h"

#include "BaseShooter.h"

namespace Game::ACT {

class BaseTurret : public BaseShooter {
protected:

public:
    BaseTurret( const Data::Mission::ACT::BaseShooterTurret& obj ) : BaseShooter( obj ) {}
    BaseTurret( const BaseTurret& obj ) : BaseShooter( obj ) {}
    virtual ~BaseTurret() {}
};

}

#endif // FC_GAME_ACT_BASE_TURRET
