#include "X1Alpha.h"

#include "../../Data/Mission/ObjResource.h"

namespace Game::ACT {

X1Alpha::X1Alpha( const Data::Accessor& accessor, const Data::Mission::ACT::X1Alpha& obj ) : BaseEntity( obj ) {
    const Data::Mission::PTCResource &ptc = *accessor.getConstPTC( 1 );

    // TODO This is a wild guess.
    this->position = obj.getPosition( ptc, static_cast<Data::Mission::ACTResource::GroundCast>(obj.internal.uint8_0) );

    this->rotation = obj.getRotationQuaternion();

    if( obj.spawnInHoverMode() ) {
        this->cockpit_frame_index =  29;
        this->legs_frame_index    = 359;
    }
    else {
        this->cockpit_frame_index = 0;
        this->legs_frame_index    = 0;
    }

    this->legs        = obj.getHasLegID();
    this->legs_id     = obj.getLegID();
    this->legs_p      = nullptr;
    this->legs_cobj_r = nullptr;

    if(this->legs)
        this->legs_cobj_r = accessor.getConstOBJ( this->legs_id );

    this->cockpit        = obj.getHasCockpitID();
    this->cockpit_id     = obj.getCockpitID();
    this->cockpit_p      = nullptr;
    this->cockpit_cobj_r = nullptr;

    if(this->cockpit)
        this->cockpit_cobj_r = accessor.getConstOBJ( this->cockpit_id );

    this->weapon        = obj.getHasWeaponID();
    this->weapon_id     = obj.getWeaponID();
    this->weapons_p[0]  = nullptr;
    this->weapons_p[1]  = nullptr;
    this->weapon_cobj_r = nullptr;

    if(this->weapon)
        this->weapon_cobj_r = accessor.getConstOBJ( this->weapon_id );

    this->beacon_lights        = obj.getHasBeaconLightsID();
    this->beacon_lights_id     = obj.getBeaconLightsID();
    this->beacon_lights_p      = nullptr;
    this->beacon_lights_cobj_r = nullptr;

    if(this->beacon_lights)
        this->beacon_lights_cobj_r = accessor.getConstOBJ( this->beacon_lights_id );

    this->pilot        = obj.getHasPilotID();
    this->pilot_id     = obj.getPilotID();
    this->pilot_p      = nullptr;
    this->pilot_cobj_r = nullptr;

    if(this->pilot)
        this->pilot_cobj_r = accessor.getConstOBJ( this->pilot_id );
}

X1Alpha::X1Alpha( const X1Alpha& obj ) :
    BaseEntity( obj ),
    rotation( obj.rotation ), cockpit_frame_index( obj.cockpit_frame_index ), legs_frame_index( obj.legs_frame_index ),
    legs_id( obj.legs_id ), legs( obj.legs ), legs_p( nullptr ), legs_cobj_r( obj.legs_cobj_r ),
    cockpit_id( obj.cockpit_id ), cockpit( obj.cockpit ), cockpit_p( nullptr ), cockpit_cobj_r( obj.cockpit_cobj_r ),
    weapon_id( obj.weapon_id ), weapon( obj.weapon ), weapons_p{ nullptr, nullptr }, weapon_cobj_r( obj.weapon_cobj_r ),
    beacon_lights_id( obj.beacon_lights_id ), beacon_lights( obj.beacon_lights ), beacon_lights_p( nullptr ), beacon_lights_cobj_r( obj.beacon_lights_cobj_r ),
    pilot_id( obj.pilot_id ), pilot( obj.pilot ), pilot_p( nullptr ), pilot_cobj_r( obj.pilot_cobj_r ) {}

X1Alpha::~X1Alpha() {
    if( this->legs_p != nullptr )
        delete this->legs_p;

    if( this->cockpit_p != nullptr )
        delete this->cockpit_p;

    if( this->weapons_p[0] != nullptr )
        delete this->weapons_p[0];

    if( this->weapons_p[1] != nullptr )
        delete this->weapons_p[1];

    if( this->beacon_lights_p != nullptr )
        delete this->beacon_lights_p;

    if( this->pilot_p != nullptr )
        delete this->pilot_p;
}

Actor* X1Alpha::duplicate( const Actor &original ) const {
    return new X1Alpha( *this );
}

void X1Alpha::resetGraphics( MainProgram &main_program ) {
    auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );

    if( this->legs_p != nullptr )
        delete this->legs_p;
    this->legs_p = nullptr;

    try {
        if( this->legs ) {
            this->legs_p = main_program.environment_p->allocateModel( this->legs_id, this->position, this->rotation, this->texture_offset );

            if(this->legs_p) {
                this->legs_p->setPositionTransformTimeline(this->legs_frame_index);
            }
        }
    }
    catch( const std::invalid_argument& argument ) {
        log.output << "Cobj with resource id " << this->legs_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }

    if( this->cockpit_p != nullptr )
        delete this->cockpit_p;
    this->cockpit_p = nullptr;

    glm::vec3 cockpit_pos(0, 0, 0);

    try {
        if( this->cockpit ) {

            if(cockpit_cobj_r) {
                cockpit_pos = this->rotation * glm::vec4( legs_cobj_r->getPosition(0, this->legs_frame_index), 1 );
            }

            this->cockpit_p = main_program.environment_p->allocateModel( this->cockpit_id, this->position + cockpit_pos, this->rotation, this->texture_offset );

            if(this->cockpit_p) {
                this->cockpit_p->setPositionTransformTimeline(cockpit_frame_index);
            }
        }
    }
    catch( const std::invalid_argument& argument ) {
        log.output << "Cobj with resource id " << this->cockpit_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }

    for(unsigned i = 0; i < 2; i++) {
        if( this->weapons_p[i] != nullptr )
            delete this->weapons_p[i];
        this->weapons_p[i] = nullptr;

        try {
            if( this->weapon ) {
                Data::Mission::ObjResource::DecodedBone weapon_bone;

                if(cockpit_cobj_r) {
                    weapon_bone = cockpit_cobj_r->getBone( 2 * i + 2, cockpit_frame_index );
                }

                this->weapons_p[i] = main_program.environment_p->allocateModel( this->weapon_id, this->position + this->rotation * weapon_bone.position + cockpit_pos, this->rotation * weapon_bone.rotation, this->texture_offset );
            }
        }
        catch( const std::invalid_argument& argument ) {
            log.output << "Cobj with resource id " << this->weapon_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
        }
    }

    if( this->beacon_lights_p != nullptr )
        delete this->beacon_lights_p;
    this->beacon_lights_p = nullptr;

    try {
        if( this->beacon_lights ) {
            glm::vec3 beacon_pos(0, 0, 0);

            if(cockpit_cobj_r) {
                beacon_pos = this->rotation * cockpit_cobj_r->getPosition(2, cockpit_frame_index);
            }

            this->beacon_lights_p = main_program.environment_p->allocateModel( this->beacon_lights_id, this->position + beacon_pos + cockpit_pos, this->rotation );
        }
    }
    catch( const std::invalid_argument& argument ) {
        log.output << "Cobj with resource id " << this->beacon_lights_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }

    if( this->pilot_p != nullptr )
        delete this->pilot_p;
    this->pilot_p = nullptr;
}

void X1Alpha::update( MainProgram &main_program, std::chrono::microseconds delta ) {}

}
