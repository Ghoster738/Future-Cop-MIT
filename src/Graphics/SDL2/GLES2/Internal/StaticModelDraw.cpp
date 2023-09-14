#include "StaticModelDraw.h"
#include "../ModelInstance.h"
#include <algorithm>
#include <cassert>
#include <cmath> // fmod()
#include <iostream>
#include "SDL.h"

void Graphics::SDL2::GLES2::Internal::StaticModelDraw::Dynamic::addTriangles(
            const std::vector<DynamicTriangleDraw::Triangle> &triangles,
            DynamicTriangleDraw::DrawCommand &triangles_draw ) const
{
    DynamicTriangleDraw::Triangle *draw_triangles_r;

    size_t number_of_triangles = triangles_draw.getTriangles( triangles.size(), &draw_triangles_r );

    for( size_t i = 0; i < number_of_triangles; i++ ) {
        draw_triangles_r[ i ] = triangles[ i ];

        for( unsigned t = 0; t < 3; t++ ) {
            draw_triangles_r[ i ].vertices[ t ].coordinate += texture_offset;
        }

        draw_triangles_r[ i ] = draw_triangles_r[ i ].addTriangle( this->camera_position, transform );
    }
}

const GLchar* Graphics::SDL2::GLES2::Internal::StaticModelDraw::default_vertex_shader =
    // Vertex shader uniforms
    "uniform mat4 ModelViewInv;\n"
    "uniform mat4 ModelView;\n"
    "uniform mat4 Transform;\n" // projection * view * model.
    "uniform vec2 TextureTranslation;\n"

    "void main()\n"
    "{\n"
    // This reflection code is based on https://stackoverflow.com/questions/27619078/reflection-mapping-in-opengl-es
    "   vec3 eye_coord_position = vec3( ModelView * POSITION );\n" // Model View multiplied by Model Position.
    "   vec3 eye_coord_normal   = vec3( ModelView * vec4(NORMAL, 0.0));\n"
    "   eye_coord_normal        = normalize( eye_coord_normal );\n"
    "   vec3 eye_reflection     = reflect( eye_coord_position, eye_coord_normal);\n"
    // Find a way to use the spherical projection properly.
    "   world_reflection        = vec3( ModelViewInv * vec4(eye_reflection, 0.0 ));\n"
    "   world_reflection        = normalize( world_reflection ) * 0.5 + vec3( 0.5, 0.5, 0.5 );\n"
    "   texture_coord_1 = TEXCOORD_0 + TextureTranslation;\n"
    "   specular = _Specular\n;"
    "   gl_Position = Transform * vec4(POSITION.xyz, 1.0);\n"
    "}\n";
const GLchar* Graphics::SDL2::GLES2::Internal::StaticModelDraw::default_fragment_shader =
    "uniform sampler2D Texture;\n"
    "uniform sampler2D Shine;\n"

    "void main()\n"
    "{\n"
    "  vec4 color = texture2D(Texture, texture_coord_1);\n"
    "   if( color.a < 0.015625 )\n"
    "       discard;\n"
    "  float BLENDING = 1.0 - color.a;\n"
    "  if( specular > 0.5 )\n"
    "    gl_FragColor = texture2D(Shine, world_reflection.xz) * BLENDING + vec4(color.rgb, 1.0);\n"
    "  else\n"
    "    gl_FragColor = color;\n"
    "}\n";

Graphics::SDL2::GLES2::Internal::StaticModelDraw::StaticModelDraw() {
    shiney_texture_r = nullptr;

    attributes.push_back( Shader::Attribute( Shader::Type::MEDIUM, "vec4 POSITION" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "vec3 NORMAL" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "vec2 TEXCOORD_0" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "float _Specular" ) );

    varyings.push_back( Shader::Varying( Shader::Type::LOW, "vec3 world_reflection" ) );
    varyings.push_back( Shader::Varying( Shader::Type::MEDIUM, "float specular" ) );
    varyings.push_back( Shader::Varying( Shader::Type::LOW, "vec2 texture_coord_1" ) );
}

