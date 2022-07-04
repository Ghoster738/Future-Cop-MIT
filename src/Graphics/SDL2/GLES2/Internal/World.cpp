#include "World.h"
#include "../../../../Data/Mission/IFF.h"
#include "GLES2.h"

#include <glm/ext/matrix_transform.hpp>
#include <cassert>

const GLchar* Graphics::SDL2::GLES2::Internal::World::default_vertex_shader =
    // Inputs
    "attribute vec4 POSITION;\n"
    "attribute vec2 TEXCOORD_0;\n"
    "attribute vec3 COLOR_0;\n"
    "attribute float _TileType;\n"

    // Vertex shader uniforms
    "uniform mat4  Transform;\n" // projection * view * model.
    "uniform float CurrentGlow;\n"
    "uniform float  WhichTile;\n" // cause the tile to flash.

    // These are the outputs
    "varying vec3 vertex_colors;\n"
    "varying vec2 texture_coord_1;\n"
    "varying float _flashing;\n"

    "void main()\n"
    "{\n"
    "   vertex_colors = COLOR_0;\n"
    "   texture_coord_1 = TEXCOORD_0;\n"
    "   float state = float(WhichTile > _TileType - 0.5 && WhichTile < _TileType + 0.5);"
    "   _flashing = CurrentGlow * state;\n"
    "   gl_Position = Transform * vec4(POSITION.xyz, 1.0);\n"
    "}\n";
const GLchar* Graphics::SDL2::GLES2::Internal::World::default_fragment_shader =
    "varying vec3 vertex_colors;\n"
    "varying vec2 texture_coord_1;\n"
    "varying float _flashing;\n"

    "uniform sampler2D Texture;\n"

    "const vec4 frag_inv = vec4(1,1,1,0);\n"

    "void main()\n"
    "{\n"
    "    vec4 frag_color = texture2D(Texture, texture_coord_1);\n"
    "    if( frag_color.x < 0.004 && frag_color.y < 0.004 && frag_color.z < 0.004)"
    "        discard;"
    "    frag_color *= vec4(vertex_colors.xyz, 0);"
    "    gl_FragColor = (1.0 - _flashing) * frag_color + _flashing * (frag_inv - frag_color);\n"
    "}\n";

Graphics::SDL2::GLES2::Internal::World::World() {
}

Graphics::SDL2::GLES2::Internal::World::~World() {
    for( auto i = tiles.begin(); i != tiles.end(); i++ ) {
        delete (*i).mesh;
        delete [] (*i).positions;
    }
}

void Graphics::SDL2::GLES2::Internal::World::setVertexShader( const GLchar *const shader_source ) {
    vertex_shader.setShader( Shader::TYPE::VERTEX, shader_source );
}

int Graphics::SDL2::GLES2::Internal::World::loadVertexShader( const char *const file_path ) {
    return vertex_shader.loadShader( Shader::TYPE::VERTEX, file_path );
}

void Graphics::SDL2::GLES2::Internal::World::setFragmentShader( const GLchar *const shader_source ) {
    fragment_shader.setShader( Shader::TYPE::FRAGMENT, shader_source );
}

int Graphics::SDL2::GLES2::Internal::World::loadFragmentShader( const char *const file_path ) {
    return fragment_shader.loadShader( Shader::TYPE::FRAGMENT, file_path );
}

int Graphics::SDL2::GLES2::Internal::World::compilieProgram() {
    // The two shaders should be allocated first.
    if( vertex_shader.getType() == Shader::TYPE::VERTEX && fragment_shader.getType() == Shader::TYPE::FRAGMENT ) {
        // Allocate the opengl program for the map.
        program.allocate();

        // Give the program these two shaders.
        program.setVertexShader( &vertex_shader );
        program.setFragmentShader( &fragment_shader );

        // Link the shader
        program.link();

        // Setup the uniforms for the map.
        texture_uniform_id = glGetUniformLocation( program.getProgramID(), "Texture" );
        matrix_uniform_id = glGetUniformLocation( program.getProgramID(), "Transform" );

        return 1;
    }
    else
    {
        return 0; // Not every shader was loaded.
    }
}

