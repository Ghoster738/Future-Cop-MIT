#include "MorphModelDraw.h"
#include "../ModelInstance.h"
#include <cassert>
#include <iostream>
#include "SDL.h"

#include "../Camera.h"

namespace {
    const size_t MORPH_BUFFER_SIZE = (3 + 3) * sizeof( float );
}

void Graphics::SDL2::GLES2::Internal::MorphModelDraw::Animation::Dynamic::addTriangles(
            const std::vector<DynamicTriangleDraw::Triangle> &triangles,
            DynamicTriangleDraw::DrawCommand &triangles_draw ) const
{
    const DeltaTriangle *const delta_triangle_r = morph_info_r->getFrame( frame_index );
    
    DynamicTriangleDraw::Triangle *draw_triangles_r;

    size_t number_of_triangles = triangles_draw.getTriangles( triangles.size(), &draw_triangles_r );

    for( size_t i = 0; i < number_of_triangles; i++ ) {
        draw_triangles_r[ i ] = triangles[ i ];
        
        if( delta_triangle_r != nullptr ) {
            for( unsigned t = 0; t < 3; t++ ) {
                draw_triangles_r[ i ].vertices[ t ].position += delta_triangle_r[ i ].vertices[ t ];
            }
        }
        
        draw_triangles_r[ i ] = draw_triangles_r[ i ].addTriangle( this->camera_position, transform );
    }
}

Graphics::SDL2::GLES2::Internal::MorphModelDraw::Animation::Animation( Utilities::ModelBuilder *model_type_r, GLsizei transparent_count ) {
    const unsigned frame_amount = model_type_r->getNumMorphFrames();
    
    this->triangles_per_frame = transparent_count;
    this->frame_data.reserve( triangles_per_frame * frame_amount );

    unsigned position_compenent_index = model_type_r->getNumVertexComponents();

    Utilities::ModelBuilder::VertexComponent element("EMPTY");
    for( unsigned i = 0; model_type_r->getMorphVertexComponent( i, element ); i++ ) {
        auto name = element.getName();

        if( name == Utilities::ModelBuilder::POSITION_COMPONENT_NAME )
            position_compenent_index = i;
    }

    Utilities::ModelBuilder::TextureMaterial material;
    GLsizei material_count;
    
    for( unsigned f = 0; f < frame_amount; f++ ) {
        material_count = 0;
        
        for( unsigned int a = 0; a < model_type_r->getNumMaterials(); a++ ) {
            model_type_r->getMaterial( a, material );
            
            unsigned opeque_count = std::min( material.count, material.opeque_count );
            
            glm::vec4 joints = glm::vec4(0, 0, 0, 1);
            
            const unsigned vertex_per_triangle = 3;
            
            for( unsigned m = opeque_count; m < material.count; m += vertex_per_triangle ) {
                DeltaTriangle triangle;
                
                for( unsigned t = 0; t < vertex_per_triangle; t++ ) {
                    model_type_r->getTransformation( joints, position_compenent_index, material_count + m + t, f );
                    
                    triangle.vertices[t].x = joints.x;
                    triangle.vertices[t].y = joints.y;
                    triangle.vertices[t].z = joints.z;
                }
                
                this->frame_data.push_back( triangle );
            }
            
            material_count += material.count;
        }
    }
}

const Graphics::SDL2::GLES2::Internal::MorphModelDraw::Animation::DeltaTriangle *const Graphics::SDL2::GLES2::Internal::MorphModelDraw::Animation::getFrame( unsigned frame_index ) const {
    if( frame_index == 0 )
        return nullptr;
    return &frame_data.at( (frame_index - 1) * triangles_per_frame );
}

