#include "StaticModelDraw.h"
#include "../ModelInstance.h"
#include <algorithm>
#include <cassert>
#include <cmath> // fmod()
#include <iostream> // fmod()
#include "SDL.h"

const GLchar* Graphics::SDL2::GLES2::Internal::StaticModelDraw::default_es_vertex_shader =
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
const GLchar* Graphics::SDL2::GLES2::Internal::StaticModelDraw::default_vertex_shader =
    "#version 110\n"
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
const GLchar* Graphics::SDL2::GLES2::Internal::StaticModelDraw::default_es_fragment_shader =
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
const GLchar* Graphics::SDL2::GLES2::Internal::StaticModelDraw::default_fragment_shader =
    "#version 110\n"

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

Graphics::SDL2::GLES2::Internal::StaticModelDraw::StaticModelDraw() {

}

Graphics::SDL2::GLES2::Internal::StaticModelDraw::~StaticModelDraw() {
    // Delete the models first.
    for( auto i = models_p.begin(); i != models_p.end(); i++ )
    {
        delete (*i).second; // First delete the pointer.
        (*i).second = nullptr; // Then set the pointer to null.
    }
}

const GLchar* Graphics::SDL2::GLES2::Internal::StaticModelDraw::getDefaultVertexShader() {
    int opengl_profile;
    
    SDL_GL_GetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, &opengl_profile );

    if( (opengl_profile & SDL_GL_CONTEXT_PROFILE_ES) != 0 )
        return default_es_vertex_shader;
    else
        return default_vertex_shader;
}

const GLchar* Graphics::SDL2::GLES2::Internal::StaticModelDraw::getDefaultFragmentShader() {
    int opengl_profile;
    
    SDL_GL_GetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, &opengl_profile );

    if( (opengl_profile & SDL_GL_CONTEXT_PROFILE_ES) != 0 )
        return default_es_fragment_shader;
    else
        return default_fragment_shader;
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
    if( shiney_texture_r != nullptr )
        shiney_texture_r->bind( 1, sepecular_texture_uniform_id );

    // Traverse the models.
    for( auto d = models_p.begin(); d != models_p.end(); d++ ) // Go through every model that has an instance.
    {
        // Get the mesh information.
        Graphics::SDL2::GLES2::Internal::Mesh *mesh_r = &(*d).second->mesh;
        
        // Go through every instance that refers to this mesh.
        for( auto instance = (*d).second->instances_r.begin(); instance != (*d).second->instances_r.end(); instance++ )
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
            glUniformMatrix4fv( view_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &model_view[0][0] ) );
            glUniformMatrix4fv( view_inv_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &model_view_inv[0][0] ) );

            // Finally we can draw the mesh!
            mesh_r->draw( 0, diffusive_texture_uniform_id );
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