void Graphics::SDL2::GLES2::Internal::World::setWorld( const Data::Mission::PTCResource &pointer_tile_cluster, const std::vector<Data::Mission::TilResource*> resources_til, const std::vector<Texture2D*> textures ) {
    tiles.resize( resources_til.size() );

    // Set up the primary tiles. O(n)
    for( auto i = tiles.begin(); i != tiles.end(); i++ ) {
        const Data::Mission::TilResource *data = resources_til[ i - tiles.begin() ];
        auto model = data->createModel( nullptr );

        assert( model != nullptr );

        (*i).mesh = new Graphics::SDL2::GLES2::Internal::Mesh( &program );
        (*i).tilResourceR = data;
        (*i).change_rate = -1.0;
        (*i).current = 0.0;
        (*i).positions_amount = 0;

        (*i).mesh->setup( *model, textures );

        delete model;
    }

    // Set the position amounts. O(x*y) or O(n^2).
    for( unsigned int x = 0; x < pointer_tile_cluster.getWidth(); x++ )
    {
        for( unsigned int y = 0; y < pointer_tile_cluster.getHeight(); y++ )
        {
            auto pointer = pointer_tile_cluster.getTile(x, y);
            if( pointer != nullptr )
                tiles.at( pointer->getIndexNumber() ).positions_amount++;
        }
    }

    // Allocate them. O(n)
    for( auto i = tiles.begin(); i != tiles.end(); i++ ) {
        (*i).positions = new glm::i32vec2 [ (*i).positions_amount ];
        (*i).positions_amount = 0; // This will be used as an index. Later it will change back into the orignal amount of positions.
    }

    // Finally setup the map. O(x*y) or O(n^2).
    for( unsigned int x = 0; x < pointer_tile_cluster.getWidth(); x++  )
    {
        for( unsigned int y = 0; y < pointer_tile_cluster.getHeight(); y++ )
        {
            auto pointer = pointer_tile_cluster.getTile( pointer_tile_cluster.getWidth() - x - 1, y );
            if( pointer != nullptr )
            {
                unsigned int index = pointer->getIndexNumber();

                tiles.at(index).positions[ tiles.at(index).positions_amount ].x = y - pointer_tile_cluster.getHeight() / 2;
                tiles.at(index).positions[ tiles.at(index).positions_amount ].y = x - pointer_tile_cluster.getWidth()  / 2;

                tiles.at(index).positions_amount++;
            }
        }
    }
    // This algorithm is 2*O(n^2) + 3*O(n) = O(n^2).
}

void Graphics::SDL2::GLES2::Internal::World::draw( const Graphics::Camera &camera ) {
    glm::mat4 til_position;
    glm::mat4 projection_view;
    glm::mat4 projection_view_model_pos;
    
    // Use the map shader for the 3D map or the world.
    program.use();

    camera.getProjectionView3D( projection_view );

    for( auto i = tiles.begin(); i != tiles.end(); i++ ) {
        if( (*i).current >= 0.0 )
        for( unsigned int d = 0; d < (*i).positions_amount; d++ ) {
            til_position = glm::translate( glm::mat4( 1.0 ), glm::vec3( ((*i).positions[d].x * 16), 0, ((*i).positions[d].y * 16 ) ) );

            projection_view_model_pos = projection_view * til_position;

            // We can now send the matrix to the program.
            glUniformMatrix4fv( matrix_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &projection_view_model_pos[0][0] ) );

            (*i).mesh->draw( 0, texture_uniform_id );
        }
    }
}


void Graphics::SDL2::GLES2::Internal::World::advanceTime( float seconds_passed ) {
    for( auto i = tiles.begin(); i != tiles.end(); i++ ) {
        if( (*i).change_rate > 0.0 )
        {
            (*i).current += seconds_passed;
            if( (*i).current > (*i).change_rate )
                (*i).current -= (*i).change_rate * 2.0;
        }

    }
}


int Graphics::SDL2::GLES2::Internal::World::setTilBlink( int til_index, float frequency ) {
    if( til_index < 0 )
        return tiles.size() - 1;
    else
    if( static_cast<unsigned int>( til_index ) < tiles.size() )
    {
        tiles[ til_index ].current = 0.0;
        tiles[ til_index ].change_rate = frequency;

        return til_index;
    }
    else
        return 0;
}
