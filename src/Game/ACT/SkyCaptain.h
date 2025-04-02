#ifndef FC_GAME_ACT_SKY_CAPTAIN
#define FC_GAME_ACT_SKY_CAPTAIN
#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/SkyCaptain.h"
#include "../../Data/Mission/ObjResource.h"
#include "../../Data/Mission/IFF.h"

#include "Aircraft.h"

namespace Game::ACT {

class SkyCaptain : public Aircraft {
public:
    SkyCaptain( const Data::Accessor& accessor, const Data::Mission::ACT::SkyCaptain& obj );
    SkyCaptain( const SkyCaptain& obj );
    virtual ~SkyCaptain();

    virtual Actor* duplicate( const Actor &original ) const;

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

}

#endif // FC_GAME_ACT_SKY_CAPTAIN
