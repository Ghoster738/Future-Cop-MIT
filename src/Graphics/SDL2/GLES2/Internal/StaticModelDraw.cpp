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

    number_of_triangles = triangles_draw.getTriangles( this->facer_triangles_amount, &draw_triangles_r );

    size_t index = 0;

    glm::vec3 color;
    glm::vec2 texture_uv[4];
    DynamicTriangleDraw::PolygonType polygon_type;

    for( size_t i = 0; i < this->facer_polygons_stride; i++) {
        const auto &facer_polygon = this->facer_polygons_info_r->at(i);

        switch( facer_polygon.type ) {
            case Data::Mission::ObjResource::PrimitiveType::STAR:
                color = glm::mix(facer_polygon.color, facer_polygon.graphics.star.other_color, std::abs(this->star_timings_r->at(facer_polygon.time_index)));

                index += DynamicTriangleDraw::Triangle::addStar(
                    &draw_triangles_r[index], number_of_triangles - index,
                    this->camera_position, this->transform, this->camera_right, this->camera_up,
                    facer_polygon.point.position, color, facer_polygon.width,
                    facer_polygon.graphics.star.vertex_count);
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
                    facer_polygon.point.position, facer_polygon.color, facer_polygon.width,
                    polygon_type, facer_polygon.graphics.texture.bmp_id, texture_uv
                );
                break;
            default:
                break; // Do nothing
        }
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
    "   vec3 eye_coord_position = vec3( ModelView * vec4(POSITION, 1.0) );\n" // Model View multiplied by Model Position.
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
    "   MAKE_FULL_POSITION(POSITION);\n"
    "   gl_Position = Transform * full_position;\n"
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

    attributes.push_back( Shader::Attribute( Shader::Type::MEDIUM, "vec3 POSITION" ) );
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
        
        glBindAttribLocation( program.getProgramID(), 0, "POSITION" );
        glBindAttribLocation( program.getProgramID(), 1, "COLOR_0" );
        glBindAttribLocation( program.getProgramID(), 2, "NORMAL" );
        glBindAttribLocation( program.getProgramID(), 3, "TEXCOORD_0" );
        glBindAttribLocation( program.getProgramID(), 4, "_METADATA" );

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

int Graphics::SDL2::GLES2::Internal::StaticModelDraw::inputModel( Utilities::ModelBuilder *model_type_r, const Data::Mission::ObjResource& obj, const std::map<uint32_t, Internal::Texture2D*>& textures ) {
    int state = 0;

    const uint32_t obj_identifier = obj.getResourceID();

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

    unsigned facer_triangles_amount = 0;

    models_p[ obj_identifier ]->star_timing_speed.resize(obj.getVertexColorOverrides().size() + 1, 0.f);
    for( unsigned i = 0; i < obj.getVertexColorOverrides().size(); i++ ) {
        models_p[ obj_identifier ]->star_timing_speed[1 + i] = obj.getVertexColorOverrides()[i].speed_factor;
    }

    models_p[ obj_identifier ]->transparent_triangles.reserve( transparent_count );
    models_p[ obj_identifier ]->facer_polygons_info   = obj.generateFacingPolygons(facer_triangles_amount, models_p[ obj_identifier ]->facer_polygons_stride);
    models_p[ obj_identifier ]->facer_triangles_amount = facer_triangles_amount;
    models_p[ obj_identifier ]->uv_animation_data     = obj.getFaceOverrideData();
    models_p[ obj_identifier ]->uv_animation_info     = obj.getFaceOverrideTypes();

    {
        glm::vec3 sphere_position;
        float radius;

        if(obj.getBoundingSphereFacingPolygons(models_p[ obj_identifier ]->facer_polygons_info, sphere_position, radius))
            models_p[ obj_identifier ]->mesh.appendBoundingSphere( sphere_position, radius );
    }

    const size_t face_override_amount = 4 * obj.getFaceOverrideTypes().size();

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
        bounding_boxes_p[ obj_identifier ]->facer_triangles_amount = 0;
        bounding_boxes_p[ obj_identifier ]->facer_polygons_stride  = 0;

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
    
    camera.getProjectionView3D( camera_3D_projection_view );

    camera.getView3D( view );
    
    // Use the static shader for the static models.
    program.use();

    // Check if there is even a shiney texture.
    if( shiney_texture_r != nullptr )
        shiney_texture_r->bind( 1, specular_texture_uniform_id );

    Dynamic dynamic;
    dynamic.camera_position = camera.getPosition();
    dynamic.camera_right = glm::vec3(view[0][0], view[1][0], view[2][0]);
    dynamic.camera_up    = glm::vec3(view[0][1], view[1][1], view[2][1]);

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
                camera_3D_model_transform = glm::translate( glm::mat4(1.0f), (*instance)->getPosition() ) * glm::mat4_cast( (*instance)->getRotation() ) * glm::scale(glm::mat4(1.0f), (*instance)->getScale());
                
                // Then multiply it to the projection, and view to get projection, view, and model matrix.
                camera_3D_projection_view_model = camera_3D_projection_view * camera_3D_model_transform;
                
                // We can now send the matrix to the program.
                glUniformMatrix4fv( matrix_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &camera_3D_projection_view_model[0][0] ) );
                
                model_view = view * camera_3D_model_transform;
                model_view_inv = glm::inverse( model_view );
                glUniformMatrix4fv( view_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &model_view[0][0] ) );
                glUniformMatrix4fv( view_inv_uniform_id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>( &model_view_inv[0][0] ) );
                
                // Finally we can draw the mesh!
                mesh_r->drawOpaque( 0, diffusive_texture_uniform_id );
                
                dynamic.texture_offset = texture_offset;
                dynamic.star_timings_r = &(*instance)->star_timings;
                dynamic.uv_frame_buffer_r = &this->uv_frame_buffer;
                dynamic.facer_polygons_info_r  = &(*d).second->facer_polygons_info;
                dynamic.facer_triangles_amount =  (*d).second->facer_triangles_amount;
                dynamic.facer_polygons_stride  =  (*d).second->facer_polygons_stride;
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

        model_instance.star_timings.resize(models_p[ obj_identifier ]->star_timing_speed.size(), 0.f);

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
    // Go through every model array.
    for( auto model_type = models_p.begin(); model_type != models_p.end(); model_type++ ) {
        for( auto instance = (*model_type).second->instances_r.begin(); instance != (*model_type).second->instances_r.end(); instance++ ) {
            (*instance)->addTextureTransformTimelineSeconds( seconds_passed );

            for( size_t i = 0; i < (*instance)->star_timings.size(); i++ ) {
                auto &value = (*instance)->star_timings[i];

                value += (*model_type).second->star_timing_speed[i] * seconds_passed;

                if(value > 1.)
                    value -= 2.;
            }
        }
    }
}

bool Graphics::SDL2::GLES2::Internal::StaticModelDraw::getBoundingSphere( uint32_t obj_identifier, glm::vec3 &position, float &radius ) const {
    if( models_p.find( obj_identifier ) != models_p.end() )
        return false;
    return models_p.at( obj_identifier )->mesh.getBoundingSphere( position, radius );
}
