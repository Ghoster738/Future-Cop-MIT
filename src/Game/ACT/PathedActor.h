#ifndef FC_GAME_ACT_BASE_PATHED_ACTOR_HEADER
#define FC_GAME_ACT_BASE_PATHED_ACTOR_HEADER

#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/PathedActor.h"

#include "../../Data/Mission/NetResource.h"

#include "BasePathedEntity.h"

namespace Game::ACT {

class PathedActor : public BasePathedEntity {
protected:
    uint32_t alive_id; bool alive_base;
    uint32_t  dead_id; bool  dead_base;

    const Data::Mission::ObjResource* alive_cobj_r;
    const Data::Mission::ObjResource*  dead_cobj_r;
    const Data::Mission::NetResource*        net_r;

    const Data::Mission::NetResource::Node *node_r;

    Graphics::ModelInstance *alive_p;

    std::chrono::microseconds    time_to_next_node;
    std::chrono::microseconds total_time_next_node;
    Utilities::Random::Generator random_generator;
    glm::quat next_node_rot;
    glm::vec3 next_node_pos;
    float height_offset;

    void setNextDestination();

public:
    PathedActor( Utilities::Random &random, const Data::Accessor& accessor, const Data::Mission::ACT::PathedActor& obj );
    PathedActor( const PathedActor& obj );
    virtual ~PathedActor();

    virtual Actor* duplicate( const Actor &original ) const;

    virtual void resetGraphics( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

}

#endif // FC_GAME_ACT_BASE_PATHED_ACTOR_HEADER
