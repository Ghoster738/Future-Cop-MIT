#include "Turret.h"

#include "../../Data/Mission/ObjResource.h"

namespace Game::ACT {

Turret::Turret( const Data::Accessor& accessor, const Data::Mission::ACT::Turret& obj ) : BaseTurret( obj ) {
    const Data::Mission::PTCResource &ptc = *accessor.getConstPTC( 1 );

    this->position = obj.getPosition( ptc, static_cast<Data::Mission::ACTResource::GroundCast>(obj.turret_shooter_internal.ground_cast_type) );

    this->position.y += obj.getHeightOffset();

    this->rest_gun_rotation = obj.getGunRotationQuaternion();
    this->gun_rotation = this->rest_gun_rotation;
    this->base_rotation = obj.getBaseRotationQuaternion();

    this->texture_offset = obj.getTextureOffset();

    this->alive_gun_id = obj.getAliveGunID();
    this->alive_gun = obj.getHasAliveGunID();
    this->alive_base_id = obj.getAliveBaseID();
    this->alive_base = obj.getHasAliveBaseID();
    this->dead_gun_id = obj.getDestroyedGunID();
    this->dead_gun = obj.getHasDestroyedGunID();
    this->dead_base_id = obj.getDestroyedBaseID();
    this->dead_base = obj.getHasDestroyedBaseID();

    this->alive_gun_cobj_r  = nullptr;
    this->alive_base_cobj_r = nullptr;
    this->dead_gun_cobj_r   = nullptr;
    this->dead_base_cobj_r  = nullptr;

    if( this->alive_gun )
        this->alive_gun_cobj_r = accessor.getConstOBJ( this->alive_gun_id );

    if( this->alive_base )
        this->alive_base_cobj_r = accessor.getConstOBJ( this->alive_base_id );

    if( this->dead_gun )
        this->dead_gun_cobj_r = accessor.getConstOBJ( this->dead_gun_id );

    if( this->dead_base )
        this->dead_base_cobj_r = accessor.getConstOBJ( this->dead_base_id );

    this->base_p = nullptr;
    this->gun_p = nullptr;
}

Turret::Turret( const Turret& obj ) :
    BaseTurret( obj ),
    rest_gun_rotation( obj.rest_gun_rotation ), gun_rotation( obj.gun_rotation ), base_rotation( obj.base_rotation ), texture_offset( obj.texture_offset ),
    alive_gun_id( obj.alive_gun_id ), alive_gun( obj.alive_gun ), alive_base_id( obj.alive_base_id ), alive_base( obj.alive_base ),
    dead_gun_id( obj.dead_gun_id ), dead_gun( obj.dead_gun ), dead_base_id( obj.dead_base_id ), dead_base( obj.dead_base ),
    alive_gun_cobj_r( obj.alive_gun_cobj_r ), alive_base_cobj_r( obj.alive_base_cobj_r ), dead_gun_cobj_r( obj.dead_gun_cobj_r ), dead_base_cobj_r( obj.dead_base_cobj_r ),
    base_p( nullptr ), gun_p( nullptr ) {}

Turret::~Turret() {
    if( this->base_p != nullptr )
        delete this->base_p;

    if( this->gun_p != nullptr )
        delete this->gun_p;
}

Actor* Turret::duplicate( const Actor &original ) const {
    return new Turret( *this );
}

void Turret::resetGraphics( MainProgram &main_program ) {
    if( this->base_p != nullptr )
        delete this->base_p;
    this->base_p = nullptr;

    if( this->gun_p != nullptr )
        delete this->gun_p;
    this->gun_p = nullptr;

    try {
        glm::vec3 gun_position = glm::vec3( 0, 0.25, 0 );

        if( this->alive_base_cobj_r != nullptr )
            gun_position = this->base_rotation * this->alive_base_cobj_r->getPosition( 0 );

        if( this->alive_base )
            this->base_p = main_program.environment_p->allocateModel( this->alive_base_id, this->position, this->base_rotation, this->texture_offset );

        if( this->alive_gun )
            this->gun_p = main_program.environment_p->allocateModel( this->alive_gun_id, this->position + gun_position, this->gun_rotation, this->texture_offset );
    }
    catch( const std::invalid_argument& argument ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "Cobj with resource id " << alive_base_id << "or " << this->alive_gun_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }
}

void Turret::update( MainProgram &main_program, std::chrono::microseconds delta ) {
}

}
