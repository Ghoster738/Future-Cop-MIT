#include "SkeletalModelDraw.h"
#include "../ModelInstance.h"
#include <glm/gtc/type_ptr.hpp>
#include <cassert>
#include "SDL.h"
#include <iostream>

#include "../Camera.h"

Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::SkeletalAnimation::SkeletalAnimation( unsigned int num_bones, unsigned int amount_of_frames ) {
    this->num_bones = num_bones;
    
    // set the joint matrix frames.
    bone_frames.resize( amount_of_frames * num_bones );
}

glm::mat4* Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::SkeletalAnimation::getFrames( unsigned int current_frame ) {
    if( current_frame < bone_frames.size() )
        return bone_frames.data() + current_frame * this->num_bones;
    else
        return nullptr;
}

void Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::SkeletalAnimation::Dynamic::addTriangles(
    const std::vector<DynamicTriangleDraw::Triangle> &triangles,
    DynamicTriangleDraw::DrawCommand &triangles_draw ) const
{
    glm::mat4 *matrices_r = nullptr;

    if( skeletal_info_r->num_bones == 0 )
        return;

    matrices_r = skeletal_info_r->getFrames( current_frame );

    if( matrices_r == nullptr )
        return;

    DynamicTriangleDraw::Triangle *draw_triangles_r;

    size_t number_of_triangles = triangles_draw.getTriangles( triangles.size(), &draw_triangles_r );

    for( size_t i = 0; i < number_of_triangles; i++ ) {
        draw_triangles_r[ i ] = triangles[ i ];

        for( unsigned t = 0; t < 3; t++ ) {
            auto position = matrices_r[ skeletal_info_r->triangle_weights[ i ].vertices[ t ] ] * glm::vec4( draw_triangles_r[ i ].vertices[ t ].position, 1);

            position.w = 1;

            position = transform * position;

            draw_triangles_r[ i ].vertices[ t ].position.x = position.x * (1. / position.w);
            draw_triangles_r[ i ].vertices[ t ].position.y = position.y * (1. / position.w);
            draw_triangles_r[ i ].vertices[ t ].position.z = position.z * (1. / position.w);

            const uint16_t texture_animation_data = static_cast<uint16_t>(draw_triangles_r[ i ].vertices[ t ].vertex_metadata[1]);

            if(texture_animation_data != 0) {
                const uint16_t texture_animation_index = texture_animation_data - 1;

                assert(texture_animation_index < uv_frame_buffer_r->size());

                draw_triangles_r[ i ].vertices[ t ].coordinate = (*uv_frame_buffer_r)[texture_animation_index];
            }

            draw_triangles_r[ i ].vertices[ t ].coordinate += texture_offset;


            draw_triangles_r[ i ].vertices[ t ].color.r *= this->color.r;
            draw_triangles_r[ i ].vertices[ t ].color.g *= this->color.g;
            draw_triangles_r[ i ].vertices[ t ].color.b *= this->color.b;
        }

        draw_triangles_r[ i ] = draw_triangles_r[ i ].addTriangle( this->camera_position );
    }

    number_of_triangles = triangles_draw.getTriangles( this->facer_triangles_amount, &draw_triangles_r );

    size_t index = 0;
    glm::vec4 position;

    glm::vec3 color;
    glm::vec2 texture_uv[4];
    DynamicTriangleDraw::PolygonType polygon_type;

    for( size_t i = 0; i < this->facer_polygons_stride; i++) {
        const auto &facer_polygon = this->facer_polygons_info_r->at(i);

        position = matrices_r[ facer_polygon.point.joints.x ] * glm::vec4(facer_polygon.point.position, 1);

        position.w = 1;

        position = transform * position;

        position = position * (1.f / position.w);

        switch( facer_polygon.type ) {
            case Data::Mission::ObjResource::PrimitiveType::STAR:
                color = glm::mix(facer_polygon.color * this->color, facer_polygon.graphics.star.other_color, std::abs(this->star_timings_r->at(facer_polygon.time_index)));

                index += DynamicTriangleDraw::Triangle::addStar(
                    &draw_triangles_r[index], number_of_triangles - index,
                    this->camera_position, this->transform, this->camera_right, this->camera_up,
                    glm::vec3(position.x, position.y, position.z), color, facer_polygon.width, facer_polygon.graphics.star.vertex_count);
                break;

            case Data::Mission::ObjResource::PrimitiveType::BILLBOARD:
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
                    glm::vec3(position.x, position.y, position.z), facer_polygon.color * this->color, facer_polygon.width,
                    polygon_type, facer_polygon.graphics.texture.bmp_id, texture_uv
                );
                break;
            default:
                break; // Do nothing
        }
    }
}

