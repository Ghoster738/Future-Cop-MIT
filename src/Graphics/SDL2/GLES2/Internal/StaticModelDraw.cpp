#include "StaticModelDraw.h"
#include "../ModelInstance.h"
#include <algorithm>
#include <cassert>
#include <cmath> // fmod()
#include <iostream> // fmod()

const GLchar* Graphics::SDL2::GLES2::Internal::StaticModelDraw::default_vertex_shader =
    "#version 100\n"
    "precision mediump float;\n"
    // Inputs
    "attribute vec4 POSITION;\n"
    "attribute vec3 NORMAL;\n"
    "attribute vec2 TEXCOORD_0;\n"
    "attribute float _Specular;\n"

    // Vertex shader uniforms
    "uniform mat4 ModelViewInv;\n"
    "uniform mat4 ModelView;\n"
    "uniform mat4 Transform;\n" // projection * view * model.
    "uniform vec2 TextureTranslation;\n"

    // These are the outputs
    "varying vec3 world_reflection;\n"
    "varying float specular;\n"
    "varying vec2 texture_coord_1;\n"

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
    "#version 100\n"
    "precision mediump float;\n"

    "varying vec3 world_reflection;\n"
    "varying float specular;\n"
    "varying vec2 texture_coord_1;\n"

    "uniform sampler2D Texture;\n"
    "uniform sampler2D Shine;\n"

    "void main()\n"
    "{\n"
    "  vec4 color = texture2D(Texture, texture_coord_1);\n"
    "  const float CUTOFF = 0.015625;\n"
    "  float BLENDING = color.a;\n"
    "  if( color.r < CUTOFF && color.g < CUTOFF && color.b < CUTOFF )"
    "    discard;\n"
    "  if( color.a > 0.0125 )\n"
    "    color.a = 0.5;\n"
    "  else\n"
    "    color.a = 1.0;\n"
    "  if( specular > 0.5)\n"
    "    gl_FragColor = texture2D(Shine, world_reflection.xz) * BLENDING + vec4(color.rgb, 1.0);\n"
    "  else\n"
    "    gl_FragColor = color;\n"
    "}\n";

namespace {

bool sortModelArray(const Graphics::SDL2::GLES2::Internal::StaticModelDraw::ModelArray *i, const Graphics::SDL2::GLES2::Internal::StaticModelDraw::ModelArray *j) {
    return (i->mesh_index < j->mesh_index);
}

}

Graphics::SDL2::GLES2::Internal::StaticModelDraw::ModelArray* Graphics::SDL2::GLES2::Internal::StaticModelDraw::getModelArray( unsigned int mesh_index ) {
    ModelArray* search_key = nullptr;

    if( !model_array.empty() )
    {
        ModelArray relationModelArray;
        relationModelArray.mesh_index = mesh_index;

        auto bound = lower_bound( model_array.begin(), model_array.end(), &relationModelArray, sortModelArray );

        const int index = bound - model_array.begin();

        if( index < model_array.size() && model_array.at(index)->mesh_index == mesh_index )
            search_key = *bound;
    }

    return search_key;
}

Graphics::SDL2::GLES2::Internal::StaticModelDraw::ModelArray* Graphics::SDL2::GLES2::Internal::StaticModelDraw::getModelArray( unsigned int mesh_index ) const {
    ModelArray* search_key = nullptr;

    if( !model_array.empty() )
    {
        ModelArray relationModelArray;
        relationModelArray.mesh_index = mesh_index;

        auto bound = lower_bound( model_array.begin(), model_array.end(), &relationModelArray, sortModelArray );

        const int index = bound - model_array.begin();

        if( index < model_array.size() && model_array.at(index)->mesh_index == mesh_index )
            search_key = *bound;
    }

    return search_key;
}

Graphics::SDL2::GLES2::Internal::StaticModelDraw::ModelArray* Graphics::SDL2::GLES2::Internal::StaticModelDraw::addModelArray( unsigned int mesh_index ) {
    ModelArray *new_model_array = new ModelArray();

    new_model_array->mesh_index = mesh_index;
    new_model_array->unculled_size = 0;

    model_array.push_back( new_model_array );
    sort( model_array.begin(), model_array.end(), sortModelArray );

    return new_model_array;
}

Graphics::SDL2::GLES2::Internal::StaticModelDraw::StaticModelDraw() {

}

Graphics::SDL2::GLES2::Internal::StaticModelDraw::~StaticModelDraw() {

}