const GLchar* Graphics::SDL2::GLES2::Internal::MorphModelDraw::default_vertex_shader =
    // Vertex shader uniforms
    "uniform mat4 ModelViewInv;\n"
    "uniform mat4 ModelView;\n"
    "uniform mat4 Transform;\n" // projection * view * model.
    "uniform vec2 TextureTranslation;\n"
    // These uniforms are for modifiying the morph attributes
    "uniform float SampleNext;\n"
    "uniform float SampleLast;\n"

    "void main()\n"
    "{\n"
    "   vec4 current_position = POSITION + POSITION_Last * vec4( SampleLast ) + POSITION_Next * vec4( SampleNext );\n"
    "   vec3 current_normal   = NORMAL   + NORMAL_Last   * vec3( SampleLast ) + NORMAL_Next   * vec3( SampleNext );\n"
    // This reflection code is based on https://stackoverflow.com/questions/27619078/reflection-mapping-in-opengl-es
    "   vec3 eye_coord_position = vec3( ModelView * current_position );\n" // Model View multiplied by Model Position.
    "   vec3 eye_coord_normal   = vec3( ModelView * vec4(current_normal, 0.0));\n"
    "   eye_coord_normal        = normalize( eye_coord_normal );\n"
    "   vec3 eye_reflection     = reflect( eye_coord_position, eye_coord_normal);\n"
    // Find a way to use the spherical projection properly.
    "   world_reflection        = vec3( ModelViewInv * vec4(eye_reflection, 0.0 ));\n"
    "   world_reflection        = normalize( world_reflection ) * 0.5 + vec3( 0.5, 0.5, 0.5 );\n"
    "   texture_coord_1 = TEXCOORD_0 + TextureTranslation;\n"
    "   specular = _Specular;\n"
    "   gl_Position = Transform * vec4(current_position.xyz, 1.0);\n"
    "}\n";
Graphics::SDL2::GLES2::Internal::MorphModelDraw::MorphModelDraw() {
    // These inputs are for the morph attributes
    attributes.push_back( Shader::Attribute( Shader::Type::MEDIUM, "vec4 POSITION_Next" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "vec3 NORMAL_Next" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::MEDIUM, "vec4 POSITION_Last" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "vec3 NORMAL_Last" ) );
}

Graphics::SDL2::GLES2::Internal::MorphModelDraw::~MorphModelDraw() {
    clearModels();
}

const GLchar* Graphics::SDL2::GLES2::Internal::MorphModelDraw::getDefaultVertexShader() {
    return default_vertex_shader;
}

int Graphics::SDL2::GLES2::Internal::MorphModelDraw::compileProgram() {
    auto ret = Graphics::SDL2::GLES2::Internal::StaticModelDraw::compileProgram();
    bool uniform_failed = false;
    bool attribute_failed = false;

    sample_next_uniform_id = program.getUniform( "SampleNext", &std::cout, &uniform_failed );
    sample_last_uniform_id = program.getUniform( "SampleLast", &std::cout, &uniform_failed );

    morph_attribute_array_last.addAttribute( "POSITION_Last", 3, GL_FLOAT, GL_FALSE, MORPH_BUFFER_SIZE, 0 );
    morph_attribute_array_last.addAttribute( "NORMAL_Last",   3, GL_FLOAT, GL_FALSE, MORPH_BUFFER_SIZE, (void*)(3 * sizeof( float )) );
    
    attribute_failed |= !program.isAttribute( "POSITION_Last", &std::cout );
    attribute_failed |= !program.isAttribute( "NORMAL_Last", &std::cout );

    morph_attribute_array_last.allocate( program );
    morph_attribute_array_last.cullUnfound();

    if( !uniform_failed && !attribute_failed )
        return ret;
    else
    {
        std::cout << "Morph Model Draw Error\n";
        std::cout << program.getInfoLog();
        std::cout << "\nVertex shader log\n";
        std::cout << vertex_shader.getInfoLog();
        std::cout << "\nFragment shader log\n";
        std::cout << fragment_shader.getInfoLog() << std::endl;
        return 0;
    }
}

int Graphics::SDL2::GLES2::Internal::MorphModelDraw::inputModel( Utilities::ModelBuilder *model_type_r, uint32_t obj_identifier, const std::map<uint32_t, Internal::Texture2D*>& textures ) {
    auto ret = Graphics::SDL2::GLES2::Internal::StaticModelDraw::inputModel( model_type_r, obj_identifier, textures );
    
    if( ret >= 0 )
    {
        GLsizei transparent_count = 0;

        auto accessor = models_p.find( obj_identifier );
        if( accessor != models_p.end() )
            transparent_count = (*accessor).second->transparent_triangles.size();
        
        if( transparent_count != 0 ) {
            if( model_animation_p[ obj_identifier ] != nullptr )
                delete model_animation_p[ obj_identifier ];
            
            model_animation_p[ obj_identifier ] = new Animation( model_type_r, transparent_count );
        }
    }
    
    return ret;
}

