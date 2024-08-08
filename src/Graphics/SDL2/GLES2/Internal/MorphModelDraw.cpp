#include "MorphModelDraw.h"
#include "../ModelInstance.h"
#include <cassert>
#include <iostream>
#include "SDL.h"

#include "../Camera.h"

void Graphics::SDL2::GLES2::Internal::MorphModelDraw::Animation::Dynamic::addTriangles(
            const std::vector<DynamicTriangleDraw::Triangle> &triangles,
            DynamicTriangleDraw::DrawCommand &triangles_draw ) const
{
    const DeltaTriangle * delta_triangle_r = nullptr;
    DynamicTriangleDraw::Triangle *draw_triangles_r;

    size_t number_of_triangles = 0;

    if(morph_info_r != nullptr) {
        number_of_triangles = triangles_draw.getTriangles( triangles.size(), &draw_triangles_r );
        delta_triangle_r = morph_info_r->getFrame( frame_index );
    }

    for( size_t i = 0; i < number_of_triangles; i++ ) {
        draw_triangles_r[ i ] = triangles[ i ];
        
        if( delta_triangle_r != nullptr ) {
            for( unsigned t = 0; t < 3; t++ ) {
                draw_triangles_r[ i ].vertices[ t ].position += delta_triangle_r[ i ].vertices[ t ];
            }
        }

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

    number_of_triangles = triangles_draw.getTriangles( this->facer_triangles_amount, &draw_triangles_r );

    size_t index = 0;

    glm::vec3 color;
    glm::vec2 texture_uv[4];
    DynamicTriangleDraw::PolygonType polygon_type;

    for( size_t i = 0; i < this->facer_polygons_stride; i++) {
        const auto &facer_polygon = this->facer_polygons_info_r->at(frame_index * this->facer_polygons_stride + i);

        switch( facer_polygon.type ) {
            case Data::Mission::ObjResource::FacerPolygon::STAR:
                color = glm::mix(facer_polygon.color, facer_polygon.graphics.star.other_color, std::abs(this->star_timings_r->at(facer_polygon.time_index)));

                index += DynamicTriangleDraw::Triangle::addStar(
                    &draw_triangles_r[index], number_of_triangles - index,
                    this->camera_position, this->transform, this->camera_right, this->camera_up,
                    facer_polygon.point[0].position, color, facer_polygon.width, facer_polygon.graphics.star.vertex_count);
                break;

            case Data::Mission::ObjResource::FacerPolygon::BILLBOARD:
                if(facer_polygon.visability_mode == Data::Mission::ObjResource::ADDITION)
                    polygon_type = DynamicTriangleDraw::PolygonType::ADDITION;
                else
                    polygon_type = DynamicTriangleDraw::PolygonType::MIX;

                if(facer_polygon.time_index == 0) {
                    for(int x = 0; x < 4; x++) {
                        texture_uv[x] = glm::vec2(facer_polygon.graphics.texture.coords[x]) * (1.f / 256.f);
                    }
                }
                else {
                    unsigned uv_index = 4 * (facer_polygon.time_index - 1);

                    for(int x = 0; x < 4; x++) {
                        texture_uv[x] = uv_frame_buffer_r->at(uv_index + x);
                    }
                }

                index += DynamicTriangleDraw::Triangle::addBillboard(
                    &draw_triangles_r[index], number_of_triangles - index,
                    this->camera_position, this->transform, this->camera_right, this->camera_up,
                    facer_polygon.point[0].position, facer_polygon.color, facer_polygon.width,
                    polygon_type, facer_polygon.graphics.texture.bmp_id, texture_uv
                );
                break;
        }
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
            
            unsigned addition_index = std::min( material.count, material.addition_index );
            unsigned mix_index = std::min( material.count, material.mix_index );
            unsigned transparent_index = std::min( mix_index, addition_index );
            
            glm::vec4 joints = glm::vec4(0, 0, 0, 1);
            
            const unsigned vertex_per_triangle = 3;
            
            for( unsigned m = transparent_index; m < material.count; m += vertex_per_triangle ) {
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
    "const int ANIMATED_UV_FRAME_AMOUNT = 64;\n"
    "const int QUAD_VERTEX_AMOUNT = 4;\n"
    "const int ANIMATED_UV_FRAME_VEC_AMOUNT = ANIMATED_UV_FRAME_AMOUNT * QUAD_VERTEX_AMOUNT;\n"

    // Vertex shader uniforms
    "uniform mat4 ModelViewInv;\n"
    "uniform mat4 ModelView;\n"
    "uniform mat4 Transform;\n" // projection * view * model.
    "uniform vec2 TextureTranslation;\n"
    // These uniforms are for modifiying the morph attributes
    "uniform float SampleLast;\n"
    "uniform vec2 AnimatedUVFrames[ ANIMATED_UV_FRAME_VEC_AMOUNT ];\n"

    "void main()\n"
    "{\n"
    "   vec3 current_position = POSITION + POSITION_Last * vec3( SampleLast );\n"
    "   vec3 current_normal   = NORMAL   + NORMAL_Last   * vec3( SampleLast );\n"
    // This reflection code is based on https://stackoverflow.com/questions/27619078/reflection-mapping-in-opengl-es
    "   vec3 eye_coord_position = vec3( ModelView * vec4(current_position, 1.0) );\n" // Model View multiplied by Model Position.
    "   vec3 eye_coord_normal   = vec3( ModelView * vec4(current_normal, 0.0));\n"
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
    "   MAKE_FULL_POSITION(current_position);\n"
    "   gl_Position = Transform * full_position;\n"
    "}\n";
Graphics::SDL2::GLES2::Internal::MorphModelDraw::MorphModelDraw() {
    // These inputs are for the morph attributes
    attributes.push_back( Shader::Attribute( Shader::Type::MEDIUM, "vec3 POSITION_Last" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW,    "vec3 NORMAL_Last" ) );

    const size_t MORPH_BUFFER_NO_NORMALS_SIZE = 3 * sizeof( float );
    const size_t MORPH_BUFFER_SIZE = (3 + 3) * sizeof( float );

    morph_model_attribute_array.addAttribute( "POSITION_Last", 3, GL_FLOAT, GL_FALSE, MORPH_BUFFER_SIZE, 0 );
    morph_model_attribute_array.addAttribute( "NORMAL_Last",   3, GL_FLOAT, GL_FALSE, MORPH_BUFFER_SIZE, (void*)(3 * sizeof( float )) );

    morph_bb_attribute_array.addAttribute( "POSITION_Last", 3, GL_FLOAT, GL_FALSE, MORPH_BUFFER_NO_NORMALS_SIZE, 0 );
    morph_bb_attribute_array.addAttribute( "NORMAL_Last",   3, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) );

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

    sample_last_uniform_id = program.getUniform( "SampleLast", &std::cout, &uniform_failed );
    
    attribute_failed |= !program.isAttribute( "POSITION_Last", &std::cout );
    attribute_failed |= !program.isAttribute( "NORMAL_Last", &std::cout );

    morph_model_attribute_array.allocate( program );
    morph_model_attribute_array.cullUnfound();

    morph_bb_attribute_array.allocate( program );
    morph_bb_attribute_array.cullUnfound();

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

int Graphics::SDL2::GLES2::Internal::MorphModelDraw::inputModel( Utilities::ModelBuilder *model_type_r,  const Data::Mission::ObjResource& obj, const std::map<uint32_t, Internal::Texture2D*>& textures ) {
    auto ret = Graphics::SDL2::GLES2::Internal::StaticModelDraw::inputModel( model_type_r, obj, textures );

    if( ret >= 0 )
    {
        const uint32_t obj_identifier = obj.getResourceID();

        GLsizei transparent_count = 0;

        auto accessor = models_p.find( obj_identifier );
        if( accessor != models_p.end() ) {
            transparent_count = (*accessor).second->transparent_triangles.size();
        }
        
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

void Graphics::SDL2::GLES2::Internal::MorphModelDraw::generalDraw( Graphics::SDL2::GLES2::Camera &camera, std::map<uint32_t, ModelArray*> &model_array_p, VertexAttributeArray &morph_attribute_array ) {
    glm::mat4 camera_3D_model_transform; // This holds the model transform like the position rotation and scale.
    glm::mat4 camera_3D_projection_view_model; // This holds the two transforms from above.
    glm::mat4 camera_3D_projection_view; // This holds the camera transform along with the view.
    glm::mat4 view;
    glm::mat4 model_view;
    glm::mat4 model_view_inv;
    
    camera.getProjectionView3D( camera_3D_projection_view ); // camera_3D_projection_view = current_camera 3D matrix.

    camera.getView3D( view );
    
    program.use();

    // Check if there is even a shiney texture.
    if( shiney_texture_r != nullptr )
        shiney_texture_r->bind( 1, specular_texture_uniform_id );

    Animation::Dynamic dynamic;
    dynamic.camera_position = camera.getPosition();
    dynamic.camera_right = glm::vec3(view[0][0], view[1][0], view[2][0]);
    dynamic.camera_up    = glm::vec3(view[0][1], view[1][1], view[2][1]);

    // Traverse the models.
    for( auto d = model_array_p.begin(); d != model_array_p.end(); d++ ) // Go through every model that has an instance.
    {
        // Get the mesh information.
        Graphics::SDL2::GLES2::Internal::Mesh *mesh_r = &( *d ).second->mesh;
        
        // Go through every instance that refers to this mesh.
        for( auto instance = ( *d ).second->instances_r.begin(); instance != ( *d ).second->instances_r.end(); instance++ )
        {
            if( camera.isVisible( *(*instance) ) ) {
                const auto texture_offset = (*instance)->getTextureOffset();
                glUniform2f( this->texture_offset_uniform_id, texture_offset.x, texture_offset.y );

                (*d).second->bindUVAnimation(animated_uv_frames_id, (*instance)->getTextureTransformTimeline(), this->uv_frame_buffer);

                // Get the position and rotation of the model.
                // Multiply them into one matrix which will hold the entire model transformation.
                camera_3D_model_transform = glm::translate( glm::mat4(1.0f), (*instance)->getPosition() ) * glm::mat4_cast( (*instance)->getRotation() );

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

                int current_last_frame = static_cast<unsigned int>( floor( (*instance)->getPositionTransformTimeline() ) ) - 1;

                if( current_last_frame < 0 )
                {
                    current_last_frame = 0; // Next is unused.
                    glUniform1f( sample_last_uniform_id, 0.0f );
                }
                else
                    glUniform1f( sample_last_uniform_id, 1.0f );

                morph_attribute_array.bind( mesh_r->getMorphOffset( current_last_frame ) );

                mesh_r->noPreBindDrawOpaque( 0, diffusive_texture_uniform_id );
                
                auto accessor = model_animation_p.find( ( *d ).first );

                dynamic.transform = camera_3D_model_transform;

                if( accessor != model_animation_p.end() )
                    dynamic.morph_info_r = (*accessor).second;
                else
                    dynamic.morph_info_r = nullptr;

                dynamic.frame_index = static_cast<unsigned int>( floor( (*instance)->getPositionTransformTimeline() ) );
                dynamic.star_timings_r = &(*instance)->star_timings;
                dynamic.uv_frame_buffer_r = &this->uv_frame_buffer;
                dynamic.facer_polygons_info_r  = &(*d).second->facer_polygons_info;
                dynamic.facer_triangles_amount =  (*d).second->facer_triangles_amount;
                dynamic.facer_polygons_stride  =  (*d).second->facer_polygons_stride;
                dynamic.texture_offset = texture_offset;
                dynamic.addTriangles( (*d).second->transparent_triangles, camera.transparent_triangles );
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
                (*instance)->setPositionTransformTimeline( fmod( (*instance)->getPositionTransformTimeline() + seconds_passed * FRAME_SPEED, total_frame_amount ) );
                (*instance)->addTextureTransformTimelineSeconds( seconds_passed );
            }
        }
        else {
            for( auto instance = (*model_type).second->instances_r.begin(); instance != (*model_type).second->instances_r.end(); instance++ ) {
                (*instance)->addTextureTransformTimelineSeconds( seconds_passed );
            }
        }
    }
}