Graphics::SDL2::GLES2::Internal::StaticModelDraw::~StaticModelDraw() {
    clearModels();
}

const GLchar* Graphics::SDL2::GLES2::Internal::StaticModelDraw::getDefaultVertexShader() {
    return default_vertex_shader;
}

const GLchar* Graphics::SDL2::GLES2::Internal::StaticModelDraw::getDefaultFragmentShader() {
    return default_fragment_shader;
}

void Graphics::SDL2::GLES2::Internal::StaticModelDraw::setVertexShader( const GLchar *const shader_source ) {
    vertex_shader.setShader( Shader::TYPE::VERTEX, shader_source, attributes, varyings );
}

int Graphics::SDL2::GLES2::Internal::StaticModelDraw::loadVertexShader( const char *const file_path ) {
    return vertex_shader.loadShader( Shader::TYPE::VERTEX, file_path );
}

void Graphics::SDL2::GLES2::Internal::StaticModelDraw::setFragmentShader( const GLchar *const shader_source ) {
    fragment_shader.setShader( Shader::TYPE::FRAGMENT, shader_source, {}, varyings  );
}

int Graphics::SDL2::GLES2::Internal::StaticModelDraw::loadFragmentShader( const char *const file_path ) {
    return fragment_shader.loadShader( Shader::TYPE::FRAGMENT, file_path );
}

int Graphics::SDL2::GLES2::Internal::StaticModelDraw::compileProgram() {
    bool uniform_failed = false;
    bool attribute_failed = false;
    bool link_success = true;
    
    // The two shaders should be allocated first.
    if( vertex_shader.getType() == Shader::TYPE::VERTEX && fragment_shader.getType() == Shader::TYPE::FRAGMENT ) {

        // Allocate the opengl program for the map.
        program.allocate();

        // Give the program these two shaders.
        program.setVertexShader( &vertex_shader );
        program.setFragmentShader( &fragment_shader );

        // Attempt to link the shader
        if( !program.link() )
            link_success = false;
        else
        {
            // Setup the uniforms for the map.
            diffusive_texture_uniform_id = program.getUniform( "Texture", &std::cout, &uniform_failed );
            sepecular_texture_uniform_id = program.getUniform( "Shine", &std::cout, &uniform_failed );
            texture_offset_uniform_id = program.getUniform( "TextureTranslation", &std::cout, &uniform_failed );
            matrix_uniform_id = program.getUniform( "Transform", &std::cout, &uniform_failed );
            view_uniform_id = program.getUniform( "ModelView", &std::cout, &uniform_failed );
            view_inv_uniform_id = program.getUniform( "ModelViewInv", &std::cout, &uniform_failed );
            
            attribute_failed |= !program.isAttribute( "POSITION", &std::cout );
            attribute_failed |= !program.isAttribute( "NORMAL", &std::cout );
            attribute_failed |= !program.isAttribute( "TEXCOORD_0", &std::cout );
            attribute_failed |= !program.isAttribute( "_Specular", &std::cout );

            link_success = true;
        }
        
        if( !link_success || uniform_failed || attribute_failed ) {
            std::cout << "StaticModelDraw program has failed." << std::endl;
            
            if( !link_success )
                std::cout << "There is trouble with linking." << std::endl;
            if( uniform_failed )
                std::cout << "There is trouble with the uniforms." << std::endl;
            if( attribute_failed )
                std::cout << "There is trouble with the attributes." << std::endl;
            
            std::cout << program.getInfoLog();
            std::cout << "\nVertex shader log\n";
            std::cout << vertex_shader.getInfoLog();
            std::cout << "\nFragment shader log\n";
            std::cout << fragment_shader.getInfoLog() << std::endl;
        }
        
        return link_success;
    }
    else
    {
        return 0; // Not every shader was loaded.
    }
}

void Graphics::SDL2::GLES2::Internal::StaticModelDraw::setTextures( Texture2D *shiney_texture_r ) {
    this->shiney_texture_r = shiney_texture_r;
}

