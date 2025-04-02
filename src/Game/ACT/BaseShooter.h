#ifndef FC_GAME_ACT_BASE_SHOOTER
#define FC_GAME_ACT_BASE_SHOOTER

#include "../../Data/Mission/ACT/BaseShooterEntity.h"

#include "BaseEntity.h"

namespace Game::ACT {

class BaseShooter : public BaseEntity {
protected:

public:
    BaseShooter( const Data::Mission::ACT::BaseShooterEntity& obj ) : BaseEntity( obj ) {}
    BaseShooter( const BaseShooter& obj ) : BaseEntity( obj ) {}
    virtual ~BaseShooter() {}
};

}

#endif // FC_GAME_ACT_BASE_SHOOTER
