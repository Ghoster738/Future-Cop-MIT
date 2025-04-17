#include "Elevator.h"

#include "../../Data/Mission/ObjResource.h"

namespace Game::ACT {

Elevator::Elevator( Utilities::Random &random, const Data::Accessor& accessor, const Data::Mission::ACT::Elevator& obj ) : BaseEntity( obj ) {
    const Data::Mission::PTCResource &ptc = *accessor.getConstPTC( 1 );

    this->position = obj.getPosition( ptc, obj.getHeightOffset( (obj.internal.starting_position - 1) % 3 ), Data::Mission::ACTResource::GroundCast::HIGH );

    this->rotation = obj.getRotationQuaternion();

    this->has_model = obj.getHasElevatorID();
    this->model_id  = obj.getElevatorID();
    this->model_p   = nullptr;
}

Elevator::Elevator( const Elevator& obj ) :
    BaseEntity( obj ),
    rotation( obj.rotation ),
    model_id( obj.model_id ), has_model( obj.has_model ), model_p( nullptr ) {}

Elevator::~Elevator() {
    if( this->model_p != nullptr )
        delete this->model_p;
}

Actor* Elevator::duplicate( const Actor &original ) const {
    return new Elevator( *this );
}

void Elevator::resetGraphics( MainProgram &main_program ) {
    auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );

    if( this->model_p != nullptr )
        delete this->model_p;
    this->model_p = nullptr;

    try {
        if( this->has_model ) {
            this->model_p = main_program.environment_p->allocateModel( this->model_id );

            if(this->model_p) {
                this->model_p->setPosition( this->position );
                this->model_p->setRotation( this->rotation );
                this->model_p->setTextureOffset( this->texture_offset );
                this->model_p->setVisable(!this->entity_bitfield.disable_rendering);
            }
        }
    }
    catch( const std::invalid_argument& argument ) {
        log.output << "Cobj with resource id " << this->model_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }
}

void Elevator::update( MainProgram &main_program, std::chrono::microseconds delta ) {}

}
