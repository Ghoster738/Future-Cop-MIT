#ifndef FC_GAME_ACT_BASE_ENTITY
#define FC_GAME_ACT_BASE_ENTITY

#include "../../Data/Mission/ACT/BaseEntity.h"

#include "Actor.h"

namespace Game {

namespace ACT {

class BaseEntity : public Actor {
protected:
    glm::vec2 texture_offset;

    Data::Mission::ACT::BaseEntity::Bitfield entity_bitfield;

public:
    BaseEntity( const Data::Mission::ACT::BaseEntity& obj ) : Actor( obj.getID() ), texture_offset( obj.getTextureOffset() ), entity_bitfield( obj.getEntityBitfield() ) {}
    BaseEntity( const BaseEntity& obj ) : Actor( obj ), texture_offset( obj.texture_offset ), entity_bitfield( obj.entity_bitfield ) {}
    virtual ~BaseEntity() {}
};

}

}

#endif // FC_GAME_ACT_BASE_ENTITY
