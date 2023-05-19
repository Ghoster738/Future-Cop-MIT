#include "World.h"
#include "../../../../Data/Mission/IFF.h"
#include "../../../../Utilities/Collision/GJK.h"
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
    "   if( frag_color.a < 0.015625 )\n"
    "       discard;\n"
    "   vec3 normal_color = vertex_colors * frag_color.rgb * 2.0;\n"
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
        delete (*i).mesh_p;
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
        auto model_p = data->createCulledModel();

        assert( model_p != nullptr );

        (*i).mesh_p = new Graphics::SDL2::GLES2::Internal::Mesh( &program );
        (*i).til_resource_r = data;
        (*i).change_rate = -1.0;
        (*i).current = 0.0;

        (*i).mesh_p->setup( *model_p, textures );

        delete model_p;
    }

    {
        std::vector<uint32_t> temp_amounts( tiles.size(), 0 );

        // Set the position amounts. O(x*y) or O(n^2).
        for( unsigned int x = 0; x < pointer_tile_cluster.getWidth(); x++ )
        {
            for( unsigned int y = 0; y < pointer_tile_cluster.getHeight(); y++ )
            {
                auto pointer = pointer_tile_cluster.getTile(x, y);
                if( pointer != nullptr )
                    temp_amounts[ pointer->getIndexNumber() ]++;
            }
        }

        // Allocate them. O(n)
        for( auto i = tiles.begin(); i != tiles.end(); i++ ) {
            (*i).sections.reserve( temp_amounts[ i - tiles.begin() ] );
        }
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
                
                tiles.at(index).sections.push_back( MeshDraw::Section() );

                tiles.at(index).sections.back().position.x = x - 1;
                tiles.at(index).sections.back().position.y = y;
            }
        }
    }
    // This algorithm is 2*O(n^2) + 3*O(n) = O(n^2).
}

bool Graphics::SDL2::GLES2::Internal::World::updateCulling( Utilities::GridBase2D<float> &culling_info, const Utilities::Collision::GJKShape &projection, const glm::vec3 &position ) const {
    if( culling_info.getWidth() * culling_info.getHeight() == 0 ) {
        return false;
    }

    std::vector<glm::vec3> section_data( 8, glm::vec3() );
    const glm::vec3 MIN = glm::vec3(0, Data::Mission::TilResource::MAX_HEIGHT, 0);
    const glm::vec3 MAX = glm::vec3( Data::Mission::TilResource::AMOUNT_OF_TILES, Data::Mission::TilResource::MIN_HEIGHT, Data::Mission::TilResource::AMOUNT_OF_TILES );

    for( auto m : tiles ) {
        for( auto s : m.sections ) {
            glm::vec3 adjusted_position( s.position.x, 0, s.position.y );
            adjusted_position *= Data::Mission::TilResource::AMOUNT_OF_TILES;

            glm::vec3 min = adjusted_position + MIN;
            glm::vec3 max = adjusted_position + MAX;

            section_data[0] = glm::vec3( min.x, min.y, min.z );
            section_data[1] = glm::vec3( max.x, min.y, min.z );
            section_data[2] = glm::vec3( min.x, max.y, min.z );
            section_data[3] = glm::vec3( max.x, max.y, min.z );
            section_data[4] = glm::vec3( min.x, min.y, max.z );
            section_data[5] = glm::vec3( max.x, min.y, max.z );
            section_data[6] = glm::vec3( min.x, max.y, max.z );
            section_data[7] = glm::vec3( max.x, max.y, max.z );

            Utilities::Collision::GJKPolyhedron section_shape( section_data );

            if( Utilities::Collision::GJK::hasCollision( projection, section_shape ) == Utilities::Collision::GJK::NO_COLLISION ) {
                culling_info.setValue( s.position.x, s.position.y, -1.0f );
            }
            else {
                const auto distance_2 = (adjusted_position.x - position.x) * (adjusted_position.x - position.x) + (adjusted_position.z - position.z) * (adjusted_position.z - position.z);

                culling_info.setValue( s.position.x, s.position.y, distance_2 );
            }
        }
    }

    return true;
}

void Graphics::SDL2::GLES2::Internal::World::draw( const Graphics::Camera &camera, const Utilities::GridBase2D<float> *const culling_info_r, DynamicTriangleDraw *dynamic_triangles_r ) {
    glm::mat4 projection_view, final_position, position_mat;
    
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

     const float TILE_SPAN = 0.5;

     const auto camera_position = camera.getPosition();
     const float squared_distance_culling = 32.0 * 32.0;

    for( auto i = tiles.begin(); i != tiles.end(); i++ ) {
        if( (*i).current >= 0.0 )
        for( unsigned int d = 0; d < (*i).sections.size(); d++ ) {

            auto section = (*i).sections[d];

            if( culling_info_r == nullptr || culling_info_r->getValue( section.position.x, section.position.y ) >= -0.5 ) {
                const glm::vec3 position = glm::vec3(
                    ((section.position.x * Data::Mission::TilResource::AMOUNT_OF_TILES + Data::Mission::TilResource::SPAN_OF_TIL) + TILE_SPAN),
                    0,
                    ((section.position.y * Data::Mission::TilResource::AMOUNT_OF_TILES + Data::Mission::TilResource::SPAN_OF_TIL) + TILE_SPAN) );

                final_position = glm::translate( projection_view, position );

                position_mat = glm::translate( glm::mat4(1), position );

                // We can now send the matrix to the program.
                glUniformMatrix4fv( matrix_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &final_position[0][0] ) );

                if( culling_info_r != nullptr && culling_info_r->getValue( section.position.x, section.position.y ) < squared_distance_culling ) {
                    (*i).mesh_p->drawOpaque( 0, texture_uniform_id );

                    if( dynamic_triangles_r != nullptr ) {
                        (*i).mesh_p->addTransparentTriangles( camera_position, position_mat, *dynamic_triangles_r );
                    }
                }
                else
                    (*i).mesh_p->draw( 0, texture_uniform_id );


            }
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