const GLchar* Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::default_vertex_shader =
    "const int ANIMATED_UV_FRAME_AMOUNT = 64;\n"
    "const int QUAD_VERTEX_AMOUNT = 4;\n"
    "const int ANIMATED_UV_FRAME_VEC_AMOUNT = ANIMATED_UV_FRAME_AMOUNT * QUAD_VERTEX_AMOUNT;\n"
    // Vertex shader uniforms
    "uniform mat4 ModelViewInv;\n"
    "uniform mat4 ModelView;\n"
    "uniform mat4 Transform;\n" // projection * view * model.
    "uniform vec3 ModelColor;\n"
    "uniform vec2 TextureTranslation;\n"
    // These uniforms are for the bones of the animation.
    "uniform mat4 Bone[12];\n" // 12 bones seems to be the limit of Future Cop.
    "uniform vec2  AnimatedUVFrames[ ANIMATED_UV_FRAME_VEC_AMOUNT ];\n"

    "void main()\n"
    "{\n"
    "   vec4 current_position = Bone[ int( JOINTS_0.x ) ] * vec4(POSITION, 1.0);\n"
    "   vec3 current_normal   = NORMAL;\n"
    // This reflection code is based on https://stackoverflow.com/questions/27619078/reflection-mapping-in-opengl-es
    "   vec3 eye_coord_position = vec3( ModelView * current_position );\n" // Model View multiplied by Model Position.
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
    "   in_color = COLOR_0 * vec4(ModelColor, 1.);\n"
    "   MAKE_FULL_POSITION(current_position);\n"
    "   gl_Position = Transform * full_position;\n"
    "}\n";
Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::SkeletalModelDraw() {
    // These attributes are for the skelatal animation.
    attributes.push_back( Shader::Attribute( Shader::Type::LOW, "vec4 JOINTS_0" ) );
    attributes.push_back( Shader::Attribute( Shader::Type::LOW, "vec3 WEIGHTS_0" ) );
}

Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::~SkeletalModelDraw() {
    clearModels();
}

