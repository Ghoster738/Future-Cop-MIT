#ifndef GRAPHICS_GLES2_INTERNAL_DYNAMIC_TRIANGLE_DRAW_H
#define GRAPHICS_GLES2_INTERNAL_DYNAMIC_TRIANGLE_DRAW_H

#include "Mesh.h"
#include "../../../Camera.h"
#include "VertexAttributeArray.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {

namespace Internal {

class DynamicTriangleDraw {
public:
    static const GLchar* default_vertex_shader;
    static const GLchar* default_fragment_shader;

    struct Vertex {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 coordinate;

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

        void setup( uint32_t texture_id, const glm::vec3 &camera_position );
    };
protected:
    Program program;
    std::vector<Shader::Attribute> attributes;
    std::vector<Shader::Varying>   varyings;
    Shader vertex_shader;
    Shader fragment_shader;

    GLuint diffusive_texture_uniform_id;
    GLuint matrix_uniform_id; // model * view * projection.
    
    VertexAttributeArray vertex_array;

    // This holds transparent triangles
    GLuint    vertex_buffer_object;
    Triangle *transparent_triangles_p;
    size_t    transparent_triangles_max;
    size_t    transparent_triangles_amount;

    void deleteTriangles();
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
     * Allocate the triangles that this render rountine could handle
     * @note 1 MiB would store 8738 Triangles with 16 bytes free
     * @param limit The number of triangles that dynamic triangle draw would support.
     * @return The number of triangles that where allocated.
     */
    size_t allocateTriangles( size_t limit = 8738 );

    /**
     * This method clears the list of triangles.
     * @note O(1) basically sets a counter to zero.
     */
    void clearTriangles();
    
    /**
     * Allocate and get a single triangle.
     * @note This is not a const method for a reason. It adds a triangle to be drawn.
     * @warning The triangle is added to the list, so be sure to set the triangle up before drawing it.
     * @return A direct reference to the triangle if successful.
     */
    Triangle* getTriangle();

    /**
     * This draws all the stored triangles.
     * @note Make sure setFragmentShader, loadFragmentShader, compilieProgram and allocateTriangles in this order are called SUCCESSFULLY.
     * @param camera This is the camera data to be passed into this routine.
     * @param textures This is the camera data to be passed into this routine.
     */
    void draw( const Camera &camera, const std::map<uint32_t, Graphics::SDL2::GLES2::Internal::Texture2D*> &textures );

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
