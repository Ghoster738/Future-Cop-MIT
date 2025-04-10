#ifndef FC_GAME_ACT_BASE_TURRET_HEADER
#define FC_GAME_ACT_BASE_TURRET_HEADER

#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/Turret.h"

#include "BaseTurret.h"

namespace Game::ACT {

class Turret : public BaseTurret {
protected:
    glm::quat rest_gun_rotation, base_rotation;

    uint32_t alive_base_id; bool alive_base;
    uint32_t  dead_base_id; bool  dead_base;

    const Data::Mission::ObjResource *alive_base_cobj_r;
    const Data::Mission::ObjResource  *dead_base_cobj_r;

    Graphics::ModelInstance *base_p;

public:
    Turret( Utilities::Random &random, const Data::Accessor& accessor, const Data::Mission::ACT::Turret& obj );
    Turret( const Turret& obj );
    virtual ~Turret();

    virtual Actor* duplicate( const Actor &original ) const;

    virtual void resetGraphics( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

}

#endif // FC_GAME_ACT_BASE_TURRET_HEADER
