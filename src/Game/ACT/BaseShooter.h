#ifndef FC_GAME_ACT_BASE_SHOOTER
#define FC_GAME_ACT_BASE_SHOOTER

#include "../../Data/Mission/ACT/BaseShooter.h"

#include "BaseEntity.h"

namespace Game::ACT {

class BaseShooter : public BaseEntity {
protected:

public:
    BaseShooter( const Data::Mission::ACT::BaseShooter& obj ) : BaseEntity( obj ) {}
    BaseShooter( const BaseShooter& obj ) : BaseEntity( obj ) {}
    virtual ~BaseShooter() {}
};

}

#endif // FC_GAME_ACT_BASE_SHOOTER
