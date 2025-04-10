#include "PathedActor.h"

#include <iostream>

namespace Game::ACT {

PathedActor::PathedActor( Utilities::Random &random, const Data::Accessor& accessor, const Data::Mission::ACT::PathedActor& obj ) : BasePathedEntity( obj ), random_generator( random.getGenerator() ) {
    const Data::Mission::PTCResource &ptc = *accessor.getConstPTC( 1 );

    this->position = obj.getPosition( ptc, obj.getHeightOffset(), Data::Mission::ACTResource::GroundCast::HIGH );

    this->alive_id = obj.getAliveID();
    this->alive_base = obj.getHasAliveID();

    this->dead_id = obj.getDestroyedID();
    this->dead_base = obj.getHasDestroyedID();

    auto net_id = obj.getNetID();
    bool net    = obj.getHasNetID();

    this->alive_cobj_r = nullptr;
    this->dead_cobj_r  = nullptr;
    this->net_r        = nullptr;
    this->node_r       = nullptr;

    if( this->alive_base )
        this->alive_cobj_r = accessor.getConstOBJ( this->alive_id );

    if( this->dead_base )
        this->dead_cobj_r = accessor.getConstOBJ( this->dead_id );

    if( net )
        this->net_r  = accessor.getConstNET( net_id );

    this->total_time_next_node  = std::chrono::microseconds(1000000);

    if( this->net_r ) {
        auto index = this->net_r->getNodeIndexFromPosition(obj.getRawPosition());

        this->node_r = this->net_r->getNodePointer(index);

        this->time_to_next_node = total_time_next_node;

        this->next_node_pos = this->position;

        unsigned int index_array[4];

        auto amount = this->node_r->getIndexes( index_array );

        if( amount != 0 ) {
            unsigned index = 0;

            if(amount != 1)
                index = this->random_generator.nextUnsignedInt() % amount;

            this->node_r = this->net_r->getNodePointer( index_array[index] );

            this->next_node_pos.x = (1.f / 32.f) * this->node_r->getPosition().x;
            this->next_node_pos.z = (1.f / 32.f) * this->node_r->getPosition().y;
        }
    }

    this->alive_p = nullptr;
}

PathedActor::PathedActor( const PathedActor& obj ) :
    BasePathedEntity( obj ),
    alive_id( obj.alive_id ), alive_base( obj.alive_base ),
    dead_id( obj.dead_id ), dead_base( obj.dead_base ),
    alive_cobj_r( obj.alive_cobj_r ), dead_cobj_r( obj.dead_cobj_r ),
    net_r( obj.net_r ), node_r( obj.node_r ),
    alive_p( nullptr ),
    time_to_next_node( obj.time_to_next_node ), total_time_next_node( obj.total_time_next_node ), next_node_pos( obj.next_node_pos ), random_generator( obj.random_generator ) {}

PathedActor::~PathedActor() {
    if( this->alive_p != nullptr )
        delete this->alive_p;
}

Actor* PathedActor::duplicate( const Actor &original ) const {
    return new PathedActor( *this );
}

void PathedActor::resetGraphics( MainProgram &main_program ) {
    if( this->alive_p != nullptr )
        delete this->alive_p;
    this->alive_p = nullptr;

    try {
        if( this->alive_base ) {
            this->alive_p = main_program.environment_p->allocateModel( this->alive_id );

            if(this->alive_p) {
                this->alive_p->setPosition( this->position );
                this->alive_p->setTextureOffset( this->texture_offset );
                this->alive_p->setVisable( true );
            }
        }
    }
    catch( const std::invalid_argument& argument ) {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "Cobj with resource id " << alive_id << " does not exist. This could be an error from the map " << main_program.resource_identifier << "\n";
    }
}

void PathedActor::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    if(this->alive_p) {
        this->alive_p->setPositionTransformTimeline( this->alive_p->getPositionTransformTimeline() + std::chrono::duration<float>( delta ).count() * 10.f);
    }

    this->time_to_next_node -= delta;

    if(this->node_r) {
        if(this->time_to_next_node.count() <= 0) {
            this->time_to_next_node = this->total_time_next_node;

            unsigned int index_array[4];

            auto amount = this->node_r->getIndexes( index_array );

            if( amount != 0 ) {
                unsigned index = 0;

                if(amount != 1)
                    index = this->random_generator.nextUnsignedInt() % amount;

                this->node_r = this->net_r->getNodePointer( index_array[index] );

                this->position.x = this->next_node_pos.x;
                this->position.z = this->next_node_pos.z;

                this->next_node_pos.x = (1.f / 32.f) * this->node_r->getPosition().x;
                this->next_node_pos.z = (1.f / 32.f) * this->node_r->getPosition().y;
            }
        }

        if(this->alive_p) {
            glm::vec3 new_position = glm::mix(this->next_node_pos, this->position, static_cast<float>(this->time_to_next_node.count()) / static_cast<float>(this->total_time_next_node.count()));

            this->alive_p->setPosition( new_position );
        }
    }
}

}