bool Graphics::SDL2::GLES2::Internal::StaticModelDraw::containsModel( uint32_t obj_identifier ) const {
    if( models_p.find( obj_identifier ) != models_p.end() )
        return ( models_p.at( obj_identifier ) != nullptr );
    else
        return false;
}

int Graphics::SDL2::GLES2::Internal::StaticModelDraw::inputModel( Utilities::ModelBuilder *model_type_r, uint32_t obj_identifier, const std::map<uint32_t, Internal::Texture2D*>& textures ) {
    int state = 0;

    if( model_type_r->getNumVertices() > 0 )
    {
        models_p[ obj_identifier ] = new ModelArray( &program );
        models_p[ obj_identifier ]->mesh.setup( *model_type_r, textures );
        state =  1;

        Utilities::ModelBuilder::TextureMaterial material;
        GLsizei transparent_count = 0;

        for( unsigned a = 0; a < model_type_r->getNumMaterials(); a++ ) {
            model_type_r->getMaterial( a, material );
            transparent_count += material.count - material.opeque_count;
        }
        models_p[ obj_identifier ]->transparent_triangles.reserve( transparent_count );

        GLsizei material_count = 0;

        unsigned   position_compenent_index = model_type_r->getNumVertexComponents();
        unsigned coordinate_compenent_index = position_compenent_index;

        Utilities::ModelBuilder::VertexComponent element("EMPTY");
        for( unsigned i = 0; model_type_r->getVertexComponent( i, element ); i++ ) {
            auto name = element.getName();

            if( name == Utilities::ModelBuilder::POSITION_COMPONENT_NAME )
                position_compenent_index = i;
            if( name == Utilities::ModelBuilder::TEX_COORD_0_COMPONENT_NAME )
                coordinate_compenent_index = i;
        }

        for( unsigned int a = 0; a < model_type_r->getNumMaterials(); a++ ) {
            model_type_r->getMaterial( a, material );

            uint32_t cbmp_id;

            if( textures.find( material.cbmp_resource_id ) != textures.end() )
                cbmp_id = material.cbmp_resource_id;
            else if( !textures.empty() ) {
                cbmp_id = textures.begin()->first;
            }
            else
                cbmp_id = 0;

            GLsizei opeque_count = std::min( material.count, material.opeque_count );

            glm::vec4   positions[3] = {glm::vec4(0, 0, 0, 1)};
            glm::vec4      color     =  glm::vec4(1, 1, 1, 1);
            glm::vec4 coordinates[3] = {glm::vec4(0, 0, 0, 1)};

            const unsigned vertex_per_triangle = 3;

            for( unsigned m = opeque_count; m < material.count; m += vertex_per_triangle ) {
                DynamicTriangleDraw::Triangle triangle;

                for( unsigned t = 0; t < 3; t++ ) {
                    model_type_r->getTransformation(   positions[t],   position_compenent_index, material_count + m + t );
                    model_type_r->getTransformation( coordinates[t], coordinate_compenent_index, material_count + m + t );

                    triangle.vertices[t].position = { positions[t].x, positions[t].y, positions[t].z };
                    triangle.vertices[t].color = color;
                    triangle.vertices[t].coordinate = coordinates[t];
                }

                triangle.setup( cbmp_id, glm::vec3(0, 0, 0) );

                models_p[ obj_identifier ]->transparent_triangles.push_back( triangle );
            }

            material_count += material.count;
        }
    }
    else
        state = -1;

    return state;
}
void Graphics::SDL2::GLES2::Internal::StaticModelDraw::clearModels() {
    // Delete the models first.
    for( auto i = models_p.begin(); i != models_p.end(); i++ )
    {
        delete (*i).second; // First delete the pointer.
        (*i).second = nullptr; // Then set the pointer to null.
    }
    models_p.clear();
}

