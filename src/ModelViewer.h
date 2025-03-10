#ifndef FC_MODEL_VIEWER_H
#define FC_MODEL_VIEWER_H

#include "GameState.h"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <filesystem>
#include <vector>

#include "Data/Mission/ObjResource.h"
#include "Graphics/ModelInstance.h"

class ModelViewer : public GameState {
public:
    static ModelViewer model_viewer;

private:
    unsigned int cobj_index;
    std::vector<const Data::Mission::ObjResource*> obj_vector;

    Graphics::ModelInstance* displayed_instance_p;
    glm::vec3 position;
    float radius;

    float rotation;
    float count_down;

    bool exported_textures;
    std::filesystem::path resource_export_path;

    Graphics::Text2DBuffer::Font font;
    unsigned font_height;

    glm::u32vec2 right_text_placement;

public:
    ModelViewer();
    virtual ~ModelViewer();

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

#endif // FC_MODEL_VIEWER_H
