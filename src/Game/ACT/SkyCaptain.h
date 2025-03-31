#ifndef FC_GAME_ACT_SKY_CAPTAIN
#define FC_GAME_ACT_SKY_CAPTAIN
#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/SkyCaptain.h"
#include "../../Data/Mission/ObjResource.h"
#include "../../Data/Mission/IFF.h"

#include "Actor.h"

namespace Game {

namespace ACT {

class SkyCaptain : public Actor {
private:
    glm::vec2 texture_offset;

    uint32_t model_id; bool model;
    Graphics::ModelInstance *model_p;

public:
    SkyCaptain( const Data::Accessor& accessor, const Data::Mission::ACT::SkyCaptain& obj );
    SkyCaptain( const SkyCaptain& obj );
    virtual ~SkyCaptain();

    virtual Actor* duplicate( const Actor &original ) const;

    virtual void resetGraphics( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

}

}

#endif // FC_GAME_ACT_SKY_CAPTAIN
