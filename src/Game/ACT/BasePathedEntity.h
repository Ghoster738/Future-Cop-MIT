#ifndef FC_GAME_ACT_BASE_PATHED_ENTITY_TURRET
#define FC_GAME_ACT_BASE_PATHED_ENTITY_TURRET

#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/BasePathedEntity.h"
#include "../../Data/Mission/NetResource.h"

#include "BaseShooter.h"

namespace Game::ACT {

class BasePathedEntity : public BaseShooter {
protected:
    float movement_speed;
    float height_offset;

    const Data::Mission::NetResource* net_r;
    const Data::Mission::NetResource::Node* node_r;

    std::chrono::microseconds    time_to_next_node;
    std::chrono::microseconds total_time_next_node;

    Utilities::Random::Generator random_generator;
    glm::quat next_node_rot;
    glm::vec3 next_node_pos;

    void setNextDestination();

public:
    BasePathedEntity( Utilities::Random &random, const Data::Accessor& accessor, const Data::Mission::ACT::BasePathedEntity& obj );
    BasePathedEntity( const BasePathedEntity& obj );
    virtual ~BasePathedEntity();

    glm::vec3 getCurrentPosition( std::chrono::microseconds delta );
};

}

#endif // FC_GAME_ACT_BASE_PATHED_ENTITY_TURRET
