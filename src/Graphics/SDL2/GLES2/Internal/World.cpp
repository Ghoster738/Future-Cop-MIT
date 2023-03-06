#include "World.h"
#include "../../../../Data/Mission/IFF.h"
#include "GLES2.h"

#include <glm/ext/matrix_transform.hpp>
#include "SDL.h"
#include <iostream>

const GLchar* Graphics::SDL2::GLES2::Internal::World::default_vertex_shader =
    // Vertex shader uniforms
    "uniform mat4  Transform;\n" // projection * view * model.
    "uniform float GlowTime;\n"
    "uniform float SelectedTile;\n"

    "void main()\n"
    "{\n"
    "   vertex_colors = COLOR_0;\n"
    "   texture_coord_1 = TEXCOORD_0;\n"
    "   _flashing = GlowTime * float(SelectedTile > _TileType - 0.5 && SelectedTile < _TileType + 0.5);\n"
    "   gl_Position = Transform * vec4(POSITION.xyz, 1.0);\n"
    "}\n";
const GLchar* Graphics::SDL2::GLES2::Internal::World::default_fragment_shader =
    "uniform sampler2D Texture;\n"

    "const vec3 frag_inv = vec3(1,1,1);\n"

    "void main()\n"
    "{\n"
    "   vec4 frag_color = texture2D(Texture, texture_coord_1);\n"
    "   if( frag_color.a < 0.015625 )"
    "       discard;\n"
    "   vec3 normal_color = vertex_colors * frag_color.rgb;\n"
    "   vec3 inverse_color = frag_inv - normal_color;\n"
    "   gl_FragColor = vec4( (1.0 - _flashing) * normal_color + _flashing * inverse_color, frag_color.a );\n"
    "}\n";

Graphics::SDL2::GLES2::Internal::World::World() {
    glow_time = 0;

    attributes.push_back( Shader::Attribute( Shader::Type::MEDIUM, "vec4 POSITION" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "vec2 TEXCOORD_0" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "vec3 COLOR_0" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::MEDIUM, "float _TileType" ) );

    varyings.push_back( Shader::Varying( Shader::Type::LOW, "vec3 vertex_colors" ) );
    varyings.push_back( Shader::Varying( Shader::Type::LOW, "vec2 texture_coord_1" ) );
    varyings.push_back( Shader::Varying( Shader::Type::LOW, "float _flashing" ) );
}

Graphics::SDL2::GLES2::Internal::World::~World() {
    for( auto i = tiles.begin(); i != tiles.end(); i++ ) {
        delete (*i).mesh;
        delete [] (*i).positions;
    }
}

const GLchar* Graphics::SDL2::GLES2::Internal::World::getDefaultVertexShader() {
    return default_vertex_shader;
}

const GLchar* Graphics::SDL2::GLES2::Internal::World::getDefaultFragmentShader() {
    return default_fragment_shader;
}

void Graphics::SDL2::GLES2::Internal::World::setVertexShader( const GLchar *const shader_source ) {
    vertex_shader.setShader( Shader::TYPE::VERTEX, shader_source, attributes, varyings );
}

int Graphics::SDL2::GLES2::Internal::World::loadVertexShader( const char *const file_path ) {
    return vertex_shader.loadShader( Shader::TYPE::VERTEX, file_path );
}

void Graphics::SDL2::GLES2::Internal::World::setFragmentShader( const GLchar *const shader_source ) {
    fragment_shader.setShader( Shader::TYPE::FRAGMENT, shader_source, {}, varyings );
}

int Graphics::SDL2::GLES2::Internal::World::loadFragmentShader( const char *const file_path ) {
    return fragment_shader.loadShader( Shader::TYPE::FRAGMENT, file_path );
}

