#include "Prop.h"

namespace Game {

namespace ACT {

Prop::Prop( const Data::Accessor& accessor, const Data::Mission::ACT::Prop& obj ) : Actor( obj.getID() ) {
    const Data::Mission::PTCResource &ptc = *accessor.getConstPTC( 1 );

    this->position = obj.getPosition( ptc, static_cast<Data::Mission::ACTResource::GroundCast>(obj.internal.ground_cast_type) );

    this->position.y += obj.getHeightOffset();

    this->rotation = obj.getRotationQuaternion();

    this->has_animated_rotation = false;

    if( obj.hasSpin() ) {
        this->has_animated_rotation = true;
        this->rotation_point_0 = obj.getRotationQuaternion( 0.0f );
        this->rotation_point_1 = obj.getRotationQuaternion( 1.0f );
        this->a = 0.0f;
    }

    this->model_id = obj.getObjResourceID();
    this->model_p = nullptr;
}

Prop::Prop( const Prop& obj ) :
    Actor( obj ),
    rotation( obj.rotation ), model_id( obj.model_id ), model_p( nullptr ),
    has_animated_rotation( obj.has_animated_rotation ),
    rotation_point_0( obj.rotation_point_0 ), rotation_point_1( obj.rotation_point_1 ),
    a( obj.a ){}

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

    this->a += std::chrono::duration<float>( delta ).count();

    if(this->a > 1.0f)
        this->a -= 1.0f;

    this->rotation = glm::mix(this->rotation_point_0, this->rotation_point_1, this->a);

    this->model_p->setRotation( this->rotation );
}

}

}
