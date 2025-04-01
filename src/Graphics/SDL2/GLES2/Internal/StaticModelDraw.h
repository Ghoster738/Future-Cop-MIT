#ifndef GRAPHICS_GLES2_INTERNAL_STATIC_MODEL_DRAW_H
#define GRAPHICS_GLES2_INTERNAL_STATIC_MODEL_DRAW_H

#include "Mesh.h"
#include "../../../../Data/Mission/ObjResource.h"
#include "../Camera.h"
#include <set>

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

class ModelInstance;

namespace Internal {

class StaticModelDraw {
public:
    static const GLchar* default_vertex_shader;
    static const GLchar* default_fragment_shader;

    static const size_t UV_FRAME_BUFFER_SIZE_LIMIT;

    struct ModelArray {
        ModelArray( Program *program ) : mesh( program ) {}
        
        Mesh mesh;
        std::vector<float> star_timing_speed;
        std::vector<glm::u8vec2> uv_animation_data;
        std::vector<Data::Mission::ObjResource::FaceOverrideType>   uv_animation_info;
        std::vector<Data::Mission::ObjResource::FacerPolygon>     facer_polygons_info;
        std::vector<DynamicTriangleDraw::Triangle>              transparent_triangles;
        std::set<GLES2::ModelInstance*> instances_r; // The list of all instances that will be drawn.
        unsigned facer_triangles_amount;
        unsigned facer_polygons_stride;

        void bindUVAnimation(GLuint animated_uv_frames_id, unsigned int time, std::vector<glm::vec2>& uv_frame_buffer) const;
    };
    class Dynamic : public Mesh::DynamicNormal {
    public:
        glm::vec2 texture_offset;
        glm::vec3 color;
        std::vector<glm::vec2> *uv_frame_buffer_r;
        std::vector<float> *star_timings_r;
        std::vector<Data::Mission::ObjResource::FacerPolygon> *facer_polygons_info_r;
        unsigned facer_triangles_amount;
        unsigned facer_polygons_stride;
        glm::vec3 camera_right, camera_up;

        virtual void addTriangles( const std::vector<DynamicTriangleDraw::Triangle> &triangles, DynamicTriangleDraw::DrawCommand &triangles_draw ) const;
    };

protected:
    Program program;
    std::vector<Shader::Attribute> attributes;
    std::vector<Shader::Varying>   varyings;
    Shader vertex_shader;
    Shader fragment_shader;
    GLuint diffusive_texture_uniform_id;
    GLuint specular_texture_uniform_id;
    GLuint matrix_uniform_id; // model * view * projection.
    GLuint model_color_uniform_id;
    GLuint texture_offset_uniform_id;
    GLuint animated_uv_frames_id;
    
    // These are used for speculars
    GLuint view_uniform_id;
    GLuint view_inv_uniform_id;

    std::map<uint32_t, ModelArray*> models_p;
    std::map<uint32_t, ModelArray*> bounding_boxes_p;

    std::vector<glm::vec2> uv_frame_buffer;

    // The textures will also need to be accessed.
    Texture2D *shiney_texture_r; // This holds the environment map.

private:
    void draw( Graphics::SDL2::GLES2::Camera &camera, std::map<uint32_t, ModelArray*> &model_array_p );
    
public:
    StaticModelDraw();
    virtual ~StaticModelDraw();

    /**
     * This method gets the default vertex shader depending on the GL version.
     * @warning make sure the correct context is binded, or else you would get an improper shader.
     * @return a pointer to a vertex shader.
     */
    static const GLchar* getDefaultVertexShader();

    /**
     * This method gets the default fragment shader depending on the GL version.
     * @warning make sure the correct context is binded, or else you would get an improper shader.
     * @return a pointer to a fragment shader.
     */
    static const GLchar* getDefaultFragmentShader();

    /**
     * This sets up and compiles this shader from memory.
     * This is to be used for internal shaders.
     * @param shader_source The memory pointer to the source code of the shader.
     */
    void setVertexShader( const GLchar *const shader_source = getDefaultVertexShader() );

    /**
     * This loads a shader from a text file, and compiles it.
     * @param shader_source The memory pointer to the source code of the shader.
     */
    int loadVertexShader( const char *const file_path );


