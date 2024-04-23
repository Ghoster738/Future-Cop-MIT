#ifndef GRAPHICS_GLES2_INTERNAL_DYNAMIC_TRIANGLE_DRAW_H
#define GRAPHICS_GLES2_INTERNAL_DYNAMIC_TRIANGLE_DRAW_H

#include <map>
#include "Texture2D.h"
#include "VertexAttributeArray.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

class Camera;

namespace Internal {

class DynamicTriangleDraw {
public:
    static const GLchar* default_vertex_shader;
    static const GLchar* default_fragment_shader;

    enum PolygonType {
        MIX             = 0,
        ADDITION        = 1,
        SUBTRACTION     = 2,
        LESSER_ADDITION = 3
    };

    struct Vertex {
        glm::vec3    position;
        glm::vec3    normal;
        glm::vec4    color;
        glm::vec2    coordinate;
        glm::i16vec2 vertex_metadata;

        // OpenGL should ignore these values.
        union {
            struct {
                uint32_t texture_id : 30;
                uint32_t polygon_type : 2; // https://psx-spx.consoledev.net/graphicsprocessingunitgpu/#semi-transparency
            } bitfield;
            float distance_from_camera;
        } metadata;
    };
    struct Triangle {
        Vertex vertices[3];

        void setup( uint32_t texture_id, const glm::vec3 &camera_position, PolygonType poly_type );

        float genDistanceSq( const glm::vec3 &camera_position ) const;

        Triangle addTriangle( const glm::vec3 &camera_position ) const;
        Triangle addTriangle( const glm::vec3 &camera_position, const glm::mat4 &matrix ) const;
    };
    struct DrawCommand {
        // This holds transparent triangles
        GLuint    vertex_buffer_object;
        Triangle *triangles_p;
        size_t    triangles_max;
        size_t    triangles_amount;

        DrawCommand();

        /**
        * Allocate the triangles that this render rountine could handle
        * @note 1 MiB would store 8738 Triangles with 16 bytes free
        * @param limit The number of triangles that dynamic triangle draw would support.
        * @return The number of triangles that where allocated.
        */
        size_t allocateBuffer( size_t limit = 8738 );

        /**
         * This method deletes the OpenGL allocations and resets the status of this struct.
         */
        void deleteBuffer();

        /**
        * This method clears the array of triangles.
        * @note O(1) basically sets a counter to zero.
        */
        void reset();

        /**
        * Allocate and get a single triangle.
        * @note This is not a const method for a reason. It adds a triangle to be drawn.
        * @warning The triangle is added to the list, so be sure to set the triangle up before drawing it.
        * @param number_of_triangles This contains the number of triangles to be allocated.
        * @param triangles_r This is the return for the array. The pointer will be set to null if nothing is allocated.
        * @return The number of triangles that where allocated.
        */
        size_t getTriangles( size_t number_of_triangles, Triangle** triangles_r );

        // The following methods are for internal use only.
        void sortTriangles();
        void draw( const VertexAttributeArray &vertex_array, const std::map<uint32_t, Graphics::SDL2::GLES2::Internal::Texture2D*> &textures, GLuint diffusive_texture_uniform_id ) const;
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
    GLuint view_uniform_id;
    GLuint view_inv_uniform_id;
    
    // The textures will also need to be accessed.
    Texture2D *env_texture_r;

    VertexAttributeArray vertex_array;
public:

    DynamicTriangleDraw();
    virtual ~DynamicTriangleDraw();

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
     * This draws all the stored triangles.
     * @note Make sure setFragmentShader, loadFragmentShader, compileProgram and allocateTriangles in this order are called SUCCESSFULLY.
     * @param camera This is the camera data to be passed into this routine.
     * @param textures This is the camera data to be passed into this routine.
     */
    void draw( Graphics::SDL2::GLES2::Camera &camera, const std::map<uint32_t, Graphics::SDL2::GLES2::Internal::Texture2D*> &textures );

    /**
     * @return the program that this World uses.
     */
    Program* getProgram() { return &program; }

    /**
     * @return The OpenGL id of the texture uniform from the static model program.
     */
    GLuint getDiffusiveTextureUniformID() const { return diffusive_texture_uniform_id; }

    /**
     * @return The OpenGL id of the matrix uniform from the static model program.
     */
    GLuint getCameraUniformID() const { return matrix_uniform_id; }
};

}

}

}

}

#endif // GRAPHICS_GLES2_INTERNAL_DYNAMIC_TRIANGLE_DRAW_H
