#ifndef FC_GAME_ACT_ITEM_PROP_HEADER
#define FC_GAME_ACT_ITEM_PROP_HEADER

#include "../../Graphics/ModelInstance.h"

#include "../../Data/Mission/ACT/Prop.h"
#include "../../Data/Mission/ObjResource.h"
#include "../../Data/Mission/IFF.h"

#include "Actor.h"

namespace Game {

namespace ACT {

class Prop : public Actor {
private:
    glm::quat rotation;

    uint32_t model_id;
    Graphics::ModelInstance *model_p;

    glm::quat rotation_points[3];
    float rotation_speed_factor;
    float rotation_time_line;
    unsigned rotation_time_line_length;
    bool has_animated_rotation;

public:
    Prop( const Data::Accessor& accessor, const Data::Mission::ACT::Prop& obj );
    Prop( const Prop& obj );
    virtual ~Prop();

    virtual Actor* duplicate( const Actor &original ) const;

    virtual void resetGraphics( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

}

}

#endif // FC_GAME_ACT_ITEM_PROP_HEADER
