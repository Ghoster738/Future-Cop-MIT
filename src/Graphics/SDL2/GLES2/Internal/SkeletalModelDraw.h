#ifndef GRAPHICS_GLES2_INTERNAL_SKELETAL_ANIMATION_MODEL_DRAW_H
#define GRAPHICS_GLES2_INTERNAL_SKELETAL_ANIMATION_MODEL_DRAW_H

#include "StaticModelDraw.h"
#include "VertexAttributeArray.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {
namespace Internal {

class SkeletalModelDraw : public StaticModelDraw {
public:
    static const GLchar* default_vertex_shader;
protected:
    // This is the uniform used for the bone array.
    GLuint mat4_array_uniform_id;

    class SkeletalAnimation {
    private:
        unsigned int num_bones;
        std::vector<glm::mat4> bone_frames;
    public:
        SkeletalAnimation( unsigned int num_bones, unsigned int amount_of_frames );

        glm::mat4* getFrames( unsigned int current_frame, unsigned int starting_bone = 0 );

        unsigned int getNumBones() const { return num_bones; }
    };

    std::vector<SkeletalAnimation*> model_animation;
public:
    SkeletalModelDraw();
    virtual ~SkeletalModelDraw();

    /**
     * This sets up and compiles this shader from memory.
     * This is to be used for internal shaders.
     * @param shader_source The memory pointer to the source code of the shader.
     */
    void setVertexShader( const GLchar *const shader_source = default_vertex_shader ) { StaticModelDraw::setVertexShader( shader_source ); }

    /**
     * This sets up and compiles this shader from memory.
     * This is to be used for internal shaders.
     * @param shader_source The memory pointer to the source code of the shader.
     */
    void setFragmentShader( const GLchar *const shader_source = default_fragment_shader ) { StaticModelDraw::setFragmentShader( shader_source ); }

    /**
     * Link every shader to the program.
     * @return false if one of the shaders are not loaded.
     */
    int compilieProgram();

    /**
     * This sets the capacity of the model types contained within this class.
     * @param model_amount The amount of models to draw.
     * @return
     */
    void setNumModelTypes( size_t model_amount );

    /**
     * This handles the loading of the models.
     * @param These are the models to load.
     * @param This is the amount of models to load.
     * @return 1 for success, or -1 for failure.
     */
    int inputModel( Utilities::ModelBuilder *model_type, int index, const std::map<uint32_t, Internal::Texture2D*>& textures );

    /**
     * This draws all of the models with the morph attribute.
     * @note Make sure setFragmentShader, loadFragmentShader, compilieProgram and setWorld in this order are called SUCCESSFULLY.
     * @param This is the camera data to be passed into world.
     */
    void draw( const Camera &camera );
};

}
}
}
}

#endif // GRAPHICS_GLES2_INTERNAL_SKELETAL_ANIMATION_MODEL_DRAW_H
