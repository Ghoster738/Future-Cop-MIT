#include "PathedTurret.h"

#include "../../Data/Mission/ObjResource.h"

namespace Game::ACT {

PathedTurret::PathedTurret( Utilities::Random &random, const Data::Accessor& accessor, const Data::Mission::ACT::PathedTurret& obj ) : BasePathedEntity( random, accessor, obj ) {
    this->alive_id = obj.getAliveID();
    this->alive_base = obj.getHasAliveID();

    this->dead_id = obj.getDestroyedID();
    this->dead_base = obj.getHasDestroyedID();

    this->gun_id = obj.getGunID();
    this->gun_base = obj.getHasGunID();

    this->alive_cobj_r = nullptr;
    this->dead_cobj_r  = nullptr;
    this->gun_cobj_r   = nullptr;

    if( this->alive_base )
        this->alive_cobj_r = accessor.getConstOBJ( this->alive_id );

    if( this->dead_base )
        this->dead_cobj_r = accessor.getConstOBJ( this->dead_id );

    if( this->gun_base )
        this->gun_cobj_r = accessor.getConstOBJ( this->gun_id );

    this->alive_p = nullptr;
    this->gun_p   = nullptr;

    // TODO: Find actual value to determine this.
    this->gun_parent_index = obj.internal.uint16_19 % 4;
}

PathedTurret::PathedTurret( const PathedTurret& obj ) :
    BasePathedEntity( obj ),
    alive_id( obj.alive_id ), alive_base( obj.alive_base ),
    dead_id( obj.dead_id ), dead_base( obj.dead_base ),
    gun_id( obj.gun_id ), gun_base( obj.gun_base ),
    alive_cobj_r( obj.alive_cobj_r ), dead_cobj_r( obj.dead_cobj_r ), gun_cobj_r( obj.gun_cobj_r ),
    alive_p( nullptr ), gun_p( nullptr ), gun_parent_index( obj.gun_parent_index ) {}

PathedTurret::~PathedTurret() {
    if( this->alive_p != nullptr )
        delete this->alive_p;

    if( this->gun_p != nullptr )
        delete this->gun_p;
}

Actor* PathedTurret::duplicate( const Actor &original ) const {
    return new PathedTurret( *this );
}

void PathedTurret::resetGraphics( MainProgram &main_program ) {
    if( this->alive_p != nullptr )
        delete this->alive_p;
    this->alive_p = nullptr;

    try {
        if( this->alive_base ) {
            this->alive_p = main_program.environment_p->allocateModel( this->alive_id );

            if(this->alive_p) {
                this->alive_p->setPosition( this->position );
                this->alive_p->setRotation( this->next_node_rot );
                this->alive_p->setTextureOffset( this->texture_offset );
                this->alive_p->setVisable( !this->entity_bitfield.disable_rendering );
            }
        }
    }
    catch( const std::invalid_argument& argument ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "Cobj with resource id " << alive_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }

    if( this->gun_p != nullptr )
        delete this->gun_p;
    this->gun_p = nullptr;

    try {
        if( this->gun_base ) {
            this->gun_p = main_program.environment_p->allocateModel( this->gun_id );

            if(this->gun_p) {
                this->gun_p->setPosition( this->position );
                this->gun_p->setRotation( this->next_node_rot );
                this->gun_p->setTextureOffset( this->texture_offset );
                this->gun_p->setVisable( !this->entity_bitfield.disable_rendering );
            }
        }
    }
    catch( const std::invalid_argument& argument ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "Cobj with resource id " << gun_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }
}

void PathedTurret::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    glm::vec3 new_position = getCurrentPosition( delta );

    if(this->alive_p) {
        this->alive_p->setPositionTransformTimeline( this->alive_p->getPositionTransformTimeline() + std::chrono::duration<float>( delta ).count() * 10.f);

        this->alive_p->setPosition( new_position );
        this->alive_p->setRotation( this->next_node_rot );
    }

    if(this->gun_p) {
        this->gun_p->setPositionTransformTimeline( this->gun_p->getPositionTransformTimeline() + std::chrono::duration<float>( delta ).count() * 10.f);

        glm::vec3 gun_position = glm::vec3( 0, 0, 0 );

        if( this->alive_cobj_r != nullptr )
            gun_position = this->next_node_rot * this->alive_cobj_r->getPosition( this->gun_parent_index, 0 );

        this->gun_p->setPosition( new_position + gun_position );
        this->gun_p->setRotation( this->next_node_rot );
    }
}

}
