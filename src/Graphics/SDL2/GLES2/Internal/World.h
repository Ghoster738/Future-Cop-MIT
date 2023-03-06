#ifndef GRAPHICS_GLES2_INTERNAL_WORLD_H
#define GRAPHICS_GLES2_INTERNAL_WORLD_H

#include "Mesh.h"
#include "../../../Camera.h"
#include "../../../../Data/Mission/PTCResource.h"
#include "../../../../Data/Mission/TilResource.h"

namespace Graphics {
namespace SDL2 {
namespace GLES2 {
namespace Internal {

/**
 * This handles everything the world will draw and handle.
 * 
 * Draws every tile of the world map.
 * I might world on occulusion culling in the future, but
 * for now this is good enough.
 */
class World {
public:
    class MeshDraw {
    public:
        Mesh *mesh;
        const Data::Mission::TilResource *tilResourceR;
        float change_rate;
        float current; // [ -change_rate, change_rate ]
        unsigned int positions_amount;
        glm::i32vec2 *positions;
    };
    static const GLchar* default_vertex_shader;
    static const GLchar* default_fragment_shader;
protected:
    Program program;
    std::vector<Shader::Attribute> attributes;
    std::vector<Shader::Varying>   varyings;
    Shader vertex_shader;
    Shader fragment_shader;
    GLuint texture_uniform_id;
    GLuint matrix_uniform_id;
    GLuint glow_time_uniform_id;
    GLuint selected_tile_uniform_id;
    std::vector<MeshDraw> tiles;
    
    GLfloat selected_tile;
    GLfloat current_selected_tile;
    GLfloat scale;
    GLfloat glow_time;
public:
    World();
    virtual ~World();

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
    int compilieProgram();

    /**
     * This sets the world.
     * @param pointer_tile_cluster This is the pointerTileCluster.
     * @param resources_til This caries the til resources in which the world will use.
     * @param textures The default textures also color the map.
     * @return If ptc does not exist in the parameter resource it will return false and do nothing.
     */
    void setWorld( const Data::Mission::PTCResource &pointer_tile_cluster, const std::vector<Data::Mission::TilResource*> resources_til, const std::map<uint32_t, Internal::Texture2D*>& textures );

    /**
     * This draws the entire map.
     * @note Make sure setFragmentShader, loadFragmentShader, compilieProgram and setWorld in this order are called SUCCESSFULLY.
     * @param This is the camera data to be passed into world.
     */
    void draw( const Camera &camera );

    /**
     * @return the program that this World uses.
     */
    Program* getProgram() { return &program; }

    /**
     * @return The OpenGL id of the texture uniform from the map program.
     */
    GLuint getTextureUniformID() const { return texture_uniform_id; }

    /**
     * @return The OpenGL id of the matrix uniform from the map program.
     */
    GLuint setCameraUniformID() const { return matrix_uniform_id; };

    /**
     * This advances the time of every instance.
     * @param seconds_passed This is the time in seconds that were passed.
     */
    void advanceTime( float seconds_passed );
    
    size_t getTilAmount() const;

    /**
     * This sets the blink rate to an entire tile set for the use of selection.
     * @param til_index The index to the til_index
     * @param frequency The blink state in seconds. Basically it is how many seconds before a change in state.
     * @return the til\_index for success or 0 if the til\_index is out of bounds.
     */
    int setTilBlink( unsigned til_index, float frequency );
    
    /**
     * This sets the blinking for the polygon tiles.
     * @param polygon_type The type of polygon to highlight.
     * @param scale This scales the blinking factor of world.
     * @return -1 if there is no functionality to set the polygon type. 0 if the end of the types has been reached. Otherwise just 1.
     */
    int setPolygonTypeBlink( unsigned polygon_type, GLfloat scale = 1.0f );
};

}
}
}
}

#endif // GL_MAP_3859_INCLUDE
