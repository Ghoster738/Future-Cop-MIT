#include "ItemPickup.h"

namespace Game {

namespace ACT {

ItemPickup::ItemPickup( const Data::Accessor& accessor, const Data::Mission::ACT::ItemPickup& obj ) : Actor( obj.getID() ) {
    Data::Mission::PTCResource &ptc = *accessor.getPTC( 1 );

    this->position = obj.getPosition( ptc );
    this->speed_per_second_radians = 0.128;
    this->rotation_radians = 0;

    this->model = obj.getHasItemID();
    this->model_id = obj.getItemID();
    this->model_p = nullptr;
}

ItemPickup::ItemPickup( const ItemPickup& obj ) :
    Actor( obj ),
    speed_per_second_radians( obj.speed_per_second_radians ), model_id( obj.model_id ), model( obj.model ), model_p( nullptr ) {}

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
        if( this->model )
            this->model_p = Graphics::ModelInstance::alloc( *main_program.environment_p, this->model_id, this->position );
    }
    catch( const std::invalid_argument& argument ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "Cobj with resource id " << model_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }
}

void ItemPickup::update( MainProgram &main_program, std::chrono::microseconds delta ) {
}

}

}
