#include "BaseTurret.h"

namespace Game::ACT {

BaseTurret::BaseTurret( const Data::Accessor& accessor, const Data::Mission::ACT::BaseTurret& obj ) : BaseShooter( obj ) {
    const Data::Mission::PTCResource &ptc = *accessor.getConstPTC( 1 );

    this->position = obj.getPosition( ptc, obj.getHeightOffset(), static_cast<Data::Mission::ACTResource::GroundCast>(obj.turret_shooter_internal.ground_cast_type) );

    this->gun_rotation = obj.getGunRotationQuaternion();

    this->alive_gun_id = obj.getAliveGunID();
    this->alive_gun = obj.getHasAliveGunID();
    this->dead_gun_id = obj.getDestroyedGunID();
    this->dead_gun = obj.getHasDestroyedGunID();

    this->alive_gun_cobj_r  = nullptr;
    this->dead_gun_cobj_r   = nullptr;

    if( this->alive_gun )
        this->alive_gun_cobj_r = accessor.getConstOBJ( this->alive_gun_id );

    if( this->dead_gun )
        this->dead_gun_cobj_r = accessor.getConstOBJ( this->dead_gun_id );

    this->gun_p = nullptr;
}

BaseTurret::~BaseTurret() {
    if( this->gun_p != nullptr )
        delete this->gun_p;
}

}