void Graphics::SDL2::GLES2::Internal::StaticModelDraw::setVertexShader( const GLchar *const shader_source ) {
    vertex_shader.setShader( Shader::TYPE::VERTEX, shader_source );
}

int Graphics::SDL2::GLES2::Internal::StaticModelDraw::loadVertexShader( const char *const file_path ) {
    return vertex_shader.loadShader( Shader::TYPE::VERTEX, file_path );
}

void Graphics::SDL2::GLES2::Internal::StaticModelDraw::setFragmentShader( const GLchar *const shader_source ) {
    fragment_shader.setShader( Shader::TYPE::FRAGMENT, shader_source );
}

int Graphics::SDL2::GLES2::Internal::StaticModelDraw::loadFragmentShader( const char *const file_path ) {
    return fragment_shader.loadShader( Shader::TYPE::FRAGMENT, file_path );
}

int Graphics::SDL2::GLES2::Internal::StaticModelDraw::compilieProgram() {
    // The two shaders should be allocated first.
    if( vertex_shader.getType() == Shader::TYPE::VERTEX && fragment_shader.getType() == Shader::TYPE::FRAGMENT ) {

        // Allocate the opengl program for the map.
        program.allocate();

        // Give the program these two shaders.
        program.setVertexShader( &vertex_shader );
        program.setFragmentShader( &fragment_shader );

        // Attempt to link the shader
        if( program.link() )
        {
            // Setup the uniforms for the map.
            diffusive_texture_uniform_id = glGetUniformLocation( program.getProgramID(), "Texture" );
            sepecular_texture_uniform_id = glGetUniformLocation( program.getProgramID(), "Shine" );
            texture_offset_uniform_id    = glGetUniformLocation( program.getProgramID(), "TextureTranslation" );

            matrix_uniform_id = glGetUniformLocation(   program.getProgramID(), "Transform" );
            view_uniform_id = glGetUniformLocation(     program.getProgramID(), "ModelView" );
            view_inv_uniform_id = glGetUniformLocation( program.getProgramID(), "ModelViewInv" );

            return 1;
        }
        else
        {
            std::cout << "StaticModelDraw program has failed to compile" << std::endl;
            std::cout << program.getInfoLog() << std::endl;
            std::cout << "Vertex shader log" << std::endl;
            std::cout << vertex_shader.getInfoLog() << std::endl;
            std::cout << "Fragment shader log" << std::endl;
            std::cout << fragment_shader.getInfoLog() << std::endl;
            return 0;
        }
    }
    else
    {
        return 0; // Not every shader was loaded.
    }
}

void Graphics::SDL2::GLES2::Internal::StaticModelDraw::setTextures( Texture2D *shiney_texture_r ) {
    this->shiney_texture_ref = shiney_texture_r;
}

void Graphics::SDL2::GLES2::Internal::StaticModelDraw::setNumModelTypes( size_t model_amount ) {
    models.resize( model_amount, nullptr );
}

bool Graphics::SDL2::GLES2::Internal::StaticModelDraw::containsModel( size_t model_index ) const {
    if( model_index < models.size() )
        return (models[model_index] != nullptr);
    else
        return false;
}

int Graphics::SDL2::GLES2::Internal::StaticModelDraw::inputModel( Utilities::ModelBuilder *model_type, int index, const std::map<uint32_t, Internal::Texture2D*>& textures ) {
    int state = 0;

    if( model_type->getNumVertices() > 0 )
    {
        models.at( index ) = new Graphics::SDL2::GLES2::Internal::Mesh( &program );
        models[ index ]->setup( *model_type, textures );
        state =  1;
    }
    else
        state = -1;

    return state;
}

