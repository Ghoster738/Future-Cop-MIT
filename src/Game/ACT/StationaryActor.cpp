#include "StationaryActor.h"

#include "../../Data/Mission/ObjResource.h"

namespace Game::ACT {

StationaryActor::StationaryActor( Utilities::Random &random, const Data::Accessor& accessor, const Data::Mission::ACT::StationaryActor& obj ) : BaseTurret( accessor, obj ) {}

StationaryActor::StationaryActor( const StationaryActor& obj ) : BaseTurret( obj ) {}

StationaryActor::~StationaryActor() {}

Actor* StationaryActor::duplicate( const Actor &original ) const {
    return new StationaryActor( *this );
}

void StationaryActor::resetGraphics( MainProgram &main_program ) {
    if( this->gun_p != nullptr )
        delete this->gun_p;
    this->gun_p = nullptr;

    try {
        if( this->alive_gun ) {
            this->gun_p = main_program.environment_p->allocateModel( this->alive_gun_id );

            if(this->gun_p) {
                this->gun_p->setPosition( this->position );
                this->gun_p->setRotation( this->gun_rotation );
                this->gun_p->setTextureOffset( this->texture_offset );
                this->gun_p->setVisable( true );
            }
        }
    }
    catch( const std::invalid_argument& argument ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "Cobj with resource id " << this->alive_gun_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }
}

void StationaryActor::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    if(this->gun_p) {
        this->gun_p->setPositionTransformTimeline( this->gun_p->getPositionTransformTimeline() + std::chrono::duration<float>( delta ).count() * 10.f);
    }
}

}
