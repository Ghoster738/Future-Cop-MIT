#ifndef FC_GAME_ACT_BASE_PATHED_ENTITY_TURRET
#define FC_GAME_ACT_BASE_PATHED_ENTITY_TURRET

#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/BasePathedEntity.h"

#include "BaseShooter.h"

namespace Game::ACT {

class BasePathedEntity : public BaseShooter {
protected:
    float movement_speed;

    const Data::Mission::NetResource* net_r;
    const Data::Mission::NetResource::Node* node_r;

    std::chrono::microseconds    time_to_next_node;
    std::chrono::microseconds total_time_next_node;

    Utilities::Random::Generator random_generator;
    glm::quat next_node_rot;
    glm::vec3 next_node_pos;
    float height_offset;

public:
    BasePathedEntity( Utilities::Random &random, const Data::Mission::ACT::BasePathedEntity& obj ) : BaseShooter( obj ), movement_speed( obj.getMovementSpeed() ), random_generator( random.getGenerator() ) {}
    BasePathedEntity( const BasePathedEntity& obj ) : BaseShooter( obj ), movement_speed( obj.movement_speed ),
        net_r( obj.net_r ), node_r( obj.node_r ),
        time_to_next_node( obj.time_to_next_node ), total_time_next_node( obj.total_time_next_node ),
        random_generator( obj.random_generator ), next_node_rot( obj.next_node_rot ), next_node_pos( obj.next_node_pos ), height_offset( obj.height_offset ) {}
    virtual ~BasePathedEntity() {}
};

}

#endif // FC_GAME_ACT_BASE_PATHED_ENTITY_TURRET
