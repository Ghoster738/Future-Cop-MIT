#include "SkeletalModelDraw.h"
#include "../ModelInstance.h"
#include <glm/gtc/type_ptr.hpp>
#include <cassert>
#include "SDL.h"
#include <iostream>

Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::SkeletalAnimation::SkeletalAnimation( unsigned int num_bones, unsigned int amount_of_frames ) {
    this->num_bones = num_bones;
    
    // set the joint matrix frames.
    bone_frames.resize( amount_of_frames * num_bones );
}

glm::mat4* Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::SkeletalAnimation::getFrames( unsigned int current_frame, unsigned int current_bone ) {
    if( current_frame < bone_frames.size() )
        return bone_frames.data() + current_frame * this->num_bones + current_bone;
    else
        return nullptr;
}

const GLchar* Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::default_es_vertex_shader =
    "precision mediump float;\n"
    // Inputs
    "attribute vec4 POSITION;\n"
    "attribute vec3 NORMAL;\n"
    "attribute vec2 TEXCOORD_0;\n"
    "attribute float _Specular;\n"
    // These attributes are for the skelatal animation.
    "attribute vec4 JOINTS_0;\n"
    "attribute vec3 WEIGHTS_0;\n"

    // Vertex shader uniforms
    "uniform mat4 ModelViewInv;\n"
    "uniform mat4 ModelView;\n"
    "uniform mat4 Transform;\n" // projection * view * model.
    "uniform vec2 TextureTranslation;\n"
    // These uniforms are for the bones of the animation.
    "uniform mat4 Bone[12];\n" // 12 bones seems to be the limit of Future Cop.

    // These are the outputs
    "varying vec3 world_reflection;\n"
    "varying float specular;\n"
    "varying vec2 texture_coord_1;\n"

    "void main()\n"
    "{\n"
    "   vec4 current_position = Bone[ int( JOINTS_0.x ) ] * POSITION;\n"
    "   vec3 current_normal   = NORMAL;\n"
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

const GLchar* Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::default_vertex_shader =
    // Inputs
    "attribute vec4 POSITION;\n"
    "attribute vec3 NORMAL;\n"
    "attribute vec2 TEXCOORD_0;\n"
    "attribute float _Specular;\n"
    // These attributes are for the skelatal animation.
    "attribute vec4 JOINTS_0;\n"
    "attribute vec3 WEIGHTS_0;\n"

    // Vertex shader uniforms
    "uniform mat4 ModelViewInv;\n"
    "uniform mat4 ModelView;\n"
    "uniform mat4 Transform;\n" // projection * view * model.
    "uniform vec2 TextureTranslation;\n"
    // These uniforms are for the bones of the animation.
    "uniform mat4 Bone[12];\n" // 12 bones seems to be the limit of Future Cop.

    // These are the outputs
    "varying vec3 world_reflection;\n"
    "varying float specular;\n"
    "varying vec2 texture_coord_1;\n"

    "void main()\n"
    "{\n"
    "   vec4 current_position = Bone[ int( JOINTS_0.x ) ] * POSITION;\n"
    "   vec3 current_normal   = NORMAL;\n"
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
Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::SkeletalModelDraw() {

}

Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::~SkeletalModelDraw() {
    // Delete the models first.
    for( auto i = model_animation_p.begin(); i != model_animation_p.end(); i++ )
    {
        delete (*i).second; // First delete the pointer.
        (*i).second = nullptr; // Then set the pointer to null.
    }
}

const GLchar* Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::getDefaultVertexShader() {
    int opengl_profile;
    
    SDL_GL_GetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, &opengl_profile );

    if( (opengl_profile & SDL_GL_CONTEXT_PROFILE_ES) != 0 )
        return default_es_vertex_shader;
    else
        return default_vertex_shader;
}

int Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::compileProgram() {
    auto ret = Graphics::SDL2::GLES2::Internal::StaticModelDraw::compileProgram();
    bool uniform_failed = false;
    bool attribute_failed = false;

    mat4_array_uniform_id = program.getUniform( "Bone", &std::cout, &uniform_failed );
    
    attribute_failed |= !program.isAttribute( "JOINTS_0", &std::cout );
    // attribute_failed |= !program.isAttribute( "WEIGHTS_0", &std::cout ); // WEIGHTS_0 is not a requirement.
    
    if( !uniform_failed && !attribute_failed )
        return ret;
    else
    {
        std::cout << "Skeletal Model Draw Error\n";
        std::cout << program.getInfoLog();
        std::cout << "\nVertex shader log\n";
        std::cout << vertex_shader.getInfoLog();
        std::cout << "\nFragment shader log\n";
        std::cout << fragment_shader.getInfoLog() << std::endl;
        return 0;
    }
}

int Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::inputModel( Utilities::ModelBuilder *model_type, uint32_t obj_identifier, const std::map<uint32_t, Internal::Texture2D*>& textures ) {
    auto ret = Graphics::SDL2::GLES2::Internal::StaticModelDraw::inputModel( model_type, obj_identifier, textures );
    
    if( ret >= 0 )
    {
        if( model_animation_p[ obj_identifier ] != nullptr )
            delete model_animation_p[ obj_identifier ];
        
        model_animation_p[ obj_identifier ] = new SkeletalAnimation( model_type->getNumJoints(), model_type->getNumJointFrames() );

        for( unsigned int frame_index = 0; frame_index < model_type->getNumJointFrames(); frame_index++ )
        {
            glm::mat4* frame_r = model_animation_p[ obj_identifier ]->getFrames( frame_index, 0 );

            for( unsigned int bone_index = 0; bone_index < model_type->getNumJoints(); bone_index++ )
            {
                frame_r[ bone_index ] = model_type->getJointFrame( frame_index, bone_index );
            }
        }

        models_p.at( obj_identifier )->mesh.setFrameAmount( model_type->getNumJointFrames() );
    }
    
    return ret;
}

void Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::draw( const Camera &camera ) {
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

    // Traverse the models.
    for( auto d = models_p.begin(); d != models_p.end(); d++ ) // Go through every model that has an instance.
    {
        // Get the mesh information.
        Graphics::SDL2::GLES2::Internal::Mesh *mesh_r = nullptr;
        SkeletalAnimation *animate_r = nullptr;
        
        if( models_p.find( ( *d ).first ) != models_p.end()  ) {
            mesh_r = &( *d ).second->mesh;
            animate_r = model_animation_p.at( ( *d ).first );
        }

        // Check if the mesh is a valid pointer.
        if( mesh_r != nullptr && animate_r != nullptr )
        {
            // Go through every instance that refers to this mesh.
            for( auto instance = ( *d ).second->instances_r.begin(); instance != ( *d ).second->instances_r.end(); instance++ )
            {
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

                int current_frame = static_cast<unsigned int>( floor( (*instance)->getTimeline() ) );

                assert( animate_r->getFrames( current_frame, 0 ) != nullptr );
                assert( animate_r->getFrames( current_frame, animate_r->getNumBones() - 1 ) != nullptr );

                glUniformMatrix4fv( mat4_array_uniform_id, animate_r->getNumBones(), GL_FALSE, glm::value_ptr( *animate_r->getFrames( current_frame, 0 ) ) );

                mesh_r->draw( 0, diffusive_texture_uniform_id );
            }
        }
    }
}

void Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::advanceTime( float seconds_passed )
{
    const float FRAME_SPEED = 10.0;

    // Go through every model array.
    for( auto model_type = models_p.begin(); model_type != models_p.end(); model_type++ ) {
        // Get the mesh.
        Graphics::SDL2::GLES2::Internal::Mesh *mesh_r = &(*model_type).second->mesh;
        
        if( mesh_r->getFrameAmount() > 0 ) {
            for( auto instance = (*model_type).second->instances_r.begin(); instance != (*model_type).second->instances_r.end(); instance++ ) {
                (*instance)->setTimeline( fmod( (*instance)->getTimeline() + seconds_passed * FRAME_SPEED, mesh_r->getFrameAmount() ) );
            }
        }
    }
}
