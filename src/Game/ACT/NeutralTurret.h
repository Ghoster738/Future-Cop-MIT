#ifndef FC_GAME_ACT_NEUTRAL_TURRET_HEADER
#define FC_GAME_ACT_NEUTRAL_TURRET_HEADER

#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/NeutralTurret.h"
#include "../../Data/Mission/ObjResource.h"
#include "../../Data/Mission/IFF.h"

#include "Actor.h"

namespace Game {

namespace ACT {

class NeutralTurret : Actor {
private:
    glm::quat rest_gun_rotation, gun_rotation, base_rotation;
    glm::vec2 texture_offset;

    uint32_t alive_gun_id;  bool alive_gun;
    uint32_t alive_base_id; bool alive_base;
    uint32_t dead_gun_id;   bool dead_gun;
    uint32_t dead_base_id;  bool dead_base;

    const Data::Mission::ObjResource  *alive_gun_cobj_r;
    const Data::Mission::ObjResource *alive_base_cobj_r;
    const Data::Mission::ObjResource   *dead_gun_cobj_r;
    const Data::Mission::ObjResource  *dead_base_cobj_r;

    Graphics::ModelInstance *base_p, *gun_p;

public:
    NeutralTurret(const Data::Accessor& accessor, const Data::Mission::ACT::NeutralTurret& obj );
    NeutralTurret( const NeutralTurret& obj );
    virtual ~NeutralTurret();

    virtual Actor* duplicate( const Actor &original ) const;

    virtual void resetGraphics( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

}

}

#endif // FC_GAME_ACT_NEUTRAL_TURRET_HEADER
