#include "MorphModelDraw.h"
#include "../../../ModelInstance.h"
#include <cassert>
#include <iostream>

namespace {
    const size_t MORPH_BUFFER_SIZE = (3 + 3) * sizeof( float );
}

const GLchar* Graphics::SDL2::GLES2::Internal::MorphModelDraw::default_vertex_shader =
    // Inputs
    "attribute vec4 POSITION;\n"
    "attribute vec3 NORMAL;\n"
    "attribute vec2 TEXCOORD_0;\n"
    "attribute float _Specular;\n"
    // These inputs are for the morph attributes
    "attribute vec4 POSITION_Next;\n"
    "attribute vec3 NORMAL_Next;\n"
    "attribute vec4 POSITION_Last;\n"
    "attribute vec3 NORMAL_Last;\n"

    // Vertex shader uniforms
    "uniform mat4 ModelViewInv;\n"
    "uniform mat4 ModelView;\n"
    "uniform mat4 Transform;\n" // projection * view * model.
    "uniform vec2 TextureTranslation;\n"
    // These uniforms are for modifiying the morph attributes
    "uniform float SampleNext;\n"
    "uniform float SampleLast;\n"

    // These are the outputs
    "varying vec3 world_reflection;\n"
    "varying float specular;\n"
    "varying vec2 texture_coord_1;\n"

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

}

Graphics::SDL2::GLES2::Internal::MorphModelDraw::~MorphModelDraw() {

}

int Graphics::SDL2::GLES2::Internal::MorphModelDraw::compilieProgram() {
    auto ret = Graphics::SDL2::GLES2::Internal::StaticModelDraw::compilieProgram();

    sample_next_uniform_id = glGetUniformLocation( program.getProgramID(), "SampleNext" );
    sample_last_uniform_id = glGetUniformLocation( program.getProgramID(), "SampleLast" );

    assert( sample_next_uniform_id > 0 );
    assert( sample_last_uniform_id > 0 );

    glUniform1f( sample_next_uniform_id, 0.0f ); // Next is unused.
    glUniform1f( sample_last_uniform_id, 1.0f );

    morph_attribute_array_last.addAttribute( "POSITION_Last", 3, GL_FLOAT, GL_FALSE, MORPH_BUFFER_SIZE, 0 );
    morph_attribute_array_last.addAttribute( "NORMAL_Last",   3, GL_FLOAT, GL_FALSE, MORPH_BUFFER_SIZE, (void*)(3 * sizeof( float )) );

    morph_attribute_array_last.allocate( program );
    morph_attribute_array_last.cullUnfound( &std::cout );

    return ret;
}

void Graphics::SDL2::GLES2::Internal::MorphModelDraw::draw( const Camera &camera ) {
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
                camera_3D_projection_view_model = camera_3D_projection_view * camera_3D_model_transform;

                // We can now send the matrix to the program.
                glUniformMatrix4fv( matrix_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &camera_3D_projection_view_model[0][0] ) );

                // TODO Find a cleaner way.
                model_view = view * camera_3D_model_transform;
                model_view_inv = glm::inverse( model_view );
                glUniformMatrix4fv(     view_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &model_view[0][0] ) );
                glUniformMatrix4fv( view_inv_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &model_view_inv[0][0] ) );

                // We now draw the the mesh!
                mesh->bindArray();

                int current_last_frame = static_cast<unsigned int>( floor( (*instance)->getTimeline() ) ) - 1;

                if( current_last_frame < 0 )
                {
                    current_last_frame = 0; // Next is unused.
                    glUniform1f( sample_last_uniform_id, 0.0f );
                }
                else
                    glUniform1f( sample_last_uniform_id, 1.0f );

                // std::cout << "getTimeline = " << (*instance)->getTimeline() << ", " << current_last_frame << ", offset = " << mesh->getMorphOffset( current_last_frame ) << std::endl;

                morph_attribute_array_last.bind( mesh->getMorphOffset( current_last_frame ) );

                mesh->noPreBindDraw( 0, diffusive_texture_uniform_id );
            }
        }
    }
}
