#include "DCSQuad.h"

namespace Game::ACT {

DCSQuad::DCSQuad( Utilities::Random &random, const Data::Accessor& accessor, const Data::Mission::ACT::DCSQuad& obj ) : Actor( obj.getID() ) {
    const Data::Mission::PTCResource &ptc = *accessor.getConstPTC( 1 );

    this->position = obj.getPosition( ptc, obj.getHeightOffset(), static_cast<Data::Mission::ACTResource::GroundCast>(obj.internal.ground_cast_type) );

    this->color = obj.getColor();
    this->scale = obj.getScale();

    this->rotation = obj.getRotationQuaternion();

    /*
    if( (tags & 0x10) != 0 )
        render_mode = ADDITION;
    else
    if( (tags & 0x02) != 0 )
        render_mode = MIX;
    else
        render_mode = OPAQUE_WITH_CUTOFF;*/

    this->dcs_id = obj.internal.dcs_id;
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
        this->dcs_p = main_program.environment_p->allocateQuadInstance();

        if(this->dcs_p) {
            this->dcs_p->position = this->position;
            this->dcs_p->rotation = this->rotation;
            this->dcs_p->color    = this->color;
            this->dcs_p->span     = this->scale;
            this->dcs_p->setQuadID( this->dcs_id );

            this->dcs_p->update();
        }
    }
    catch( const std::invalid_argument& argument ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "Cobj with resource id " << dcs_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }
}

void DCSQuad::update( MainProgram &main_program, std::chrono::microseconds delta ) {}

}