const GLchar* Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::getDefaultVertexShader() {
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

int Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::inputModel( Utilities::ModelBuilder *model_type_r, const Data::Mission::ObjResource& obj, const std::map<uint32_t, Internal::Texture2D*>& textures) {
    auto ret = Graphics::SDL2::GLES2::Internal::StaticModelDraw::inputModel( model_type_r, obj, textures );
    
    if( ret >= 0 )
    {
        const uint32_t obj_identifier = obj.getResourceID();

        if( model_animation_p[ obj_identifier ] != nullptr )
            delete model_animation_p[ obj_identifier ];
        
        model_animation_p[ obj_identifier ] = new SkeletalAnimation( model_type_r->getNumJoints(), model_type_r->getNumJointFrames() );

        for( unsigned int frame_index = 0; frame_index < model_type_r->getNumJointFrames(); frame_index++ )
        {
            glm::mat4* frame_r = model_animation_p[ obj_identifier ]->getFrames( frame_index );

            for( unsigned int bone_index = 0; bone_index < model_type_r->getNumJoints(); bone_index++ )
            {
                frame_r[ bone_index ] = model_type_r->getJointFrame( frame_index, bone_index );
            }
        }

        models_p.at( obj_identifier )->mesh.setFrameAmount( model_type_r->getNumJointFrames() );

        // triangle_weights;
        GLsizei transparent_count = 0;

        auto accessor = models_p.find( obj_identifier );
        if( accessor != models_p.end() )
            transparent_count = (*accessor).second->transparent_triangles.size();

        model_animation_p[ obj_identifier ]->triangle_weights.reserve( transparent_count );

        GLsizei material_count = 0;

        unsigned joint_compenent_index = model_type_r->getNumVertexComponents();

        Utilities::ModelBuilder::VertexComponent element("EMPTY");
        for( unsigned i = 0; model_type_r->getVertexComponent( i, element ); i++ ) {
            auto name = element.getName();

            if( name == Utilities::ModelBuilder::JOINTS_INDEX_0_COMPONENT_NAME )
                joint_compenent_index = i;
        }

        Utilities::ModelBuilder::TextureMaterial material;
        for( unsigned a = 0; a < model_type_r->getNumMaterials(); a++ ) {
            model_type_r->getMaterial( a, material );

            GLsizei addition_index = std::min( material.count, material.addition_index );
            GLsizei mix_index = std::min( material.count, material.mix_index );
            GLsizei transparent_index = std::min( mix_index, addition_index );

            glm::vec4 joints = glm::vec4(0, 0, 0, 1);

            const unsigned vertex_per_triangle = 3;

            for( unsigned m = transparent_index; m < material.count; m += vertex_per_triangle ) {
                SkeletalAnimation::TriangleIndex triangle;

                for( unsigned t = 0; t < vertex_per_triangle; t++ ) {
                    model_type_r->getTransformation( joints, joint_compenent_index, material_count + m + t );

                    triangle.vertices[t] = joints.x;
                }

                model_animation_p[ obj_identifier ]->triangle_weights.push_back( triangle );
            }

            material_count += material.count;
        }
    }
    
    return ret;
}

void Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::clearModels() {
    StaticModelDraw::clearModels();

    // Delete the models first.
    for( auto i = model_animation_p.begin(); i != model_animation_p.end(); i++ )
    {
        delete (*i).second; // First delete the pointer.
        (*i).second = nullptr; // Then set the pointer to null.
    }
    model_animation_p.clear();
}

void Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::draw( Graphics::SDL2::GLES2::Camera &camera ) {
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

    SkeletalAnimation::Dynamic dynamic;
    dynamic.camera_position = camera.getPosition();
    dynamic.camera_right = glm::vec3(view[0][0], view[1][0], view[2][0]);
    dynamic.camera_up    = glm::vec3(view[0][1], view[1][1], view[2][1]);

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
                if( camera.isVisible( *(*instance) ) ) {
                    const auto texture_offset = (*instance)->getTextureOffset();
                    glUniform2f( this->texture_offset_uniform_id, texture_offset.x, texture_offset.y );

                    (*d).second->bindUVAnimation(animated_uv_frames_id, (*instance)->getTextureTransformTimeline(), this->uv_frame_buffer);

                    auto color = (*instance)->getColor();

                    glUniform3fv( this->model_color_uniform_id, 1, reinterpret_cast<const GLfloat*>( &color ));

                    // Get the position and rotation of the model.
                    // Multiply them into one matrix which will hold the entire model transformation.
                    camera_3D_model_transform = glm::translate( glm::mat4(1.0f), (*instance)->getPosition() ) * glm::mat4_cast( (*instance)->getRotation() ) * glm::scale(glm::mat4(1.0f), (*instance)->getScale());
                    
                    // Then multiply it to the projection, and view to get projection, view, and model matrix.
                    camera_3D_projection_view_model = camera_3D_projection_view * camera_3D_model_transform;
                    
                    // We can now send the matrix to the program.
                    glUniformMatrix4fv( matrix_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &camera_3D_projection_view_model[0][0] ) );
                    
                    // TODO Find a cleaner way.
                    model_view = view * camera_3D_model_transform;
                    model_view_inv = glm::inverse( model_view );
                    glUniformMatrix4fv(     view_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &model_view[0][0] ) );
                    glUniformMatrix4fv( view_inv_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &model_view_inv[0][0] ) );
                    
                    int current_frame = static_cast<unsigned int>( floor( (*instance)->getPositionTransformTimeline() ) );
                    
                    assert( animate_r->getFrames( current_frame ) != nullptr );
                    
                    glUniformMatrix4fv( mat4_array_uniform_id, animate_r->getNumBones(), GL_FALSE, glm::value_ptr( *animate_r->getFrames( current_frame ) ) );
                    
                    mesh_r->drawOpaque( 0, diffusive_texture_uniform_id );
                    
                    dynamic.transform = camera_3D_model_transform;
                    dynamic.skeletal_info_r = animate_r;
                    dynamic.current_frame = current_frame;
                    dynamic.uv_frame_buffer_r = &this->uv_frame_buffer;
                    dynamic.texture_offset = texture_offset;
                    dynamic.color = color;
                    dynamic.star_timings_r = &(*instance)->star_timings;
                    dynamic.facer_polygons_info_r  = &(*d).second->facer_polygons_info;
                    dynamic.facer_triangles_amount =  (*d).second->facer_triangles_amount;
                    dynamic.facer_polygons_stride  =  (*d).second->facer_polygons_stride;
                    dynamic.addTriangles( (*d).second->transparent_triangles, camera.transparent_triangles );
                }
            }
        }
    }
}

void Graphics::SDL2::GLES2::Internal::SkeletalModelDraw::advanceTime( float seconds_passed )
{
    // Go through every model array.
    for( auto model_type = models_p.begin(); model_type != models_p.end(); model_type++ ) {
        // Get the mesh.
        Graphics::SDL2::GLES2::Internal::Mesh *mesh_r = &(*model_type).second->mesh;
        
        for( auto instance = (*model_type).second->instances_r.begin(); instance != (*model_type).second->instances_r.end(); instance++ ) {
            (*instance)->addTextureTransformTimelineSeconds( seconds_passed );
        }
    }
}
