#ifndef GRAPHICS_GLES2_INTERNAL_MORPH_MODEL_DRAW_H
#define GRAPHICS_GLES2_INTERNAL_MORPH_MODEL_DRAW_H

#include "StaticModelDraw.h"
#include "VertexAttributeArray.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {
namespace Internal {

class MorphModelDraw : public StaticModelDraw {
public:
    static const GLchar* default_vertex_shader;
    
    class Animation {
    public:
        
        struct DeltaTriangle {
            glm::vec3 vertices[3];
        };
        
        class Dynamic : public Mesh::DynamicNormal {
        public:
            Animation *morph_info_r;
            unsigned int frame_index;
            std::vector<glm::vec2> *uv_frame_buffer_r;
            glm::vec2 texture_offset;
            std::vector<float> *star_timings_r;
            std::vector<Data::Mission::ObjResource::FacerPolygon> *facer_polygons_info_r;
            unsigned facer_triangles_amount;
            unsigned facer_polygons_stride;
            glm::vec3 camera_right, camera_up;

            virtual void addTriangles( const std::vector<DynamicTriangleDraw::Triangle> &triangles, DynamicTriangleDraw::DrawCommand &triangles_draw ) const;
        };
    protected:
        
        unsigned triangles_per_frame;
        std::vector<DeltaTriangle> frame_data; // [Delta Frame 1], [Delta Frame 2], ..., [Delta Frame end]
    public:

        Animation( Utilities::ModelBuilder *model_type_r, GLsizei transparent_count );
        
        const DeltaTriangle *const getFrame( unsigned frame_index ) const;
    };
protected:
    VertexAttributeArray morph_model_attribute_array;
    VertexAttributeArray morph_bb_attribute_array;

    // uniforms are used for morpth attributes.
    GLuint sample_last_uniform_id;
    
    std::map<uint32_t, Animation*> model_animation_p;

private:
    void generalDraw( Graphics::SDL2::GLES2::Camera &camera, std::map<uint32_t, ModelArray*> &model_array_p, VertexAttributeArray &morph_attribute_array );
    
public:
    MorphModelDraw();
    virtual ~MorphModelDraw();

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
    int inputModel( Utilities::ModelBuilder *model_type_r,  const Data::Mission::ObjResource& obj, const std::map<uint32_t, Internal::Texture2D*>& textures );

    void clearModels();

    /**
     * This draws all of the models with the morph attribute.
     * @note Make sure setFragmentShader, loadFragmentShader, compileProgram and setWorld in this order are called SUCCESSFULLY.
     * @param camera This is the camera data to be passed into world.
     */
    void draw( Graphics::SDL2::GLES2::Camera &camera ) { generalDraw( camera, models_p, morph_model_attribute_array ); }

    /**
     * This draws all of the models with the morph attribute.
     * @note Make sure setFragmentShader, loadFragmentShader, compileProgram and setWorld in this order are called SUCCESSFULLY.
     * @param camera This is the camera data to be passed into world.
     */
    void drawBoundingBoxes( Graphics::SDL2::GLES2::Camera &camera ) { generalDraw( camera, bounding_boxes_p, morph_bb_attribute_array ); }
    
    virtual void advanceTime( float seconds_passed );
};

}
}
}
}

#endif // GRAPHICS_GLES2_INTERNAL_MORPH_MODEL_DRAW_H
