#include "SkyCaptain.h"

namespace Game::ACT {

SkyCaptain::SkyCaptain( const Data::Accessor& accessor, const Data::Mission::ACT::SkyCaptain& obj ) : Aircraft( accessor, obj ) {}

SkyCaptain::SkyCaptain( const SkyCaptain& obj ) : Aircraft( obj ) {}

SkyCaptain::~SkyCaptain() {}

Actor* SkyCaptain::duplicate( const Actor &original ) const {
    return new SkyCaptain( *this );
}

void SkyCaptain::update( MainProgram &main_program, std::chrono::microseconds delta ) {}

}
