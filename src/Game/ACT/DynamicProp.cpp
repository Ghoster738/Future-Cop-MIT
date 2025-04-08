#include "DynamicProp.h"

#include "../../Data/Mission/ObjResource.h"

namespace Game::ACT {

DynamicProp::DynamicProp( const Data::Accessor& accessor, const Data::Mission::ACT::DynamicProp& obj ) : BaseEntity( obj ) {
    const Data::Mission::PTCResource &ptc = *accessor.getConstPTC( 1 );

    this->position = obj.getPosition( ptc, obj.getHeightOffset(), static_cast<Data::Mission::ACTResource::GroundCast>(obj.internal.ground_cast_type) );

    this->rotation = obj.getRotationQuaternion();

    this->alive    = obj.getHasAliveID();
    this->alive_id = obj.getAliveObjResourceID();
    this->alive_p  = nullptr;
}

DynamicProp::DynamicProp( const DynamicProp& obj ) :
    BaseEntity( obj ),
    rotation( obj.rotation ),
    alive_id( obj.alive_id ), alive( obj.alive ), alive_p( nullptr ) {}

DynamicProp::~DynamicProp() {
    if( this->alive_p != nullptr )
        delete this->alive_p;
}

Actor* DynamicProp::duplicate( const Actor &original ) const {
    return new DynamicProp( *this );
}

void DynamicProp::resetGraphics( MainProgram &main_program ) {
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

void DynamicProp::update( MainProgram &main_program, std::chrono::microseconds delta ) {}

}