    /**
     * This sets up and compiles this shader from memory.
     * This is to be used for internal shaders.
     * @param shader_source The memory pointer to the source code of the shader.
     */
    void setFragmentShader( const GLchar *const shader_source = getDefaultFragmentShader() );

    /**
     * This loads a shader from a text file, and compiles it.
     * @param shader_source The memory pointer to the source code of the shader.
     */
    int loadFragmentShader( const char *const file_path );

    /**
     * Link every shader to the program.
     * @return false if one of the shaders are not loaded.
     */
    int compileProgram();

    /**
     * This sets the environement texture.
     * @param env_texture_ref This stores the shiney texture.
     */
    void setEnvironmentTexture( Texture2D *env_texture_ref );

    /**
     * This checks for the existence of the model inside the draw routine.
     * @note This is meant to tell Environment whether or not the model is contained within a *ModelDraw class.
     * @return True if the model exists in the class.
     */
    bool containsModel( uint32_t obj_identifier ) const;

    /**
     * This checks for the existence of the specific bounding boxes inside the draw routine.
     * @note This is meant to tell Environment whether or not the model is contained within a *ModelDraw class.
     * @return True if the model exists in the class.
     */
    bool containsBBModel( uint32_t obj_identifier ) const;

    /**
     * This handles the loading of the models.
     * @param model_type_r Holds the model information.
     * @param obj The model builder's source. Holds metadata needed for rendering the model.
     * @param textures The accessor of the textures available for the models.
     * @return 1 for success, or -1 for failure.
     */
    int inputModel( Utilities::ModelBuilder *model_type_r, const Data::Mission::ObjResource& obj, const std::map<uint32_t, Internal::Texture2D*>& textures );

    /**
     * This handles the loading of the models.
     * @param model_type_r Holds the model information.
     * @param resource_cobj The id associated with the model.
     * @param textures The accessor of the textures available for the models. Only nothing texture is used.
     * @return 1 for success, or -1 for failure.
     */
    int inputBoundingBoxes( Utilities::ModelBuilder *model_type_r, uint32_t resource_cobj, const std::map<uint32_t, Internal::Texture2D*>& textures );

    void clearModels();

    /**
     * This draws all the models that are opaque.
     * @note Make sure setFragmentShader, loadFragmentShader, compileProgram and setWorld in this order are called SUCCESSFULLY.
     * @param camera This is the camera data to be passed into world.
     */
    void draw( Graphics::SDL2::GLES2::Camera &camera ) { draw( camera, models_p ); }

    /**
     * This draws all the bounding boxes.
     * @note Make sure setFragmentShader, loadFragmentShader, compileProgram and setWorld in this order are called SUCCESSFULLY.
     * @param camera This is the camera data to be passed into world.
     */
    void drawBoundingBoxes( Graphics::SDL2::GLES2::Camera &camera ) { draw( camera, bounding_boxes_p ); }

    int allocateObjModel( uint32_t resource_cobj, GLES2::ModelInstance &model_instance );

    int allocateObjBBModel( uint32_t resource_cobj, GLES2::ModelInstance &model_instance );

    /**
     * This advances the time of every instance.
     * @note Timing is less important for animation data.
     * @param seconds_passed This is the time in seconds that were passed since the last frame.
     */
    virtual void advanceTime( float seconds_passed );

    /**
     * @return the program that this World uses.
     */
    Program* getProgram() { return &program; }

    /**
     * @return The OpenGL id of the texture uniform from the static model program.
     */
    GLuint getDiffusiveTextureUniformID() const { return diffusive_texture_uniform_id; }

    /**
     * @return The OpenGL id of the texture uniform from the static model program.
     */
    GLuint getSpecularTextureUniformID() const { return specular_texture_uniform_id; }

    /**
     * @return The OpenGL id of the matrix uniform from the static model program.
     */
    GLuint setCameraUniformID() const { return matrix_uniform_id; }
    
    /**
     * This method makes a bounding sphere for the entire mesh.
     * @param position This holds the sphere's center location.
     * @param radius This holds the rotation of the sphere.
     * @return true if a bounding sphere is generated.
     */
    bool getBoundingSphere( uint32_t obj_identifier, glm::vec3 &position, float &radius ) const;
};

}

}

}

}

#endif // GRAPHICS_GLES2_INTERNAL_STATIC_MODEL_DRAW_H