void Graphics::SDL2::GLES2::Internal::StaticModelDraw::draw( Graphics::SDL2::GLES2::Camera &camera ) {
    glm::mat4 camera_3D_model_transform; // This holds the model transform like the position rotation and scale.
    glm::mat4 camera_3D_projection_view_model; // This holds the two transforms from above.
    glm::mat4 camera_3D_projection_view; // This holds the camera transform along with the view.
    glm::mat4 view;
    glm::mat4 model_view;
    glm::mat4 model_view_inv;
    
    camera.getProjectionView3D( camera_3D_projection_view ); // camera_3D_projection_view = current_camera 3D matrix.

    camera.getView3D( view );
    
    // Use the static shader for the static models.
    program.use();

    // Check if there is even a shiney texture.
    if( shiney_texture_r != nullptr )
        shiney_texture_r->bind( 1, sepecular_texture_uniform_id );

    Dynamic dynamic;
    dynamic.camera_position = camera.getPosition();

    // Traverse the models.
    for( auto d = models_p.begin(); d != models_p.end(); d++ ) // Go through every model that has an instance.
    {
        // Get the mesh information.
        Graphics::SDL2::GLES2::Internal::Mesh *mesh_r = &(*d).second->mesh;
        
        // Go through every instance that refers to this mesh.
        for( auto instance = (*d).second->instances_r.begin(); instance != (*d).second->instances_r.end(); instance++ )
        {
            if( camera.isVisible( *(*instance) ) ) {
                const auto texture_offset = (*instance)->getTextureOffset();
                glUniform2f( this->texture_offset_uniform_id, texture_offset.x, texture_offset.y );

                dynamic.texture_offset = texture_offset;

                // Get the position and rotation of the model.
                // Multiply them into one matrix which will hold the entire model transformation.
                camera_3D_model_transform = glm::translate( glm::mat4(1.0f), (*instance)->getPosition() ) * glm::toMat4( (*instance)->getRotation() );
                
                // Then multiply it to the projection, and view to get projection, view, and model matrix.
                camera_3D_projection_view_model = camera_3D_projection_view * (glm::translate( glm::mat4(1.0f), (*instance)->getPosition() ) * glm::toMat4( (*instance)->getRotation() ));
                
                // We can now send the matrix to the program.
                glUniformMatrix4fv( matrix_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &camera_3D_projection_view_model[0][0] ) );
                
                model_view = view * camera_3D_model_transform;
                model_view_inv = glm::inverse( model_view );
                glUniformMatrix4fv( view_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &model_view[0][0] ) );
                glUniformMatrix4fv( view_inv_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &model_view_inv[0][0] ) );
                
                // Finally we can draw the mesh!
                mesh_r->drawOpaque( 0, diffusive_texture_uniform_id );
                
                dynamic.transform = camera_3D_model_transform;
                dynamic.addTriangles( (*d).second->transparent_triangles, camera.transparent_triangles );
            }
        }
    }
}

int Graphics::SDL2::GLES2::Internal::StaticModelDraw::allocateObjModel( uint32_t obj_identifier, GLES2::ModelInstance &model_instance ) {
    if( models_p.find( obj_identifier ) != models_p.end() ) // Do some bounds checking!
    {
        // This holds the model instance sheet.
        ModelArray *model_array_r = models_p[ obj_identifier ];

        model_instance.array_r = model_array_r;
        
        if( !models_p[ obj_identifier ]->mesh.getBoundingSphere( model_instance.culling_sphere_position, model_instance.culling_sphere_radius ) )
        {
            model_instance.culling_sphere_position = glm::vec3( 0, 0, 0 );
            model_instance.culling_sphere_radius = 1.0f;
        }

        // Finally added the instance.
        model_array_r->instances_r.insert( &model_instance );

        return 1; // The instance is successfully allocated.
    }
    else
        return -1; // The requested index_obj does not exist
}

void Graphics::SDL2::GLES2::Internal::StaticModelDraw::advanceTime( float time_seconds ) {
    // No operation. Static Meshes do not have the concept of time.
}

bool Graphics::SDL2::GLES2::Internal::StaticModelDraw::getBoundingSphere( uint32_t obj_identifier, glm::vec3 &position, float &radius ) const {
    if( models_p.find( obj_identifier ) != models_p.end() )
        return false;
    return models_p.at( obj_identifier )->mesh.getBoundingSphere( position, radius );
}
