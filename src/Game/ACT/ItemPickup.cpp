#include "ItemPickup.h"

namespace Game::ACT {

ItemPickup::ItemPickup( Utilities::Random &random, const Data::Accessor& accessor, const Data::Mission::ACT::ItemPickup& obj ) : BaseEntity( obj ) {
    const Data::Mission::PTCResource &ptc = *accessor.getConstPTC( 1 );

    this->position = obj.getPosition( ptc, 0.f, static_cast<Data::Mission::ACTResource::GroundCast>(obj.internal.ground_cast_type) );

    this->speed_per_second_radians = obj.getRotationSpeed();
    this->rotation_radians = 0;

    this->blink_time_line = 0;
    this->has_blink = obj.hasBlink();

    this->model = obj.getHasItemID();
    this->model_id = obj.getItemID();
    this->model_p = nullptr;
}

ItemPickup::ItemPickup( const ItemPickup& obj ) :
    BaseEntity( obj ),
    speed_per_second_radians( obj.speed_per_second_radians ), rotation_radians( obj.rotation_radians ),
    blink_time_line( obj.blink_time_line ), has_blink( obj.has_blink ),
    model_id( obj.model_id ), model( obj.model ), model_p( nullptr ) {}

ItemPickup::~ItemPickup() {
    if( this->model_p != nullptr )
        delete this->model_p;
}

Actor* ItemPickup::duplicate( const Actor &original ) const {
    return new ItemPickup( *this );
}

void ItemPickup::resetGraphics( MainProgram &main_program ) {
    if( this->model_p != nullptr )
        delete this->model_p;
    this->model_p = nullptr;

    try {
        if( this->model ) {
            this->model_p = main_program.environment_p->allocateModel( this->model_id );

            if(this->model_p) {
                this->model_p->setPosition( this->position );
                this->model_p->setTextureOffset( this->texture_offset );
                this->model_p->setVisable( !this->entity_bitfield.disable_rendering );
            }
        }
    }
    catch( const std::invalid_argument& argument ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "Cobj with resource id " << model_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }
}

void ItemPickup::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    this->rotation_radians += std::chrono::duration<float>( delta ).count() * this->speed_per_second_radians;

    if(this->rotation_radians > glm::tau<float>()) {
        this->rotation_radians -= glm::tau<float>() * std::abs(static_cast<int>(this->rotation_radians / glm::tau<float>()));
    }

    glm::quat rotation( glm::vec3( 0, this->rotation_radians, 0) );

    if(this->model_p)
        this->model_p->setRotation( rotation );

    if(this->has_blink) {
        this->blink_time_line += std::chrono::duration<float>( delta ).count();

        if(this->blink_time_line > 1)
            this->blink_time_line -= std::abs(static_cast<int>(this->blink_time_line));

        if(this->model_p) {
            if(0.5 > this->blink_time_line)
                this->model_p->setColor( glm::vec3(1.0f, 0.5f, 0.5f) );
            else
                this->model_p->setColor( glm::vec3(1.0f, 1.0f, 1.0f) );
        }
    }
}

}
