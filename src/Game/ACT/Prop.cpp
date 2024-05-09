#include "Prop.h"

namespace Game {

namespace ACT {

Prop::Prop( const Data::Accessor& accessor, const Data::Mission::ACT::Prop& obj ) : Actor( obj.getID() ) {
    Data::Mission::PTCResource &ptc = *accessor.getPTC( 1 );

    this->position = obj.getPosition( ptc );

    this->rotation = obj.getRotationQuaternion();

    this->model_id = obj.getObjResourceID();
    this->model_p = nullptr;
}

Prop::Prop( const Prop& obj ) :
    Actor( obj ),
    rotation( obj.rotation ), model_id( obj.model_id ), model_p( nullptr ) {}

Prop::~Prop() {
    if( this->model_p != nullptr )
        delete this->model_p;
}

Actor* Prop::duplicate( const Actor &original ) const {
    return new Prop( *this );
}

void Prop::resetGraphics( MainProgram &main_program ) {
    if( this->model_p != nullptr )
        delete this->model_p;
    this->model_p = nullptr;

    try {
        this->model_p = Graphics::ModelInstance::alloc( *main_program.environment_p, this->model_id, this->position, this->rotation );
    }
    catch( const std::invalid_argument& argument ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "Cobj with resource id " << model_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }
}

void Prop::update( MainProgram &main_program, std::chrono::microseconds delta ) {
}

}

}
