#ifndef FC_PARTICLE_VIEWER_H
#define FC_PARTICLE_VIEWER_H

#include "GameState.h"

#include <vector>
#include "Data/Mission/PYRResource.h"
#include "Graphics/Text2DBuffer.h"

#include "Graphics/ParticleInstance.h"

class ParticleViewer : public GameState {
public:
    static ParticleViewer particle_viewer;

private:
    unsigned particle_index;
    std::vector<Data::Mission::PYRResource::Particle> particles;
    Graphics::ParticleInstance *particle_instance_p;

    float next_frame_delay_seconds;
    float next_frame_delay;
    unsigned particle_frame_index;

    float count_down;

    Graphics::Text2DBuffer::Font font;
    unsigned font_height;

    glm::u32vec2 right_text_placement;

public:
    ParticleViewer();
    virtual ~ParticleViewer();

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

#endif // FC_PARTICLE_VIEWER_H
