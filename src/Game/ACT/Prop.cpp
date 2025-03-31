#include "Prop.h"

namespace Game {

namespace ACT {

Prop::Prop( const Data::Accessor& accessor, const Data::Mission::ACT::Prop& obj ) : Actor( obj.getID() ) {
    const Data::Mission::PTCResource &ptc = *accessor.getConstPTC( 1 );

    this->position = obj.getPosition( ptc, static_cast<Data::Mission::ACTResource::GroundCast>(obj.internal.ground_cast_type) );

    this->position.y += obj.getHeightOffset();

    this->rotation = obj.getRotationQuaternion( 0 );

    this->has_animated_rotation = false;

    if( obj.hasSpin() ) {
        this->has_animated_rotation = true;
        this->rotation_points[0] = obj.getRotationQuaternion( 0 );
        this->rotation_points[1] = obj.getRotationQuaternion( 1 );
        this->rotation_time_line = 0.0f;
    }

    this->model_id = obj.getObjResourceID();
    this->model_p = nullptr;
}

Prop::Prop( const Prop& obj ) :
    Actor( obj ),
    rotation( obj.rotation ), model_id( obj.model_id ), model_p( nullptr ),
    rotation_points{obj.rotation_points[0], obj.rotation_points[1], obj.rotation_points[2]},
    has_animated_rotation( obj.has_animated_rotation ),
    rotation_time_line( obj.rotation_time_line ){}

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
        this->model_p = main_program.environment_p->allocateModel( this->model_id, this->position, this->rotation );
    }
    catch( const std::invalid_argument& argument ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "Cobj with resource id " << model_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }
}

void Prop::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    if(!this->has_animated_rotation)
        return;

    this->rotation_time_line += std::chrono::duration<float>( delta ).count();

    if(this->rotation_time_line > 1.0f)
        this->rotation_time_line -= 2.0f;

    if(this->rotation_time_line < 0.0f)
        this->rotation = glm::mix(this->rotation_points[1], this->rotation_points[0], 1.0f + this->rotation_time_line);
    else
        this->rotation = glm::mix(this->rotation_points[0], this->rotation_points[1], this->rotation_time_line);

    this->model_p->setRotation( this->rotation );
}

}

}
