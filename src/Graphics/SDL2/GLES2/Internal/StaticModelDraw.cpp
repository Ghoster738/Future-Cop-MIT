#include "StaticModelDraw.h"
#include "../ModelInstance.h"

#include <algorithm>
#include <cassert>
#include <cmath> // fmod()
#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include "SDL.h"


void Graphics::SDL2::GLES2::Internal::StaticModelDraw::ModelArray::bindUVAnimation(GLuint animated_uv_frames_id, unsigned int time, std::vector<glm::vec2>& uv_frame_buffer) const {
    for(auto i = uv_animation_info.cbegin(); i != uv_animation_info.cend(); i++) {
        const uint_fast32_t duration = time % (*i).getEntireDurationUnits();
        const uint_fast32_t frame_index = ((*i).number_of_frames - 1) - (duration / (*i).frame_duration);
        
        const size_t index = 4 * (i - uv_animation_info.cbegin());
        const size_t uv_data_index = (*i).uv_data_offset / sizeof(glm::u8vec2) + 4 * frame_index;

        uv_frame_buffer[index + 0] = glm::vec2(uv_animation_data[uv_data_index + 0].x * 1.0 / 256.0, uv_animation_data[uv_data_index + 0].y * 1.0 / 256.0);
        uv_frame_buffer[index + 1] = glm::vec2(uv_animation_data[uv_data_index + 1].x * 1.0 / 256.0, uv_animation_data[uv_data_index + 1].y * 1.0 / 256.0);
        uv_frame_buffer[index + 2] = glm::vec2(uv_animation_data[uv_data_index + 2].x * 1.0 / 256.0, uv_animation_data[uv_data_index + 2].y * 1.0 / 256.0);
        uv_frame_buffer[index + 3] = glm::vec2(uv_animation_data[uv_data_index + 3].x * 1.0 / 256.0, uv_animation_data[uv_data_index + 3].y * 1.0 / 256.0);
    }

    if(uv_frame_buffer.size() != 0)
        glUniform2fv( animated_uv_frames_id, uv_frame_buffer.size(), reinterpret_cast<float*>(glm::value_ptr(uv_frame_buffer[0])) );
    else
        glUniform2f( animated_uv_frames_id, 0, 0);
}

void Graphics::SDL2::GLES2::Internal::StaticModelDraw::Dynamic::addTriangles(
            const std::vector<DynamicTriangleDraw::Triangle> &triangles,
            DynamicTriangleDraw::DrawCommand &triangles_draw ) const
{
    DynamicTriangleDraw::Triangle *draw_triangles_r;

    size_t number_of_triangles = triangles_draw.getTriangles( triangles.size(), &draw_triangles_r );

    for( size_t i = 0; i < number_of_triangles; i++ ) {
        draw_triangles_r[ i ] = triangles[ i ];

        for( unsigned t = 0; t < 3; t++ ) {
            const uint16_t texture_animation_data = static_cast<uint16_t>(draw_triangles_r[ i ].vertices[ t ].vertex_metadata[1]);

            if(texture_animation_data != 0) {
                const uint16_t texture_animation_index = texture_animation_data - 1;

                assert(texture_animation_index < uv_frame_buffer_r->size());

                draw_triangles_r[ i ].vertices[ t ].coordinate = (*uv_frame_buffer_r)[texture_animation_index];
            }

            draw_triangles_r[ i ].vertices[ t ].coordinate += texture_offset;
        }

        draw_triangles_r[ i ] = draw_triangles_r[ i ].addTriangle( this->camera_position, transform );
    }
}

const size_t Graphics::SDL2::GLES2::Internal::StaticModelDraw::UV_FRAME_BUFFER_SIZE_LIMIT = 8 * 4;
const GLchar* Graphics::SDL2::GLES2::Internal::StaticModelDraw::default_vertex_shader =
    "const int ANIMATED_UV_FRAME_AMOUNT = 8;\n"
    "const int QUAD_VERTEX_AMOUNT = 4;\n"
    "const int ANIMATED_UV_FRAME_VEC_AMOUNT = ANIMATED_UV_FRAME_AMOUNT * QUAD_VERTEX_AMOUNT;\n"

    // Vertex shader uniforms
    "uniform mat4 ModelViewInv;\n"
    "uniform mat4 ModelView;\n"
    "uniform mat4 Transform;\n" // projection * view * model.
    "uniform vec2 TextureTranslation;\n"
    "uniform vec2 AnimatedUVFrames[ ANIMATED_UV_FRAME_VEC_AMOUNT ];\n"

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
    "   specular = _METADATA[0];\n"
    "   texture_coord_1 = TEXCOORD_0 * float( _METADATA[1] == 0. );\n"
    "   texture_coord_1 += AnimatedUVFrames[ int( clamp( _METADATA[1] - 1., 0., float(ANIMATED_UV_FRAME_VEC_AMOUNT) ) ) ] * float( _METADATA[1] != 0. );\n"
    "   texture_coord_1 += TextureTranslation;\n"
    "   in_color = COLOR_0;\n"
    "   gl_Position = Transform * vec4(POSITION.xyz, 1.0);\n"
    "}\n";
