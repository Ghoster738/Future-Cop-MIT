#ifndef FC_GAME_ACT_DCS_QUAD_HEADER
#define FC_GAME_ACT_DCS_QUAD_HEADER

#include "../../Graphics/QuadInstance.h"

#include "../../Data/Mission/ACT/DCSQuad.h"

#include "Actor.h"

namespace Game::ACT {

class DCSQuad : public Actor {
private:
    glm::vec4 color;
    glm::quat rotation;
    glm::vec2 scale;

    uint32_t dcs_id;
    Graphics::QuadInstance *dcs_p;

public:
    DCSQuad( Utilities::Random &random, const Data::Accessor& accessor, const Data::Mission::ACT::DCSQuad& obj );
    DCSQuad( const DCSQuad& obj );
    virtual ~DCSQuad();

    virtual Actor* duplicate( const Actor &original ) const;

    virtual void resetGraphics( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

}

#endif // FC_GAME_ACT_DCS_QUAD_HEADER
