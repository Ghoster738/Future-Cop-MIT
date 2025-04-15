#ifndef FC_GAME_ACT_BASE_ENTITY
#define FC_GAME_ACT_BASE_ENTITY

#include "../../Data/Mission/ACT/BaseEntity.h"

#include "Actor.h"

namespace Game {

namespace ACT {

class BaseEntity : public Actor {
protected:
    glm::vec2 texture_offset;

    bool disable_rendering;

public:
    BaseEntity( const Data::Mission::ACT::BaseEntity& obj ) : Actor( obj.getID() ), texture_offset( obj.getTextureOffset() ), disable_rendering( obj.disableRendering() ) {}
    BaseEntity( const BaseEntity& obj ) : Actor( obj ), texture_offset( obj.texture_offset ), disable_rendering( obj.disable_rendering ) {}
    virtual ~BaseEntity() {}
};

}

}

#endif // FC_GAME_ACT_BASE_ENTITY
