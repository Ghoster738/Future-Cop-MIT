#include "WalkableProp.h"

#include "../../Data/Mission/ObjResource.h"

namespace Game::ACT {

WalkableProp::WalkableProp( const Data::Accessor& accessor, const Data::Mission::ACT::WalkableProp& obj ) : BaseEntity( obj ) {
    const Data::Mission::PTCResource &ptc = *accessor.getConstPTC( 1 );

    this->position = obj.getPosition( ptc, obj.getHeightOffset(), Data::Mission::ACTResource::GroundCast::NONE );

    this->rotation = obj.getRotationQuaternion();

    this->alive    = obj.getHasAliveID();
    this->alive_id = obj.getAliveObjResourceID();
    this->alive_p  = nullptr;
}

WalkableProp::WalkableProp( const WalkableProp& obj ) :
    BaseEntity( obj ),
    rotation( obj.rotation ),
    alive_id( obj.alive_id ), alive( obj.alive ), alive_p( nullptr ) {}

WalkableProp::~WalkableProp() {
    if( this->alive_p != nullptr )
        delete this->alive_p;
}

Actor* WalkableProp::duplicate( const Actor &original ) const {
    return new WalkableProp( *this );
}

void WalkableProp::resetGraphics( MainProgram &main_program ) {
    auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );

    if( this->alive_p != nullptr )
        delete this->alive_p;
    this->alive_p = nullptr;

    try {
        if( this->alive ) {
            this->alive_p = main_program.environment_p->allocateModel( this->alive_id );

            if(this->alive_p) {
                this->alive_p->setPosition( this->position );
                this->alive_p->setRotation( this->rotation );
                this->alive_p->setVisable(true);
            }
        }
    }
    catch( const std::invalid_argument& argument ) {
        log.output << "Cobj with resource id " << this->alive_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }
}

void WalkableProp::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    if(this->alive_p) {
        this->alive_p->setPositionTransformTimeline( this->alive_p->getPositionTransformTimeline() + std::chrono::duration<float>( delta ).count() * 10.f);
    }
}

}
