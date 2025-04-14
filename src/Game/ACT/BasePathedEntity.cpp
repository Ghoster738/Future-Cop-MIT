#include "BasePathedEntity.h"

namespace Game::ACT {

void BasePathedEntity::setNextDestination() {
    unsigned int index_array[4];

    auto amount = this->node_r->getIndexes( index_array );

    if( amount != 0 ) {
        unsigned index = 0;

        if(amount != 1)
            index = this->random_generator.nextUnsignedInt() % amount;

        this->node_r = this->net_r->getNodePointer( index_array[index] );

        this->next_node_pos.x = this->node_r->getPosition().x;
        this->next_node_pos.y = this->node_r->getPosition().y + this->height_offset;
        this->next_node_pos.z = this->node_r->getPosition().z;

        glm::vec2 destination = glm::vec2(this->next_node_pos.x, this->next_node_pos.z) - glm::vec2(this->position.x, this->position.z);

        double distance = glm::distance(this->position, this->next_node_pos);

        if(distance >= 0.) {
            auto num = static_cast<std::chrono::microseconds::rep>(distance / this->movement_speed * 1000000);

            this->total_time_next_node = std::chrono::microseconds(num);
        }
        this->time_to_next_node = total_time_next_node;

        if(destination.x != 0.f || destination.y != 0.f) {
            destination = glm::normalize(destination);

            float angle = glm::atan(destination.y, destination.x);

            this->next_node_rot = glm::quat( glm::vec3(0, -angle + glm::pi<float>() / 2.f, 0) );
        }
    }
}

BasePathedEntity::BasePathedEntity( Utilities::Random &random, const Data::Accessor& accessor, const Data::Mission::ACT::BasePathedEntity& obj ) : BaseShooter( obj ), movement_speed( obj.getMovementSpeed() ), height_offset( obj.getHeightOffset() ), random_generator( random.getGenerator() ) {
    const Data::Mission::PTCResource &ptc = *accessor.getConstPTC( 1 );

    this->position = obj.getPosition( ptc, this->height_offset, Data::Mission::ACTResource::GroundCast::NONE );

    auto net_id = obj.getNetID();
    bool net    = obj.getHasNetID();

    this->net_r  = nullptr;
    this->node_r = nullptr;

    if( net )
        this->net_r  = accessor.getConstNET( net_id );

    this->total_time_next_node  = std::chrono::microseconds(1000000);

    if( this->net_r ) {
        auto index = this->net_r->getNodeIndexFromPosition(obj.getRawPosition());
        this->node_r = this->net_r->getNodePointer(index);

        if(this->node_r)
            this->position.y += this->node_r->getYAxis();

        this->next_node_pos = this->position;
        this->next_node_rot = glm::quat(1.f, 0.f, 0.f, 0.f);

        setNextDestination();
    }
}

BasePathedEntity::BasePathedEntity( const BasePathedEntity& obj ) :
    BaseShooter( obj ), movement_speed( obj.movement_speed ), height_offset( obj.height_offset ),
    net_r( obj.net_r ), node_r( obj.node_r ),
    time_to_next_node( obj.time_to_next_node ), total_time_next_node( obj.total_time_next_node ),
    random_generator( obj.random_generator ), next_node_rot( obj.next_node_rot ), next_node_pos( obj.next_node_pos ) {}

BasePathedEntity::~BasePathedEntity() {}

glm::vec3 BasePathedEntity::getCurrentPosition( std::chrono::microseconds delta ) {
    if(this->node_r == nullptr)
        return this->position;

    this->time_to_next_node -= delta;

    if(this->time_to_next_node.count() <= 0) {
        this->position = this->next_node_pos;

        setNextDestination();
    }

    return glm::mix(this->next_node_pos, this->position, static_cast<float>(this->time_to_next_node.count()) / static_cast<float>(this->total_time_next_node.count()));
}

}
