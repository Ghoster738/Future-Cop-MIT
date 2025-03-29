#include "SkyCaptain.h"

namespace Game {

namespace ACT {

SkyCaptain::SkyCaptain( const Data::Accessor& accessor, const Data::Mission::ACT::SkyCaptain& obj ) : Actor( obj.getID() ) {
    const Data::Mission::PTCResource &ptc = *accessor.getConstPTC( 1 );

    {
        auto v = obj.getSpawnPosition();
        this->position = glm::vec3( v.x, ptc.getRayCast2D( v.x, v.y ), v.y );
    }

    this->position.y += obj.getHeightOffset();

    this->model = obj.hasModelID();
    this->model_id = obj.getModelID();
    this->model_p = nullptr;
}

SkyCaptain::SkyCaptain( const SkyCaptain& obj ) :
    Actor( obj ),
    model_id( obj.model_id ), model( obj.model ), model_p( nullptr ) {}

SkyCaptain::~SkyCaptain() {
    if( this->model_p != nullptr )
        delete this->model_p;
}

Actor* SkyCaptain::duplicate( const Actor &original ) const {
    return new SkyCaptain( *this );
}

void SkyCaptain::resetGraphics( MainProgram &main_program ) {
    if( this->model_p != nullptr )
        delete this->model_p;
    this->model_p = nullptr;

    try {
        if( this->model )
            this->model_p = main_program.environment_p->allocateModel( this->model_id, this->position );
    }
    catch( const std::invalid_argument& argument ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "Cobj with resource id " << model_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }
}

void SkyCaptain::update( MainProgram &main_program, std::chrono::microseconds delta ) {
}

}

}
