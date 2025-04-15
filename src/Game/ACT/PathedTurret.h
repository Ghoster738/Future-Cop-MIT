#ifndef FC_GAME_ACT_BASE_PATHED_TURRET_HEADER
#define FC_GAME_ACT_BASE_PATHED_TURRET_HEADER

#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/PathedTurret.h"

#include "BasePathedEntity.h"

namespace Game::ACT {

class PathedTurret : public BasePathedEntity {
protected:
    uint32_t alive_id; bool alive_base;
    uint32_t  dead_id; bool  dead_base;
    uint32_t   gun_id; bool   gun_base;

    const Data::Mission::ObjResource* alive_cobj_r;
    const Data::Mission::ObjResource*  dead_cobj_r;
    const Data::Mission::ObjResource*   gun_cobj_r;

    Graphics::ModelInstance* alive_p;
    Graphics::ModelInstance*   gun_p;

    unsigned gun_parent_index;

public:
    PathedTurret( Utilities::Random &random, const Data::Accessor& accessor, const Data::Mission::ACT::PathedTurret& obj );
    PathedTurret( const PathedTurret& obj );
    virtual ~PathedTurret();

    virtual Actor* duplicate( const Actor &original ) const;

    virtual void resetGraphics( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

}

#endif // FC_GAME_ACT_BASE_PATHED_TURRET_HEADER
