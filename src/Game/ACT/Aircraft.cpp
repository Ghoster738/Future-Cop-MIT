#include "Aircraft.h"

namespace Game {

namespace ACT {

Aircraft::Aircraft( const Data::Accessor& accessor, const Data::Mission::ACT::Aircraft& obj ) : Actor( obj.getID() ) {
    const Data::Mission::PTCResource &ptc = *accessor.getConstPTC( 1 );

    {
        auto v = obj.getSpawnPosition();
        this->position = glm::vec3( v.x, ptc.getRayCast2D( v.x, v.y, Data::Mission::ACTResource::GroundCast::HIGH ), v.y );
    }

    this->position.y += obj.getHeightOffset();

    this->texture_offset = obj.getTextureOffset();

    this->model = obj.hasModelID();
    this->model_id = obj.getModelID();
    this->model_p = nullptr;
}

Aircraft::Aircraft( const Aircraft& obj ) :
    Actor( obj ), texture_offset( obj.texture_offset ),
    model_id( obj.model_id ), model( obj.model ), model_p( nullptr ) {}

Aircraft::~Aircraft() {
    if( this->model_p != nullptr )
        delete this->model_p;
}

Actor* Aircraft::duplicate( const Actor &original ) const {
    return new Aircraft( *this );
}

void Aircraft::resetGraphics( MainProgram &main_program ) {
    if( this->model_p != nullptr )
        delete this->model_p;
    this->model_p = nullptr;

    try {
        if( this->model )
            this->model_p = main_program.environment_p->allocateModel( this->model_id, this->position, glm::quat(), this->texture_offset );
    }
    catch( const std::invalid_argument& argument ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "Cobj with resource id " << model_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }
}

void Aircraft::update( MainProgram &main_program, std::chrono::microseconds delta ) {
}

}

}