void Graphics::SDL2::GLES2::Internal::MorphModelDraw::clearModels() {
    StaticModelDraw::clearModels();

    // Delete the models first.
    for( auto i = model_animation_p.begin(); i != model_animation_p.end(); i++ )
    {
        delete (*i).second; // First delete the pointer.
        (*i).second = nullptr; // Then set the pointer to null.
    }
    model_animation_p.clear();
}

void Graphics::SDL2::GLES2::Internal::MorphModelDraw::draw( Graphics::SDL2::GLES2::Camera &camera ) {
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

    Animation::Dynamic dynamic;
    dynamic.camera_position = camera.getPosition();

    // Traverse the models.
    for( auto d = models_p.begin(); d != models_p.end(); d++ ) // Go through every model that has an instance.
    {
        // Get the mesh information.
        Graphics::SDL2::GLES2::Internal::Mesh *mesh_r = &( *d ).second->mesh;
        
        // Go through every instance that refers to this mesh.
        for( auto instance = ( *d ).second->instances_r.begin(); instance != ( *d ).second->instances_r.end(); instance++ )
        {
            if( camera.isVisible( *(*instance) ) ) {
                // Get the position and rotation of the model.
                // Multiply them into one matrix which will hold the entire model transformation.
                camera_3D_model_transform = glm::translate( glm::mat4(1.0f), (*instance)->getPosition() ) * glm::toMat4( (*instance)->getRotation() );

                // Then multiply it to the projection, and view to get projection, view, and model matrix.
                camera_3D_projection_view_model = camera_3D_projection_view * camera_3D_model_transform;

                // We can now send the matrix to the program.
                glUniformMatrix4fv( matrix_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &camera_3D_projection_view_model[0][0] ) );

                // TODO Find a cleaner way.
                model_view = view * camera_3D_model_transform;
                model_view_inv = glm::inverse( model_view );
                glUniformMatrix4fv(     view_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &model_view[0][0] ) );
                glUniformMatrix4fv( view_inv_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &model_view_inv[0][0] ) );

                // We now draw the the mesh!
                mesh_r->bindArray();

                int current_last_frame = static_cast<unsigned int>( floor( (*instance)->getTimeline() ) ) - 1;

                if( current_last_frame < 0 )
                {
                    current_last_frame = 0; // Next is unused.
                    glUniform1f( sample_last_uniform_id, 0.0f );
                }
                else
                    glUniform1f( sample_last_uniform_id, 1.0f );

                morph_attribute_array_last.bind( mesh_r->getMorphOffset( current_last_frame ) );

                mesh_r->noPreBindDrawOpaque( 0, diffusive_texture_uniform_id );
                
                auto accessor = model_animation_p.find( ( *d ).first );
                if( accessor != model_animation_p.end() ) {
                    dynamic.transform = camera_3D_model_transform;
                    dynamic.morph_info_r = (*accessor).second;
                    dynamic.frame_index = static_cast<unsigned int>( floor( (*instance)->getTimeline() ) );
                    dynamic.addTriangles( (*d).second->transparent_triangles, camera.transparent_triangles );
                }
            }
        }
    }
}

void Graphics::SDL2::GLES2::Internal::MorphModelDraw::advanceTime( float seconds_passed )
{
    const float FRAME_SPEED = 10.0;

    // Go through every model array.
    for( auto model_type = models_p.begin(); model_type != models_p.end(); model_type++ ) {
        // Get the mesh.
        Graphics::SDL2::GLES2::Internal::Mesh *mesh_r = &(*model_type).second->mesh;
        
        if( mesh_r->getMorphFrameAmount() > 0 )
        {
            auto morph_frame_amount = mesh_r->getMorphFrameAmount();
            auto total_frame_amount = morph_frame_amount + 1;

            // Go through every instance of the model.
            for( auto instance = (*model_type).second->instances_r.begin(); instance != (*model_type).second->instances_r.end(); instance++ ) {
                (*instance)->setTimeline( fmod( (*instance)->getTimeline() + seconds_passed * FRAME_SPEED, total_frame_amount ) );
            }
        }
    }
}
