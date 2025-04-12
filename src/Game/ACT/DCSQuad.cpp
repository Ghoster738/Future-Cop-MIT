#include "DCSQuad.h"

namespace Game::ACT {

DCSQuad::DCSQuad( Utilities::Random &random, const Data::Accessor& accessor, const Data::Mission::ACT::DCSQuad& obj ) : Actor( obj.getID() ) {
    const Data::Mission::PTCResource &ptc = *accessor.getConstPTC( 1 );

    // TODO GroundCast is a wild guess it is to be confirmed!
    this->position = obj.getPosition( ptc, obj.getHeightOffset(), static_cast<Data::Mission::ACTResource::GroundCast>(obj.internal.uint8_2) );

    this->color = obj.getColor();
    this->scale = obj.getScale();

    this->rotation = obj.getRotationQuaternion();

    this->model_id = 13;
    this->model_p = nullptr;
}

DCSQuad::DCSQuad( const DCSQuad& obj ) :
    Actor( obj ),
    color( obj.color ), rotation( obj.rotation ), scale( obj.scale ), model_id( obj.model_id ), model_p( nullptr ) {}

DCSQuad::~DCSQuad() {
    if( this->model_p != nullptr )
        delete this->model_p;
}

Actor* DCSQuad::duplicate( const Actor &original ) const {
    return new DCSQuad( *this );
}

void DCSQuad::resetGraphics( MainProgram &main_program ) {
    if( this->model_p != nullptr )
        delete this->model_p;
    this->model_p = nullptr;

    try {
        this->model_p = main_program.environment_p->allocateModel( this->model_id );

        if(this->model_p) {
            this->model_p->setPosition( this->position );
            this->model_p->setRotation( this->rotation );
            this->model_p->setScale( this->scale );
            this->model_p->setVisable( true );
        }
    }
    catch( const std::invalid_argument& argument ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "Cobj with resource id " << model_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }
}

void DCSQuad::update( MainProgram &main_program, std::chrono::microseconds delta ) {}

}
