#ifndef FC_MODEL_VIEWER_H
#define FC_MODEL_VIEWER_H

#include "GameState.h"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include "Data/Mission/ObjResource.h"
#include "Graphics/ModelInstance.h"

class ModelViewer : public GameState {
private:
    unsigned int cobj_index;
    std::vector<Data::Mission::ObjResource*> obj_vector;

    Graphics::ModelInstance* displayed_instance_p;
    glm::vec3 position;
    float radius;

    float rotation;
    float count_down;

    bool exported_textures;
    std::string resource_export_path;

public:
    ModelViewer();
    virtual ~ModelViewer();

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program );

    virtual void grabControls( MainProgram &main_program, std::chrono::microseconds delta );
    virtual void display( MainProgram &main_program );
};

#endif // FC_MODEL_VIEWER_H
