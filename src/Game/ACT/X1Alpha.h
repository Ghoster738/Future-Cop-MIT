#ifndef FC_GAME_ACT_X1_ALPHA
#define FC_GAME_ACT_X1_ALPHA

#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/X1Alpha.h"

#include "BaseEntity.h"

namespace Game::ACT {

class X1Alpha : public BaseEntity {
private:
    uint32_t legs_id; bool legs;
    Graphics::ModelInstance *legs_p;

    uint32_t cockpit_id; bool cockpit;
    Graphics::ModelInstance *cockpit_p;

    uint32_t weapon_id; bool weapon;
    Graphics::ModelInstance *weapon_p;

    uint32_t beacon_light_id; bool beacon_light;
    Graphics::ModelInstance *beacon_light_p;

    uint32_t pilot_id; bool pilot;
    Graphics::ModelInstance *pilot_p;

public:
    X1Alpha( const Data::Accessor& accessor, const Data::Mission::ACT::X1Alpha& obj );
    X1Alpha( const X1Alpha& obj );
    virtual ~X1Alpha();

    virtual Actor* duplicate( const Actor &original ) const;

    virtual void resetGraphics( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

}

#endif // FC_GAME_ACT_X1_ALPHA