const GLchar* Graphics::SDL2::GLES2::Internal::StaticModelDraw::default_fragment_shader =
    "uniform sampler2D Texture;\n"
    "uniform sampler2D Shine;\n"

    "void main()\n"
    "{\n"
    "  vec4 color = texture2D(Texture, texture_coord_1) * in_color;\n"
    "  float BLENDING = 1.0 - color.a;\n"
    "  if( specular > 0.5 )\n"
    "    gl_FragColor = texture2D(Shine, world_reflection.xz) * BLENDING + vec4(color.rgb, 1.0);\n"
    "  else {\n"
    "    if( color.a < 0.015625 )\n"
    "      discard;\n"
    "    gl_FragColor = color;\n"
    "  }\n"
    "}\n";

Graphics::SDL2::GLES2::Internal::StaticModelDraw::StaticModelDraw() {
    shiney_texture_r = nullptr;

    attributes.push_back( Shader::Attribute( Shader::Type::MEDIUM, "vec4 POSITION" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "vec3 NORMAL" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "vec4 COLOR_0" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "vec2 TEXCOORD_0" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "vec2 _METADATA" ) );

    varyings.push_back( Shader::Varying( Shader::Type::LOW,    "vec4 in_color" ) );
    varyings.push_back( Shader::Varying( Shader::Type::LOW,    "vec3 world_reflection" ) );
    varyings.push_back( Shader::Varying( Shader::Type::MEDIUM, "float specular" ) );
    varyings.push_back( Shader::Varying( Shader::Type::LOW,    "vec2 texture_coord_1" ) );
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
            specular_texture_uniform_id = program.getUniform( "Shine", &std::cout, &uniform_failed );
            texture_offset_uniform_id = program.getUniform( "TextureTranslation", &std::cout, &uniform_failed );
            matrix_uniform_id = program.getUniform( "Transform", &std::cout, &uniform_failed );
            view_uniform_id = program.getUniform( "ModelView", &std::cout, &uniform_failed );
            view_inv_uniform_id = program.getUniform( "ModelViewInv", &std::cout, &uniform_failed );
            animated_uv_frames_id = program.getUniform( "AnimatedUVFrames", &std::cout, &uniform_failed );
            
            attribute_failed |= !program.isAttribute( "POSITION", &std::cout );
            attribute_failed |= !program.isAttribute( "NORMAL", &std::cout );
            attribute_failed |= !program.isAttribute( "COLOR_0", &std::cout );
            attribute_failed |= !program.isAttribute( "TEXCOORD_0", &std::cout );
            attribute_failed |= !program.isAttribute( "_METADATA", &std::cout );

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

void Graphics::SDL2::GLES2::Internal::StaticModelDraw::setEnvironmentTexture( Texture2D *env_texture_ref ) {
    this->shiney_texture_r = env_texture_ref;
}

bool Graphics::SDL2::GLES2::Internal::StaticModelDraw::containsModel( uint32_t obj_identifier ) const {
    if( models_p.find( obj_identifier ) != models_p.end() )
        return ( models_p.at( obj_identifier ) != nullptr );
    else
        return false;
}

bool Graphics::SDL2::GLES2::Internal::StaticModelDraw::containsBBModel( uint32_t obj_identifier ) const {
    if( bounding_boxes_p.find( obj_identifier ) != bounding_boxes_p.end() )
        return ( bounding_boxes_p.at( obj_identifier ) != nullptr );
    else
        return false;
}

int Graphics::SDL2::GLES2::Internal::StaticModelDraw::inputModel( Utilities::ModelBuilder *model_type_r, uint32_t obj_identifier, const std::map<uint32_t, Internal::Texture2D*>& textures, const std::vector<Data::Mission::ObjResource::FaceOverrideType>& face_override_animation, const std::vector<glm::u8vec2>& face_override_uvs ) {
    int state = 0;

    if( model_type_r->getNumVertices() > 0 )
    {
        VertexAttributeArray vertex_array;

        vertex_array.addAttribute("NORMAL", 3, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
        vertex_array.addAttribute("COLOR_0", 4, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        vertex_array.addAttribute("TEXCOORD_0", 2, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
        vertex_array.addAttribute("_METADATA", 2, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

        models_p[ obj_identifier ] = new ModelArray( &program );
        models_p[ obj_identifier ]->mesh.setup( *model_type_r, textures, &vertex_array );
        state =  1;

        Utilities::ModelBuilder::TextureMaterial material;
        GLsizei transparent_count = 0;

        for( unsigned a = 0; a < model_type_r->getNumMaterials(); a++ ) {
            model_type_r->getMaterial( a, material );

            GLsizei addition_index = std::min( material.count, material.addition_index );
            GLsizei mix_index = std::min( material.count, material.mix_index );
            GLsizei transparent_index = std::min( mix_index, addition_index );

            transparent_count += material.count - transparent_index;
        }
        models_p[ obj_identifier ]->transparent_triangles.reserve( transparent_count );
        models_p[ obj_identifier ]->uv_animation_data = face_override_uvs;
        models_p[ obj_identifier ]->uv_animation_info = face_override_animation;

        const size_t face_override_amount = 4 * face_override_animation.size();

        if(uv_frame_buffer.size() < std::max(face_override_amount, UV_FRAME_BUFFER_SIZE_LIMIT) )
            uv_frame_buffer.resize( std::max(face_override_amount, UV_FRAME_BUFFER_SIZE_LIMIT) );

        GLsizei material_count = 0;

        unsigned   position_compenent_index = model_type_r->getNumVertexComponents();
        unsigned     normal_compenent_index = position_compenent_index;
        unsigned      color_compenent_index = position_compenent_index;
        unsigned coordinate_compenent_index = position_compenent_index;
        unsigned   metadata_compenent_index = position_compenent_index;

        Utilities::ModelBuilder::VertexComponent element("EMPTY");
        for( unsigned i = 0; model_type_r->getVertexComponent( i, element ); i++ ) {
            auto name = element.getName();

            if( name == Utilities::ModelBuilder::POSITION_COMPONENT_NAME )
                position_compenent_index = i;
            if( name == Utilities::ModelBuilder::NORMAL_COMPONENT_NAME )
                normal_compenent_index = i;
            if( name == Utilities::ModelBuilder::COLORS_0_COMPONENT_NAME )
                color_compenent_index = i;
            if( name == Utilities::ModelBuilder::TEX_COORD_0_COMPONENT_NAME )
                coordinate_compenent_index = i;
            if( name == Data::Mission::ObjResource::METADATA_COMPONENT_NAME )
                metadata_compenent_index = i;
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

            unsigned addition_index = std::min( material.count, material.addition_index );
            unsigned mix_index = std::min( material.count, material.mix_index );
            unsigned transparent_index = std::min( mix_index, addition_index );

            DynamicTriangleDraw::PolygonType polygon_type;

            glm::vec4   position = glm::vec4(0, 0, 0, 1);
            glm::vec4     normal = glm::vec4(0, 0, 0, 1);
            glm::vec4      color = glm::vec4(1, 1, 1, 1);
            glm::vec4 coordinate = glm::vec4(0, 0, 0, 1);
            glm::vec4   metadata = glm::vec4(0, 0, 0, 0);

            const unsigned vertex_per_triangle = 3;

            for( unsigned m = transparent_index; m < material.count; m += vertex_per_triangle ) {
                DynamicTriangleDraw::Triangle triangle;

                for(unsigned t = 0; t < 3; t++) {
                    model_type_r->getTransformation(   position,   position_compenent_index, material_count + m + t );
                    model_type_r->getTransformation(     normal,     normal_compenent_index, material_count + m + t );
                    model_type_r->getTransformation(      color,      color_compenent_index, material_count + m + t );
                    model_type_r->getTransformation( coordinate, coordinate_compenent_index, material_count + m + t );
                    model_type_r->getTransformation(   metadata,   metadata_compenent_index, material_count + m + t );

                    triangle.vertices[t].position = { position.x, position.y, position.z };
                    triangle.vertices[t].normal = normal;
                    triangle.vertices[t].color = color;
                    triangle.vertices[t].coordinate = coordinate;
                    triangle.vertices[t].vertex_metadata = metadata;

                    if(face_override_amount != 0 && static_cast<uint16_t>(triangle.vertices[t].vertex_metadata[1]) > face_override_amount) {
                        for(unsigned a = 0; a < t + 1; a++)
                            std::cout << "i[" << a << "] = " << triangle.vertices[a].vertex_metadata[1] << "\n";
                        std::cout << "face_override_amount = " << face_override_amount << std::endl;
                        assert(false);
                    }
                }

                if( m < mix_index )
                    polygon_type = DynamicTriangleDraw::PolygonType::ADDITION;
                else
                    polygon_type = DynamicTriangleDraw::PolygonType::MIX;

                triangle.setup( cbmp_id, glm::vec3(0, 0, 0), polygon_type );

                models_p[ obj_identifier ]->transparent_triangles.push_back( triangle );
            }

            material_count += material.count;
        }
    }
    else
        state = -1;

    return state;
}

int Graphics::SDL2::GLES2::Internal::StaticModelDraw::inputBoundingBoxes( Utilities::ModelBuilder *model_type_r, uint32_t obj_identifier, const std::map<uint32_t, Internal::Texture2D*>& textures ) {
    int state = 0;

    if( model_type_r->getNumVertices() > 0 ) {
        VertexAttributeArray vertex_array;

        vertex_array.addAttribute("NORMAL", 3, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
        vertex_array.addAttribute("COLOR_0", 4, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        vertex_array.addAttribute("TEXCOORD_0", 2, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
        vertex_array.addAttribute("_METADATA", 2, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

        bounding_boxes_p[ obj_identifier ] = new ModelArray( &program );
        bounding_boxes_p[ obj_identifier ]->mesh.setup( *model_type_r, textures, &vertex_array );

        state =  1;
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
    // Simiualar, but different.
    for( auto i = bounding_boxes_p.begin(); i != bounding_boxes_p.end(); i++ )
    {
        delete (*i).second;
        (*i).second = nullptr;
    }
    bounding_boxes_p.clear();
}

void Graphics::SDL2::GLES2::Internal::StaticModelDraw::draw( Graphics::SDL2::GLES2::Camera &camera, std::map<uint32_t, ModelArray*> &model_array_p ) {
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
        shiney_texture_r->bind( 1, specular_texture_uniform_id );

    Dynamic dynamic;
    dynamic.camera_position = camera.getPosition();

    // Traverse the models.
    for( auto d = model_array_p.begin(); d != model_array_p.end(); d++ ) // Go through every model that has an instance.
    {
        // Get the mesh information.
        Graphics::SDL2::GLES2::Internal::Mesh *mesh_r = &(*d).second->mesh;
        
        // Go through every instance that refers to this mesh.
        for( auto instance = (*d).second->instances_r.begin(); instance != (*d).second->instances_r.end(); instance++ )
        {
            if( camera.isVisible( *(*instance) ) ) {
                const auto texture_offset = (*instance)->getTextureOffset();
                
                if(&models_p == &model_array_p) {
                    glUniform2f( this->texture_offset_uniform_id, texture_offset.x, texture_offset.y );
                    (*d).second->bindUVAnimation(this->animated_uv_frames_id, (*instance)->getTextureTransformTimeline(), this->uv_frame_buffer);
                }
                else {
                    glUniform2f( this->texture_offset_uniform_id, 0, 0 );
                    glUniform2f( this->animated_uv_frames_id, 0, 0 );
                }

                // Get the position and rotation of the model.
                // Multiply them into one matrix which will hold the entire model transformation.
                camera_3D_model_transform = glm::translate( glm::mat4(1.0f), (*instance)->getPosition() ) * glm::mat4_cast( (*instance)->getRotation() );
                
                // Then multiply it to the projection, and view to get projection, view, and model matrix.
                camera_3D_projection_view_model = camera_3D_projection_view * (glm::translate( glm::mat4(1.0f), (*instance)->getPosition() ) * glm::mat4_cast( (*instance)->getRotation() ));
                
                // We can now send the matrix to the program.
                glUniformMatrix4fv( matrix_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &camera_3D_projection_view_model[0][0] ) );
                
                model_view = view * camera_3D_model_transform;
                model_view_inv = glm::inverse( model_view );
                glUniformMatrix4fv( view_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &model_view[0][0] ) );
                glUniformMatrix4fv( view_inv_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &model_view_inv[0][0] ) );
                
                // Finally we can draw the mesh!
                mesh_r->drawOpaque( 0, diffusive_texture_uniform_id );
                
                dynamic.texture_offset = texture_offset;
                dynamic.uv_frame_buffer_r = &this->uv_frame_buffer;
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

int Graphics::SDL2::GLES2::Internal::StaticModelDraw::allocateObjBBModel( uint32_t obj_identifier, GLES2::ModelInstance &model_instance ) {
    if( bounding_boxes_p.find( obj_identifier ) != bounding_boxes_p.end() ) // Do some bounds checking!
    {
        // This holds the model instance sheet.
        ModelArray *model_array_r = bounding_boxes_p[ obj_identifier ];

        model_instance.bb_array_r = model_array_r;

        if( !bounding_boxes_p[ obj_identifier ]->mesh.getBoundingSphere( model_instance.culling_sphere_position, model_instance.culling_sphere_radius ) )
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

void Graphics::SDL2::GLES2::Internal::StaticModelDraw::advanceTime( float seconds_passed ) {
    static std::string ticks_string = "const unsigned ticks[] = {";
    static std::string times_string = "const float times[] = {";
    static float time_passed = 0;
    const static float time_scale = 4.0;
    
    bool first = false;
    
    // Go through every model array.
    for( auto model_type = models_p.begin(); model_type != models_p.end(); model_type++ ) {
        for( auto instance = (*model_type).second->instances_r.begin(); instance != (*model_type).second->instances_r.end(); instance++ ) {
            (*instance)->addTextureTransformTimelineSeconds( seconds_passed );
            
            if(!first && time_passed < time_scale) {
                ticks_string += std::to_string((*instance)->texture_transform_timeline);
            }
            
            first = true;
        }
    }
    
    if(time_passed < time_scale) {
        times_string += std::to_string(seconds_passed);
        
        if(time_passed + seconds_passed >= time_scale)
            std::cout << times_string << "};\n" << ticks_string << "};" << std::endl;
        else {
            ticks_string += ", ";
            times_string += ", ";
        }
    }
    time_passed += seconds_passed;
}

bool Graphics::SDL2::GLES2::Internal::StaticModelDraw::getBoundingSphere( uint32_t obj_identifier, glm::vec3 &position, float &radius ) const {
    if( models_p.find( obj_identifier ) != models_p.end() )
        return false;
    return models_p.at( obj_identifier )->mesh.getBoundingSphere( position, radius );
}

/*
 const float times[] = {0.000000, 0.345506, 0.062126, 0.004702, 0.005930, 0.004484, 0.005249, 0.004432, 0.007193, 0.004118, 0.003835, 0.006508, 0.003940, 0.004251, 0.006268, 0.003840, 0.004147, 0.004201, 0.005783, 0.003782, 0.004153, 0.004188, 0.004299, 0.003933, 0.004151, 0.004004, 0.004273, 0.003799, 0.004086, 0.003974, 0.004256, 0.004047, 0.004218, 0.003904, 0.004223, 0.004229, 0.004164, 0.004263, 0.005651, 0.003913, 0.004269, 0.004049, 0.004891, 0.003537, 0.004150, 0.004015, 0.004100, 0.003808, 0.005492, 0.006580, 0.004264, 0.004162, 0.004374, 0.004354, 0.003943, 0.003924, 0.004034, 0.004388, 0.003717, 0.004082, 0.004111, 0.004084, 0.003844, 0.004121, 0.004227, 0.005018, 0.003635, 0.003960, 0.003964, 0.004730, 0.003595, 0.004368, 0.003674, 0.005214, 0.003742, 0.003714, 0.004133, 0.005093, 0.003533, 0.003984, 0.003987, 0.004434, 0.003794, 0.004195, 0.003921, 0.005243, 0.003632, 0.003898, 0.003973, 0.005558, 0.003603, 0.004092, 0.003976, 0.005194, 0.003630, 0.004008, 0.003961, 0.004089, 0.003992, 0.004021, 0.004037, 0.004847, 0.003535, 0.004013, 0.004001, 0.005765, 0.003650, 0.003941, 0.003950, 0.005096, 0.003683, 0.004007, 0.004021, 0.005364, 0.003643, 0.003994, 0.003896, 0.004857, 0.003693, 0.003914, 0.003875, 0.005648, 0.003666, 0.003837, 0.004037, 0.004612, 0.003627, 0.003962, 0.003991, 0.005700, 0.003676, 0.004074, 0.003986, 0.004021, 0.003748, 0.003928, 0.004063, 0.005314, 0.003614, 0.003936, 0.004018, 0.006380, 0.003704, 0.003794, 0.004138, 0.004287, 0.003761, 0.004213, 0.004009, 0.005519, 0.003765, 0.004160, 0.004196, 0.004203, 0.003616, 0.004030, 0.004110, 0.005528, 0.003696, 0.004152, 0.004028, 0.004794, 0.003819, 0.003928, 0.003966, 0.004905, 0.004099, 0.003949, 0.004040, 0.005114, 0.003827, 0.003977, 0.007473, 0.003889, 0.004091, 0.004037, 0.004044, 0.004069, 0.004004, 0.004034, 0.006176, 0.003591, 0.004166, 0.004102, 0.004124, 0.003920, 0.004152, 0.004214, 0.004055, 0.003905, 0.004025, 0.004044, 0.005469, 0.003580, 0.004014, 0.003948, 0.004744, 0.003533, 0.003886, 0.003948, 0.005438, 0.003579, 0.003945, 0.003863, 0.004786, 0.003593, 0.003978, 0.004082, 0.004680, 0.003623, 0.004206, 0.003626, 0.005373, 0.003579, 0.003994, 0.003946, 0.004933, 0.003602, 0.004300, 0.003656, 0.004512, 0.003572, 0.003964, 0.003836, 0.005961, 0.003655, 0.004011, 0.003943, 0.004525, 0.003627, 0.003956, 0.003829, 0.005616, 0.003598, 0.004333, 0.003573, 0.004075, 0.003718, 0.004018, 0.003891, 0.005322, 0.003541, 0.003862, 0.003900, 0.005347, 0.003586, 0.003974, 0.003873, 0.004605, 0.003613, 0.004279, 0.003825, 0.004971, 0.003633, 0.003954, 0.003881, 0.005730, 0.003547, 0.004123, 0.003868, 0.004505, 0.003559, 0.003883, 0.003791, 0.005834, 0.003586, 0.004029, 0.003909, 0.005399, 0.003559, 0.003945, 0.003918, 0.005082, 0.003538, 0.004012, 0.003963, 0.005054, 0.003546, 0.003986, 0.003924, 0.005359, 0.003562, 0.003888, 0.003979, 0.005142, 0.003523, 0.004068, 0.003928, 0.013639, 0.014159, 0.004044, 0.004203, 0.008263, 0.003978, 0.004040, 0.004268, 0.004033, 0.004009, 0.004265, 0.004230, 0.004070, 0.004122, 0.004260, 0.004074, 0.003843, 0.004158, 0.004068, 0.004074, 0.004253, 0.003967, 0.004050, 0.004243, 0.003930, 0.004006, 0.004096, 0.003982, 0.004089, 0.003859, 0.004299, 0.003598, 0.004105, 0.003787, 0.004100, 0.003910, 0.003937, 0.003968, 0.004011, 0.004038, 0.004044, 0.004338, 0.004009, 0.003923, 0.003945, 0.003962, 0.004057, 0.003952, 0.004062, 0.003930, 0.004051, 0.003950, 0.004021, 0.003891, 0.003980, 0.004003, 0.004266, 0.003565, 0.004075, 0.003928, 0.004333, 0.003663, 0.003986, 0.003960, 0.003930, 0.004033, 0.003728, 0.003996, 0.003967, 0.003823, 0.003889, 0.004001, 0.003932, 0.003964, 0.003868, 0.004025, 0.003913, 0.004057, 0.003965, 0.004032, 0.003923, 0.004039, 0.003887, 0.004015, 0.004028, 0.004047, 0.004042, 0.003971, 0.004026, 0.004050, 0.004061, 0.004049, 0.004075, 0.003946, 0.004056, 0.003971, 0.004086, 0.004000, 0.004123, 0.003972, 0.004068, 0.003937, 0.003822, 0.004207, 0.004026, 0.003965, 0.004491, 0.003670, 0.004108, 0.003820, 0.004178, 0.003754, 0.003920, 0.003838, 0.003901, 0.003955, 0.004079, 0.003866, 0.004136, 0.003887, 0.003978, 0.003975, 0.003897, 0.003976, 0.003945, 0.003855, 0.004138, 0.004199, 0.003666, 0.003984, 0.003961, 0.004001, 0.003980, 0.003950, 0.003943, 0.004062, 0.003933, 0.004028, 0.003961, 0.004044, 0.003967, 0.004008, 0.004033, 0.003952, 0.004085, 0.003891, 0.003991, 0.003979, 0.004075, 0.003984, 0.004065, 0.003982, 0.004360, 0.003709, 0.004066, 0.003999, 0.003952, 0.003863, 0.004113, 0.004073, 0.004135, 0.004019, 0.004010, 0.004050, 0.003998, 0.003993, 0.004023, 0.003981, 0.004029, 0.003952, 0.004006, 0.003977, 0.004113, 0.003985, 0.003882, 0.004061, 0.004138, 0.004031, 0.004022, 0.004044, 0.003979, 0.004273, 0.003569, 0.004121, 0.004011, 0.003980, 0.004075, 0.004062, 0.004106, 0.004039, 0.003924, 0.004120, 0.004021, 0.003814, 0.004112, 0.003839, 0.004075, 0.003852, 0.004102, 0.003785, 0.003941, 0.004037, 0.003934, 0.003972, 0.003991, 0.003926, 0.003981, 0.003900, 0.003931, 0.003919, 0.003972, 0.003898, 0.004025, 0.003921, 0.003988, 0.003950, 0.003985, 0.003977, 0.003942, 0.003891, 0.003842, 0.003996, 0.004013, 0.004641, 0.003618, 0.004006, 0.003934, 0.004005, 0.003895, 0.003827, 0.004054, 0.003970, 0.003932, 0.003945, 0.003923, 0.003970, 0.003849, 0.003872, 0.003945, 0.004012, 0.003985, 0.003968, 0.003937, 0.003969, 0.003929, 0.003952, 0.003965, 0.003983, 0.003987, 0.003884, 0.004076, 0.003965, 0.003996, 0.004067, 0.003973, 0.004067, 0.003925, 0.004055, 0.004007, 0.004006, 0.004000, 0.004056, 0.004759, 0.004045, 0.003996, 0.004026, 0.003934, 0.004036, 0.003996, 0.004139, 0.003942, 0.004015, 0.004029, 0.004300, 0.003607, 0.004072, 0.004007, 0.003987, 0.003956, 0.004038, 0.003965, 0.003987, 0.004029, 0.003869, 0.004031, 0.004007, 0.003958, 0.004034, 0.003962, 0.003990, 0.003969, 0.004016, 0.003961, 0.003960, 0.003959, 0.003967, 0.003977, 0.003998, 0.003977, 0.003942, 0.003984, 0.003984, 0.003964, 0.004027, 0.003925, 0.004076, 0.004032, 0.004055, 0.004016, 0.004089, 0.003979, 0.003989, 0.003961, 0.004094, 0.004019, 0.003974, 0.004009, 0.004065, 0.004003, 0.004019, 0.004039, 0.004084, 0.004008, 0.004334, 0.003632, 0.004010, 0.004009, 0.004032, 0.003943, 0.003974, 0.003959, 0.004010, 0.004002, 0.004030, 0.004073, 0.004090, 0.003995, 0.004038, 0.004017, 0.004075, 0.004043, 0.003982, 0.003900, 0.004052, 0.004044, 0.003958, 0.003991, 0.004047, 0.003958, 0.003964, 0.004019, 0.003967, 0.004026, 0.003895, 0.003818, 0.003985, 0.003982, 0.004053, 0.004001, 0.003994, 0.003939, 0.004016, 0.003895, 0.003984, 0.004000, 0.004003, 0.004005, 0.004052, 0.003956, 0.004013, 0.004008, 0.003963, 0.004010, 0.004047, 0.004019, 0.003992, 0.003990, 0.004054, 0.003948, 0.004067, 0.003924, 0.004008, 0.003988, 0.003999, 0.003991, 0.004065, 0.003991, 0.004071, 0.004089, 0.003956, 0.004041, 0.003999, 0.003881, 0.003932, 0.005367, 0.003674, 0.003919, 0.003833, 0.004035, 0.003739, 0.003934, 0.003892, 0.003971, 0.003882, 0.003945, 0.004016, 0.003983, 0.004014, 0.003977, 0.004018, 0.004036, 0.004074, 0.004006, 0.003982, 0.003989, 0.004026, 0.003860, 0.004006, 0.003944, 0.004030, 0.003998, 0.004025, 0.003954, 0.003974, 0.003971, 0.003968, 0.003938, 0.003940, 0.003948, 0.003987, 0.003963, 0.003983, 0.003962, 0.003978, 0.003957, 0.004074, 0.003963, 0.004006, 0.003970, 0.004026, 0.003958, 0.004011, 0.004049, 0.004032, 0.004067, 0.003864, 0.004002, 0.003984, 0.003801, 0.003979, 0.003964, 0.004042, 0.004027, 0.003933, 0.003994, 0.004004, 0.004064, 0.004008, 0.003956, 0.004001, 0.003939, 0.003977, 0.003978, 0.004030, 0.004015, 0.004003, 0.004003, 0.004099, 0.003965, 0.004160, 0.003903, 0.004034, 0.003975, 0.004032, 0.003984, 0.004083, 0.004026, 0.004132, 0.003984, 0.004014, 0.004138, 0.004028, 0.003829, 0.004010, 0.003984, 0.003933, 0.003955, 0.004067, 0.003889, 0.004031, 0.004047, 0.004004, 0.004026, 0.004006, 0.004060, 0.003923, 0.003950, 0.004043, 0.003993, 0.003978, 0.003953, 0.004181, 0.004091, 0.003993, 0.004076, 0.004039, 0.004010, 0.003990, 0.003957, 0.004074, 0.004046, 0.004024, 0.004005, 0.004033, 0.004048, 0.004065, 0.003990, 0.004005, 0.004094, 0.004165, 0.003969, 0.004036, 0.004057, 0.004032, 0.004029, 0.003980, 0.004004, 0.004008, 0.004005, 0.004022, 0.004037, 0.003960, 0.004016, 0.004002, 0.003833, 0.003957, 0.003959, 0.003995, 0.003992, 0.004024, 0.003974, 0.003986, 0.004002, 0.004060, 0.003972, 0.003972, 0.004020, 0.003999, 0.004017, 0.004043, 0.003966, 0.003993, 0.003951, 0.003980, 0.004024, 0.003992, 0.004017, 0.004176, 0.004027, 0.004008, 0.004037, 0.004035, 0.004003, 0.004018, 0.004001, 0.003973, 0.003999, 0.004051, 0.004032, 0.003943, 0.003966, 0.004074, 0.003937, 0.003958, 0.003980, 0.004014, 0.004011, 0.004056, 0.003983, 0.003960, 0.004013, 0.004034, 0.004025, 0.003957, 0.004016, 0.003941, 0.004014, 0.003955, 0.004010, 0.003979, 0.004003};
 const unsigned ticks[] = {0, 209, 246, 248, 251, 253, 256, 258, 262, 264, 266, 269, 271, 273, 276, 278, 280, 282, 285, 287, 289, 291, 293, 295, 297, 299, 301, 303, 305, 307, 309, 311, 313, 315, 317, 319, 321, 323, 326, 328, 330, 332, 334, 336, 338, 340, 342, 344, 347, 350, 352, 354, 356, 358, 360, 362, 364, 366, 368, 370, 372, 374, 376, 378, 380, 383, 385, 387, 389, 391, 393, 395, 397, 400, 402, 404, 406, 409, 411, 413, 415, 417, 419, 421, 423, 426, 428, 430, 432, 435, 437, 439, 441, 444, 446, 448, 450, 452, 454, 456, 458, 460, 462, 464, 466, 469, 471, 473, 475, 478, 480, 482, 484, 487, 489, 491, 493, 495, 497, 499, 501, 504, 506, 508, 510, 512, 514, 516, 518, 521, 523, 525, 527, 529, 531, 533, 535, 538, 540, 542, 544, 547, 549, 551, 553, 555, 557, 559, 561, 564, 566, 568, 570, 572, 574, 576, 578, 581, 583, 585, 587, 589, 591, 593, 595, 597, 599, 601, 603, 606, 608, 610, 614, 616, 618, 620, 622, 624, 626, 628, 631, 633, 635, 637, 639, 641, 643, 645, 647, 649, 651, 653, 656, 658, 660, 662, 664, 666, 668, 670, 673, 675, 677, 679, 681, 683, 685, 687, 689, 691, 693, 695, 698, 700, 702, 704, 706, 708, 710, 712, 714, 716, 718, 720, 723, 725, 727, 729, 731, 733, 735, 737, 740, 742, 744, 746, 748, 750, 752, 754, 757, 759, 761, 763, 766, 768, 770, 772, 774, 776, 778, 780, 783, 785, 787, 789, 792, 794, 796, 798, 800, 802, 804, 806, 809, 811, 813, 815, 818, 820, 822, 824, 827, 829, 831, 833, 836, 838, 840, 842, 845, 847, 849, 851, 854, 856, 858, 860, 868, 876, 878, 880, 885, 887, 889, 891, 893, 895, 897, 899, 901, 903, 905, 907, 909, 911, 913, 915, 917, 919, 921, 923, 925, 927, 929, 931, 933, 935, 937, 939, 941, 943, 945, 947, 949, 951, 953, 955, 957, 959, 961, 963, 965, 967, 969, 971, 973, 975, 977, 979, 981, 983, 985, 987, 989, 991, 993, 995, 997, 999, 1001, 1003, 1005, 1007, 1009, 1011, 1013, 1015, 1017, 1019, 1021, 1023, 1025, 1027, 1029, 1031, 1033, 1035, 1037, 1039, 1041, 1043, 1045, 1047, 1049, 1051, 1053, 1055, 1057, 1059, 1061, 1063, 1065, 1067, 1069, 1071, 1073, 1075, 1077, 1079, 1081, 1083, 1085, 1087, 1089, 1091, 1093, 1095, 1097, 1099, 1101, 1103, 1105, 1107, 1109, 1111, 1113, 1115, 1117, 1119, 1121, 1123, 1125, 1127, 1129, 1131, 1133, 1135, 1137, 1139, 1141, 1143, 1145, 1147, 1149, 1151, 1153, 1155, 1157, 1159, 1161, 1163, 1165, 1167, 1169, 1171, 1173, 1175, 1177, 1179, 1181, 1183, 1185, 1187, 1189, 1191, 1193, 1195, 1197, 1199, 1201, 1203, 1205, 1207, 1209, 1211, 1213, 1215, 1217, 1219, 1221, 1223, 1225, 1227, 1229, 1231, 1233, 1235, 1237, 1239, 1241, 1243, 1245, 1247, 1249, 1251, 1253, 1255, 1257, 1259, 1261, 1263, 1265, 1267, 1269, 1271, 1273, 1275, 1277, 1279, 1281, 1283, 1285, 1287, 1289, 1291, 1293, 1295, 1297, 1299, 1301, 1303, 1305, 1307, 1309, 1311, 1313, 1315, 1317, 1319, 1321, 1323, 1325, 1327, 1329, 1331, 1333, 1335, 1337, 1339, 1341, 1343, 1345, 1347, 1349, 1351, 1353, 1355, 1357, 1359, 1361, 1363, 1365, 1367, 1369, 1371, 1373, 1375, 1377, 1379, 1381, 1383, 1385, 1387, 1389, 1391, 1393, 1395, 1397, 1399, 1401, 1403, 1405, 1407, 1409, 1411, 1413, 1415, 1417, 1419, 1421, 1423, 1425, 1427, 1429, 1431, 1433, 1435, 1437, 1439, 1441, 1443, 1445, 1447, 1449, 1451, 1453, 1455, 1457, 1459, 1461, 1463, 1465, 1467, 1469, 1471, 1473, 1475, 1477, 1479, 1481, 1483, 1485, 1487, 1489, 1491, 1493, 1495, 1497, 1499, 1501, 1503, 1505, 1507, 1509, 1511, 1513, 1515, 1517, 1519, 1521, 1523, 1525, 1527, 1529, 1531, 1533, 1535, 1537, 1539, 1541, 1543, 1545, 1547, 1549, 1551, 1553, 1555, 1557, 1559, 1561, 1563, 1565, 1567, 1569, 1571, 1573, 1575, 1577, 1579, 1581, 1583, 1585, 1587, 1589, 1591, 1593, 1595, 1597, 1599, 1601, 1603, 1605, 1607, 1609, 1611, 1613, 1615, 1617, 1619, 1621, 1623, 1625, 1627, 1629, 1631, 1633, 1635, 1637, 1639, 1641, 1643, 1645, 1647, 1649, 1651, 1653, 1655, 1657, 1659, 1661, 1663, 1666, 1668, 1670, 1672, 1674, 1676, 1678, 1680, 1682, 1684, 1686, 1688, 1690, 1692, 1694, 1696, 1698, 1700, 1702, 1704, 1706, 1708, 1710, 1712, 1714, 1716, 1718, 1720, 1722, 1724, 1726, 1728, 1730, 1732, 1734, 1736, 1738, 1740, 1742, 1744, 1746, 1748, 1750, 1752, 1754, 1756, 1758, 1760, 1762, 1764, 1766, 1768, 1770, 1772, 1774, 1776, 1778, 1780, 1782, 1784, 1786, 1788, 1790, 1792, 1794, 1796, 1798, 1800, 1802, 1804, 1806, 1808, 1810, 1812, 1814, 1816, 1818, 1820, 1822, 1824, 1826, 1828, 1830, 1832, 1834, 1836, 1838, 1840, 1842, 1844, 1846, 1848, 1850, 1852, 1854, 1856, 1858, 1860, 1862, 1864, 1866, 1868, 1870, 1872, 1874, 1876, 1878, 1880, 1882, 1884, 1886, 1888, 1890, 1892, 1894, 1896, 1898, 1900, 1902, 1904, 1906, 1908, 1910, 1912, 1914, 1916, 1918, 1920, 1922, 1924, 1926, 1928, 1930, 1932, 1934, 1936, 1938, 1940, 1942, 1944, 1946, 1948, 1950, 1952, 1954, 1956, 1958, 1960, 1962, 1964, 1966, 1968, 1970, 1972, 1974, 1976, 1978, 1980, 1982, 1984, 1986, 1988, 1990, 1992, 1994, 1996, 1998, 2000, 2002, 2004, 2006, 2008, 2010, 2012, 2014, 2016, 2018, 2020, 2022, 2024, 2026, 2028, 2030, 2032, 2034, 2036, 2038, 2040, 2042, 2044, 2046, 2048, 2050, 2052, 2054, 2056, 2058};
*/
