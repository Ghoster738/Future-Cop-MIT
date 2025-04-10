#ifndef FC_GAME_ACT_NEUTRAL_TURRET_HEADER
#define FC_GAME_ACT_NEUTRAL_TURRET_HEADER

#include "../../Data/Mission/ACT/NeutralTurret.h"

#include "Turret.h"

namespace Game::ACT {

class NeutralTurret : public Turret {
public:
    NeutralTurret( Utilities::Random &random, const Data::Accessor& accessor, const Data::Mission::ACT::NeutralTurret& obj );
    NeutralTurret( const NeutralTurret& obj );
    virtual ~NeutralTurret();

    virtual Actor* duplicate( const Actor &original ) const;

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

}

#endif // FC_GAME_ACT_NEUTRAL_TURRET_HEADER
