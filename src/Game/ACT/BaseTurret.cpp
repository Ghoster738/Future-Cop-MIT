#include "BaseTurret.h"

namespace Game::ACT {

BaseTurret::BaseTurret( const Data::Accessor& accessor, const Data::Mission::ACT::BaseShooterTurret& obj ) : BaseShooter( obj ) {
    const Data::Mission::PTCResource &ptc = *accessor.getConstPTC( 1 );

    this->position = obj.getPosition( ptc, static_cast<Data::Mission::ACTResource::GroundCast>(obj.turret_shooter_internal.ground_cast_type) );

    this->position.y += obj.getHeightOffset();
}

}
