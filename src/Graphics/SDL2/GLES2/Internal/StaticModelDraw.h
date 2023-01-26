#ifndef GRAPHICS_GLES2_INTERNAL_STATIC_MODEL_DRAW_H
#define GRAPHICS_GLES2_INTERNAL_STATIC_MODEL_DRAW_H

#include "Mesh.h"
#include "../../../../Data/Mission/ObjResource.h"
#include "../../../Camera.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

class ModelInstance;

namespace Internal {

class StaticModelDraw {
public:
    static const GLchar* default_vertex_shader;
    static const GLchar* default_fragment_shader;
    static const GLchar* default_es_vertex_shader;
    static const GLchar* default_es_fragment_shader;
    struct ModelArray {
        uint32_t obj_identifier; // The type of model the instances will represent.
        unsigned int unculled_size;
        std::vector<GLES2::ModelInstance*> instances; // The list of all instances that will be drawn.

    };
protected:
    Program program;
    Shader vertex_shader;
    Shader fragment_shader;
    GLuint diffusive_texture_uniform_id;
    GLuint sepecular_texture_uniform_id;
    GLuint matrix_uniform_id; // model * view * projection.
    GLuint texture_offset_uniform_id;
    
    // These are used for speculars
    GLuint view_uniform_id;
    GLuint view_inv_uniform_id;

    // The models need to be accessed.
    std::map<uint32_t, Mesh*> models;

    // The textures will also need to be accessed.
    Texture2D *shiney_texture_r; // This holds the environment map.

    // This stores the actual data.
    std::vector<ModelArray*> model_array;

    ModelArray* getModelArray( uint32_t obj_identifier );
    ModelArray* getModelArray( uint32_t obj_identifier ) const;
    ModelArray* addModelArray( uint32_t obj_identifier );
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
     * This sets the textures.
     * @param general_textures_ref This holds all the textures that the models could use.
     * @param shiney_texture_ref This stores the shiney texture.
     */
    void setTextures( Texture2D *shiney_texture_ref );

    /**
     * This checks for the existence of the model inside the Draw routine.
     * @note This is meant to tell Environment whether or not the model is contained within a *ModelDraw class.
     * @return True if the model exists in the class.
     */
    bool containsModel( uint32_t obj_identifier ) const;

    /**
     * This handles the loading of the models.
     * @param These are the models to load.
     * @param This is the amount of models to load.
     * @return 1 for success, or -1 for failure.
     */
    int inputModel( Utilities::ModelBuilder *model_type, uint32_t resource_cobj, const std::map<uint32_t, Internal::Texture2D*>& textures );

    /**
     * This draws all of the models.
     * @note Make sure setFragmentShader, loadFragmentShader, compilieProgram and setWorld in this order are called SUCCESSFULLY.
     * @param This is the camera data to be passed into world.
     */
    void draw( const Camera &camera );

    /**
     * This prunes the object models in the model\_array. It is an O( number\_of\_models\_loaded ) operation.
     */
    int prune();

    int allocateObjModel( uint32_t resource_cobj, GLES2::ModelInstance &model_instance );

    /**
     * This advances the time of every instance.
     * @param seconds_passed This is the time in seconds that were passed.
     */
    void advanceTime( float seconds_passed );

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
    GLuint getSepecularTextureUniformID() const { return sepecular_texture_uniform_id; }

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