int Graphics::SDL2::GLES2::Internal::World::compilieProgram() {
    bool link_success     = true;
    bool uniform_failed   = false;
    bool attribute_failed = false;
    
    // The two shaders should be allocated first.
    if( vertex_shader.getType() == Shader::TYPE::VERTEX && fragment_shader.getType() == Shader::TYPE::FRAGMENT ) {
        // Allocate the opengl program for the map.
        program.allocate();

        // Give the program these two shaders.
        program.setVertexShader( &vertex_shader );
        program.setFragmentShader( &fragment_shader );

        // Link the shader
        if( !program.link() )
            link_success = false;
        else
        {
            // Setup the uniforms for the map.
            texture_uniform_id       = program.getUniform( "Texture",   &std::cout, &uniform_failed );
            matrix_uniform_id        = program.getUniform( "Transform", &std::cout, &uniform_failed );
            glow_time_uniform_id     = program.getUniform( "GlowTime",  &std::cout, &uniform_failed );
            selected_tile_uniform_id = program.getUniform( "SelectedTile",  &std::cout, &uniform_failed );
            
            attribute_failed |= !program.isAttribute( "POSITION",   &std::cout );
            attribute_failed |= !program.isAttribute( "TEXCOORD_0", &std::cout );
            attribute_failed |= !program.isAttribute( "COLOR_0",    &std::cout );
            attribute_failed |= !program.isAttribute( "_TileType",  &std::cout );
        }
        
        if( !link_success || uniform_failed || attribute_failed )
        {
            std::cout << "World Draw Error\n";
            std::cout << program.getInfoLog();
            std::cout << "\nVertex shader log\n";
            std::cout << vertex_shader.getInfoLog();
            std::cout << "\nFragment shader log\n";
            std::cout << fragment_shader.getInfoLog() << std::endl;
            
            return 1;
        }
        else
            return -1;
    }
    else
    {
        return 0; // Not every shader was loaded.
    }
}

void Graphics::SDL2::GLES2::Internal::World::setWorld( const Data::Mission::PTCResource &pointer_tile_cluster, const std::vector<Data::Mission::TilResource*> resources_til,  const std::map<uint32_t, Internal::Texture2D*>& textures ) {
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
            auto pointer = pointer_tile_cluster.getTile( x, y );
            if( pointer != nullptr )
            {
                unsigned int index = pointer->getIndexNumber();
                
                tiles.at(index).positions[ tiles.at(index).positions_amount ].x = x - 1;
                tiles.at(index).positions[ tiles.at(index).positions_amount ].y = y;

                tiles.at(index).positions_amount++;
            }
        }
    }
    // This algorithm is 2*O(n^2) + 3*O(n) = O(n^2).
}

void Graphics::SDL2::GLES2::Internal::World::draw( const Graphics::Camera &camera ) {
    glm::mat4 projection_view, final_position;
    
    // Use the map shader for the 3D map or the world.
    program.use();

    camera.getProjectionView3D( projection_view );
    
    if( this->glow_time > 1.0f )
        glUniform1f( glow_time_uniform_id, 2.0f - this->glow_time );
    else
        glUniform1f( glow_time_uniform_id, this->glow_time );
    
    if( this->selected_tile != this->current_selected_tile ) {
        this->current_selected_tile = this->selected_tile;
        glUniform1f( selected_tile_uniform_id, this->selected_tile );
    }

    for( auto i = tiles.begin(); i != tiles.end(); i++ ) {
        if( (*i).current >= 0.0 )
        for( unsigned int d = 0; d < (*i).positions_amount; d++ ) {
            final_position = glm::translate( projection_view, glm::vec3( ((*i).positions[d].x * 16 + 8.5), 0, ((*i).positions[d].y * 16  + 8.5) ) );

            // We can now send the matrix to the program.
            glUniformMatrix4fv( matrix_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &final_position[0][0] ) );

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
    
    // Update glow time.
    this->glow_time += seconds_passed * this->scale;
    
    if( this->glow_time > 2.0f )
        this->glow_time = 0.0f;
}

size_t Graphics::SDL2::GLES2::Internal::World::getTilAmount() const {
    return tiles.size();
}

int Graphics::SDL2::GLES2::Internal::World::setTilBlink( unsigned til_index, float frequency ) {
    if( til_index < tiles.size() )
    {
        tiles[ til_index ].current = 0.0;
        tiles[ til_index ].change_rate = frequency;

        return 1;
    }
    else
        return 0;
}

int Graphics::SDL2::GLES2::Internal::World::setPolygonTypeBlink( unsigned polygon_type, GLfloat scale ) {
    if( polygon_type < 112 ) {
        this->scale = scale;
        this->selected_tile = polygon_type;
        return 1;
    }
    else
        return 0;
}
