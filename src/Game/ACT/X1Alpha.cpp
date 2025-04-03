#include "X1Alpha.h"

namespace Game::ACT {

X1Alpha::X1Alpha( const Data::Accessor& accessor, const Data::Mission::ACT::X1Alpha& obj ) : BaseEntity( obj ) {
    const Data::Mission::PTCResource &ptc = *accessor.getConstPTC( 1 );

    // TODO This is a wild guess.
    this->position = obj.getPosition( ptc, static_cast<Data::Mission::ACTResource::GroundCast>(obj.internal.uint8_0) );

    this->legs    = obj.getHasLegID();
    this->legs_id = obj.getLegID();
    this->legs_p  = nullptr;

    this->cockpit    = obj.getHasCockpitID();
    this->cockpit_id = obj.getCockpitID();
    this->cockpit_p  = nullptr;

    this->weapon    = obj.getHasWeaponID();
    this->weapon_id = obj.getWeaponID();
    this->weapon_p  = nullptr;

    this->beacon_light    = obj.getHasBeaconLightsID();
    this->beacon_light_id = obj.getBeaconLightsID();
    this->beacon_light_p  = nullptr;

    this->pilot    = obj.getHasPilotID();
    this->pilot_id = obj.getPilotID();
    this->pilot_p  = nullptr;
}

X1Alpha::X1Alpha( const X1Alpha& obj ) :
    BaseEntity( obj ),
    legs_id( obj.legs_id ), legs( obj.legs ), legs_p( nullptr ),
    cockpit_id( obj.cockpit_id ), cockpit( obj.cockpit ), cockpit_p( nullptr ),
    weapon_id( obj.weapon_id ), weapon( obj.weapon ), weapon_p( nullptr ),
    beacon_light_id( obj.beacon_light_id ), beacon_light( obj.beacon_light ), beacon_light_p( nullptr ),
    pilot_id( obj.pilot_id ), pilot( obj.pilot ), pilot_p( nullptr ) {}

X1Alpha::~X1Alpha() {
    if( this->legs_p != nullptr )
        delete this->legs_p;

    if( this->cockpit_p != nullptr )
        delete this->cockpit_p;

    if( this->weapon_p != nullptr )
        delete this->weapon_p;

    if( this->beacon_light_p != nullptr )
        delete this->beacon_light_p;

    if( this->pilot_p != nullptr )
        delete this->pilot_p;
}

Actor* X1Alpha::duplicate( const Actor &original ) const {
    return new X1Alpha( *this );
}

void X1Alpha::resetGraphics( MainProgram &main_program ) {
    if( this->legs_p != nullptr )
        delete this->legs_p;
    this->legs_p = nullptr;

    if( this->cockpit_p != nullptr )
        delete this->cockpit_p;
    this->cockpit_p = nullptr;

    if( this->weapon_p != nullptr )
        delete this->weapon_p;
    this->weapon_p = nullptr;

    if( this->beacon_light_p != nullptr )
        delete this->beacon_light_p;
    this->beacon_light_p = nullptr;

    if( this->pilot_p != nullptr )
        delete this->pilot_p;
    this->pilot_p = nullptr;

    try {
        if( this->legs )
            this->legs_p = main_program.environment_p->allocateModel( this->legs_id, this->position, glm::quat(), this->texture_offset );
    }
    catch( const std::invalid_argument& argument ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "Cobj with resource id " << legs_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }
}

void X1Alpha::update( MainProgram &main_program, std::chrono::microseconds delta ) {}

}