void Graphics::SDL2::GLES2::Internal::StaticModelDraw::draw( const Graphics::Camera &camera ) {
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
    if( shiney_texture_ref != nullptr )
        shiney_texture_ref->bind( 1, sepecular_texture_uniform_id );

    // Traverse the models.
    for( unsigned int d = 0; d < model_array.size(); d++ ) // Go through every model that has an instance.
    {
        // Get the mesh information.
        Graphics::SDL2::GLES2::Internal::Mesh *mesh = models.at( model_array.at( d )->mesh_index );

        // Check if the mesh is a valid pointer.
        if( mesh != nullptr )
        {
            // Go through every instance that refers to this mesh.
            for( auto instance = model_array[ d ]->instances.begin(); instance != model_array[ d ]->instances.end(); instance++ )
            {
                // Get the position and rotation of the model.
                // Multiply them into one matrix which will hold the entire model transformation.
                camera_3D_model_transform = glm::translate( glm::mat4(1.0f), (*instance)->getPosition() ) * glm::toMat4( (*instance)->getRotation() );

                // Then multiply it to the projection, and view to get projection, view, and model matrix.
                camera_3D_projection_view_model = camera_3D_projection_view * (glm::translate( glm::mat4(1.0f), (*instance)->getPosition() ) * glm::toMat4( (*instance)->getRotation() ));

                // We can now send the matrix to the program.
                glUniformMatrix4fv( matrix_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &camera_3D_projection_view_model[0][0] ) );

                model_view = view * camera_3D_model_transform;
                model_view_inv = glm::inverse( model_view );
                glUniformMatrix4fv(     view_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &model_view[0][0] ) );
                glUniformMatrix4fv( view_inv_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &model_view_inv[0][0] ) );

                // Finally we can draw the mesh!
                mesh->draw( 0, diffusive_texture_uniform_id );
            }
        }
    }
}

int Graphics::SDL2::GLES2::Internal::StaticModelDraw::prune() {
    int count_deleted = 0;

    for( auto model_type = model_array.begin(); model_type < model_array.end(); model_type++ ) {
        if( (*model_type)->instances.size() != (*model_type)->unculled_size )
        {
            count_deleted += ((*model_type)->instances.size() - (*model_type)->unculled_size);
            (*model_type)->instances.resize( (*model_type)->unculled_size );

            // There should not be a negative count.
            assert( ((*model_type)->instances.size() - (*model_type)->unculled_size) <= 0 );
        }
    }

    return count_deleted;
}

int Graphics::SDL2::GLES2::Internal::StaticModelDraw::allocateObjModel( unsigned int index_obj, Graphics::ModelInstance &model_instance ) {
    if( index_obj < models.size() ) // Do some bounds checking!
    {
        // This holds the model instance sheet.
        ModelArray *model_array = getModelArray( index_obj );

        if( model_array == nullptr ) // Check if the mesh was actually loaded.
            model_array = addModelArray( index_obj );

        auto internal_data = reinterpret_cast<Graphics::SDL2::GLES2::ModelInternalData*>( model_instance.getInternalData() );
        internal_data->array = model_array;
        internal_data->index_position = model_array->instances.size();
        
        bool result = false;
        
        if( models[ index_obj ] != nullptr ) {
            result = models[ index_obj ]->getBoundingSphere( internal_data->culling_sphere_position, internal_data->culling_sphere_radius );
        }
        // assert( result ); // TODO Add a return false case to getBoundingSphere.

        // Finally added the instance.
        model_array->instances.push_back( &model_instance );

        // Model Array should have the new size.
        model_array->unculled_size++;

        return 1; // The instance is successfully allocated.
    }
    else
        return -1; // The requested index_obj does not exist
}

void Graphics::SDL2::GLES2::Internal::StaticModelDraw::advanceTime( float time_seconds ) {
    const float FRAME_SPEED = 10.0;

    // Go through every model array.
    for( auto model_type = model_array.begin(); model_type < model_array.end(); model_type++ ) {

        // Get the mesh.
        Graphics::SDL2::GLES2::Internal::Mesh *mesh = models.at( (*model_type)->mesh_index  );

        // Test to see if the mesh has an animation to it.
        if( mesh != nullptr )
        {
            if( mesh->getMorphFrameAmount() > 0 )
            {
                auto morph_frame_amount = mesh->getMorphFrameAmount();
                auto total_frame_amount = morph_frame_amount + 1;

                // Go through every instance of the model.
                for( auto instance = (*model_type)->instances.begin(); instance != (*model_type)->instances.end(); instance++ ) {
                    (*instance)->setTimeline( fmod( (*instance)->getTimeline() + time_seconds * FRAME_SPEED, total_frame_amount ) );
                }
            }
            else
            if( mesh->getFrameAmount() > 0 ) {
                for( auto instance = (*model_type)->instances.begin(); instance != (*model_type)->instances.end(); instance++ ) {
                    (*instance)->setTimeline( fmod( (*instance)->getTimeline() + time_seconds * FRAME_SPEED, mesh->getFrameAmount() ) );
                }
            }
        }
    }
}

bool Graphics::SDL2::GLES2::Internal::StaticModelDraw::getBoundingSphere( unsigned int mesh_index, glm::vec3 &position, float &radius ) const {
    auto model_r = this->models.at( mesh_index );
    
    if( model_r == nullptr )
        return false;
    
    return model_r->getBoundingSphere( position, radius );
}
