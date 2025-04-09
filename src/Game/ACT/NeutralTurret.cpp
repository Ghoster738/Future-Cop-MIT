#include "NeutralTurret.h"

namespace Game::ACT {

NeutralTurret::NeutralTurret( const Data::Accessor& accessor, const Data::Mission::ACT::NeutralTurret& obj ) : Turret( accessor, obj ) {}

NeutralTurret::NeutralTurret( const NeutralTurret& obj ) : Turret( obj ) {}

NeutralTurret::~NeutralTurret() {}

Actor* NeutralTurret::duplicate( const Actor &original ) const {
    return new NeutralTurret( *this );
}

void NeutralTurret::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    Turret::update(main_program, delta);
}

}
