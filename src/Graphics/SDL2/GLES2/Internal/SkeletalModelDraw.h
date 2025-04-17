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
    public:
        struct TriangleIndex {
            uint_fast8_t vertices[3];
        };
        class Dynamic : public Mesh::DynamicNormal {
        public:
            SkeletalAnimation *skeletal_info_r;
            unsigned int current_frame;
            std::vector<glm::vec2> *uv_frame_buffer_r;
            glm::vec2 texture_offset;
            glm::vec3 color;
            std::vector<float> *star_timings_r;
            std::vector<Data::Mission::ObjResource::FacerPolygon> *facer_polygons_info_r;
            unsigned facer_triangles_amount;
            unsigned facer_polygons_stride;
            glm::vec3 camera_right, camera_up;

            virtual void addTriangles( const std::vector<DynamicTriangleDraw::Triangle> &triangles, DynamicTriangleDraw::DrawCommand &triangles_draw ) const;
        };

    protected:
        unsigned int num_bones;
        std::vector<glm::mat4> bone_frames;

    public:
        std::vector<TriangleIndex> triangle_weights;

        SkeletalAnimation( unsigned int num_bones, unsigned int amount_of_frames );

        glm::mat4* getFrames( unsigned int current_frame );

        unsigned int getNumBones() const { return num_bones; }
    };

    std::map<uint32_t, SkeletalAnimation*> model_animation_p;
public:
    SkeletalModelDraw();
    virtual ~SkeletalModelDraw();

    /**
     * This method gets the default vertex shader depending on the GL version.
     * @warning make sure the correct context is binded, or else you would get an improper shader.
     * @return a pointer to a vertex shader.
     */
    static const GLchar* getDefaultVertexShader();

    /**
     * This sets up and compiles this shader from memory.
     * This is to be used for internal shaders.
     * @param shader_source The memory pointer to the source code of the shader.
     */
    void setVertexShader( const GLchar *const shader_source = getDefaultVertexShader() ) { StaticModelDraw::setVertexShader( shader_source ); }

    /**
     * This sets up and compiles this shader from memory.
     * This is to be used for internal shaders.
     * @param shader_source The memory pointer to the source code of the shader.
     */
    void setFragmentShader( const GLchar *const shader_source = getDefaultFragmentShader() ) { StaticModelDraw::setFragmentShader( shader_source ); }

    /**
     * Link every shader to the program.
     * @return false if one of the shaders are not loaded.
     */
    int compileProgram();

    /**
     * This handles the loading of the models.
     * @param model_type_r Holds the model information.
     * @param obj The model builder's source. Holds metadata needed for rendering the model.
     * @param textures The accessor of the textures available for the models.
     * @return 1 for success, or -1 for failure.
     */
    int inputModel( Utilities::ModelBuilder *model_type_r, const Data::Mission::ObjResource& obj, const std::map<uint32_t, Internal::Texture2D*>& textures );

    void clearModels();

    /**
     * This draws all of the models with the morph attribute.
     * @note Make sure setFragmentShader, loadFragmentShader, compileProgram and setWorld in this order are called SUCCESSFULLY.
     * @param camera This is the camera data to be passed into world.
     */
    void draw( Graphics::SDL2::GLES2::Camera &camera );
    
    virtual void advanceTime( float seconds_passed );
};

}
}
}
}

#endif // GRAPHICS_GLES2_INTERNAL_SKELETAL_ANIMATION_MODEL_DRAW_H
