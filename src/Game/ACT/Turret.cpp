#include "Turret.h"

#include "../../Data/Mission/ObjResource.h"

namespace Game::ACT {

Turret::Turret( Utilities::Random &random, const Data::Accessor& accessor, const Data::Mission::ACT::Turret& obj ) : BaseTurret( accessor, obj ) {
    this->base_rotation = obj.getBaseRotationQuaternion();

    this->alive_base_id = obj.getAliveBaseID();
    this->alive_base = obj.getHasAliveBaseID();

    this->dead_base_id = obj.getDestroyedBaseID();
    this->dead_base = obj.getHasDestroyedBaseID();

    this->alive_base_cobj_r = nullptr;
    this->dead_base_cobj_r  = nullptr;

    if( this->alive_base )
        this->alive_base_cobj_r = accessor.getConstOBJ( this->alive_base_id );

    if( this->dead_base )
        this->dead_base_cobj_r = accessor.getConstOBJ( this->dead_base_id );

    this->base_p = nullptr;

    this->rest_gun_rotation = this->gun_rotation;
}

Turret::Turret( const Turret& obj ) :
    BaseTurret( obj ),
    rest_gun_rotation( obj.rest_gun_rotation ), base_rotation( obj.base_rotation ),
    alive_base_id( obj.alive_base_id ), alive_base( obj.alive_base ),
    dead_base_id( obj.dead_base_id ), dead_base( obj.dead_base ),
    alive_base_cobj_r( obj.alive_base_cobj_r ), dead_base_cobj_r( obj.dead_base_cobj_r ),
    base_p( nullptr ) {}

Turret::~Turret() {
    if( this->base_p != nullptr )
        delete this->base_p;
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
            gun_position = this->base_rotation * this->alive_base_cobj_r->getPosition( 0, 0 );

        if( this->alive_base ) {
            this->base_p = main_program.environment_p->allocateModel( this->alive_base_id );

            if(this->base_p) {
                this->base_p->setPosition( this->position );
                this->base_p->setRotation( this->base_rotation );
                this->base_p->setTextureOffset( this->texture_offset );
                this->base_p->setVisable( true );
            }
        }

        if( this->alive_gun ) {
            this->gun_p = main_program.environment_p->allocateModel( this->alive_gun_id );

            if(this->gun_p) {
                this->gun_p->setPosition( this->position + gun_position );
                this->gun_p->setRotation( this->gun_rotation );
                this->gun_p->setTextureOffset( this->texture_offset );
                this->gun_p->setVisable( true );
            }
        }
    }
    catch( const std::invalid_argument& argument ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "Cobj with resource id " << alive_base_id << "or " << this->alive_gun_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }
}

void Turret::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    if(this->base_p) {
        this->base_p->setPositionTransformTimeline( this->base_p->getPositionTransformTimeline() + std::chrono::duration<float>( delta ).count() * 10.f);
    }
    if(this->gun_p) {
        this->gun_p->setPositionTransformTimeline( this->gun_p->getPositionTransformTimeline() + std::chrono::duration<float>( delta ).count() * 10.f);
    }
}

}
