#include "DCSQuad.h"

namespace Game::ACT {

DCSQuad::DCSQuad( Utilities::Random &random, const Data::Accessor& accessor, const Data::Mission::ACT::DCSQuad& obj ) : Actor( obj.getID() ) {
    const Data::Mission::PTCResource &ptc = *accessor.getConstPTC( 1 );

    // TODO GroundCast is a wild guess it is to be confirmed!
    this->position = obj.getPosition( ptc, obj.getHeightOffset(), static_cast<Data::Mission::ACTResource::GroundCast>(obj.internal.uint8_2) );

    this->color = obj.getColor();
    this->scale = obj.getScale();

    this->rotation = obj.getRotationQuaternion();

    this->dcs_id = 13;
    this->dcs_p = nullptr;
}

DCSQuad::DCSQuad( const DCSQuad& obj ) :
    Actor( obj ),
    color( obj.color ), rotation( obj.rotation ), scale( obj.scale ), dcs_id( obj.dcs_id ), dcs_p( nullptr ) {}

DCSQuad::~DCSQuad() {
    if( this->dcs_p != nullptr )
        delete this->dcs_p;
}

Actor* DCSQuad::duplicate( const Actor &original ) const {
    return new DCSQuad( *this );
}

void DCSQuad::resetGraphics( MainProgram &main_program ) {
    if( this->dcs_p != nullptr )
        delete this->dcs_p;
    this->dcs_p = nullptr;

    try {
        this->dcs_p = main_program.environment_p->allocateModel( this->dcs_id );

        if(this->dcs_p) {
            this->dcs_p->setPosition( this->position );
            this->dcs_p->setRotation( this->rotation );
            this->dcs_p->setColor( this->color );
            this->dcs_p->setScale( this->scale );
            this->dcs_p->setVisable( true );
        }
    }
    catch( const std::invalid_argument& argument ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "Cobj with resource id " << dcs_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }
}

void DCSQuad::update( MainProgram &main_program, std::chrono::microseconds delta ) {}

}
