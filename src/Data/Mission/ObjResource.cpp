#include "ObjResource.h"

#include "IFF.h"

#include <glm/gtc/constants.hpp>
#include <glm/geometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <cassert>
#include <iostream>

namespace {
    // The header
    const uint32_t TAG_4DGI = 0x34444749; // which is { 0x34, 0x44, 0x47, 0x49 } or { '4', 'D', 'G', 'I' } or "4DGI"
    // Texture reference list
    const uint32_t TAG_3DTL = 0x3344544C; // which is { 0x33, 0x44, 0x54, 0x4C } or { '3', 'D', 'T', 'L' } or "3DTL"
    // Face Quad list holding offsets of vertices and normals.
    const uint32_t TAG_3DQL = 0x3344514C; // which is { 0x33, 0x44, 0x51, 0x4C } or { '3', 'D', 'Q', 'L' } or "3DQL"
    // Bones.
    const uint32_t TAG_3DHY = 0x33444859; // which is { 0x33, 0x44, 0x48, 0x59 } or { '3', 'D', 'H', 'Y' } or "3DHY"
    // Positions of other objects
    const uint32_t TAG_3DHS = 0x33444853; // which is { 0x33, 0x44, 0x48, 0x53 } or { '3', 'D', 'H', 'S' } or "3DHS"
    // Bone Animation Attributes.
    const uint32_t TAG_3DMI = 0x33444D49; // which is { 0x33, 0x44, 0x4d, 0x49 } or { '3', 'D', 'M', 'I' } or "3DMI"
    // Texture Cordinate Animation chunk.
    const uint32_t TAG_3DTA = 0x33445441; // which is { 0x33, 0x44, 0x54, 0x41 } or { '3', 'D', 'T', 'A' } or "3DTA"
    // Circle Vertex Color Animation chunk.
    const uint32_t TAG_3DAL = 0x3344414C; // which is { 0x33, 0x44, 0x41, 0x4C } or { '3', 'D', 'A', 'L' } or "3DAL"
    // Reference IDs
    const uint32_t TAG_3DRF = 0x33445246; // which is { 0x33, 0x44, 0x52, 0x46 } or { '3', 'D', 'R', 'F' } or "3DRF"
    // 4D vertex list (Note: Ignore the 4D data).
    const uint32_t TAG_4DVL = 0x3444564C; // which is { 0x34, 0x44, 0x56, 0x4C } or { '4', 'D', 'V', 'L' } or "4DVL"
    // 4D normal list
    const uint32_t TAG_4DNL = 0x34444E4C; // which is { 0x34, 0x44, 0x4E, 0x4C } or { '4', 'D', 'N', 'L' } or "4DNL"
    // 3D reference lengths.
    const uint32_t TAG_3DRL = 0x3344524C; // which is { 0x33, 0x44, 0x52, 0x4C } or { '3', 'D', 'R', 'L' } or "3DRL"
    // Animation track data
    const uint32_t TAG_AnmD = 0x416e6d44; // which is { 0x41, 0x6e, 0x6d, 0x44 } or { 'A', 'n', 'm', 'D' } or "AnmD"
    // 3D bounding box
    const uint32_t TAG_3DBB = 0x33444242; // which is { 0x33, 0x44, 0x42, 0x42 } or { '3', 'D', 'B', 'B' } or "3DBB"

    const uint8_t QUAD_TABLE[2][3] = { {0, 1, 2}, {2, 3, 0}};

    void triangleToCoords( const Data::Mission::ObjResource::Primitive &triangle, const Data::Mission::ObjResource::FaceType &texture_quad, glm::u8vec2 *coords, int16_t *face_override_index )
    {
        assert(coords != nullptr);
        assert(face_override_index != nullptr);

        if( !triangle.visual.uses_texture ) {
            coords[0] = glm::u8vec2(0, 0);
            coords[1] = glm::u8vec2(0, 0);
            coords[2] = glm::u8vec2(0, 0);

            face_override_index[0] = 0;
            face_override_index[1] = 0;
            face_override_index[2] = 0;
        }
        else
        if( triangle.type != Data::Mission::ObjResource::PrimitiveType::TRIANGLE_OTHER )
        {
            coords[0] = texture_quad.coords[QUAD_TABLE[0][0]];
            coords[1] = texture_quad.coords[QUAD_TABLE[0][1]];
            coords[2] = texture_quad.coords[QUAD_TABLE[0][2]];

            if(texture_quad.face_override_index != 0) {
                face_override_index[0] = 4 * (texture_quad.face_override_index - 1) + QUAD_TABLE[0][0] + 1;
                face_override_index[1] = 4 * (texture_quad.face_override_index - 1) + QUAD_TABLE[0][1] + 1;
                face_override_index[2] = 4 * (texture_quad.face_override_index - 1) + QUAD_TABLE[0][2] + 1;
            }
            else {
                face_override_index[0] = 0;
                face_override_index[1] = 0;
                face_override_index[2] = 0;
            }
        }
        else
        {
            coords[0] = texture_quad.coords[QUAD_TABLE[1][0]];
            coords[1] = texture_quad.coords[QUAD_TABLE[1][1]];
            coords[2] = texture_quad.coords[QUAD_TABLE[1][2]];

            if(texture_quad.face_override_index != 0) {
                face_override_index[0] = 4 * (texture_quad.face_override_index - 1) + QUAD_TABLE[1][0] + 1;
                face_override_index[1] = 4 * (texture_quad.face_override_index - 1) + QUAD_TABLE[1][1] + 1;
                face_override_index[2] = 4 * (texture_quad.face_override_index - 1) + QUAD_TABLE[1][2] + 1;
            }
            else {
                face_override_index[0] = 0;
                face_override_index[1] = 0;
                face_override_index[2] = 0;
            }
        }
    }

    void handlePositions( glm::vec3 &position, const glm::i16vec3 *array, int index ) {
        position.x = -array[ index ].x * Data::Mission::ObjResource::FIXED_POINT_UNIT;
        position.y =  array[ index ].y * Data::Mission::ObjResource::FIXED_POINT_UNIT;
        position.z =  array[ index ].z * Data::Mission::ObjResource::FIXED_POINT_UNIT;
    }
    void handleNormals( glm::vec3 &normal, const glm::i16vec3 *array, int index ) {
        normal.x = -array[ index ].x;
        normal.y =  array[ index ].y;
        normal.z =  array[ index ].z;

        if(array[ index ].x != 0 || array[ index ].y != 0 || array[ index ].z != 0)
            normal = glm::normalize( normal );
        else
            normal = glm::vec3( 0, 1, 0 );
    }

    uint8_t reverse(uint8_t b) {
        b = (b & 0b11110000) >> 4 | (b & 0b00001111) << 4;
        b = (b & 0b11001100) >> 2 | (b & 0b00110011) << 2;
        b = (b & 0b10101010) >> 1 | (b & 0b01010101) << 1;
        return b;
    }
}

glm::u8vec4 Data::Mission::ObjResource::FaceType::getColor( Material material ) const {
    glm::u8vec4 color;

    const uint_fast16_t max_number = 0xFF;

    if( material.polygon_color_type == VertexColorMode::FULL ) {
        if( material.visability == ADDITION) {
            color.r = std::min( static_cast<uint_fast16_t>(static_cast<uint_fast16_t>(opcodes[1]) * 2), max_number );
            color.g = std::min( static_cast<uint_fast16_t>(static_cast<uint_fast16_t>(opcodes[2]) * 2), max_number );
            color.b = std::min( static_cast<uint_fast16_t>(static_cast<uint_fast16_t>(opcodes[3]) * 2), max_number );
        }
        else {
            color.r = std::min( static_cast<uint_fast16_t>(static_cast<uint_fast16_t>(opcodes[1]) * 2), max_number );
            color.g = std::min( static_cast<uint_fast16_t>(static_cast<uint_fast16_t>(opcodes[2]) * 2), max_number );
            color.b = 0;
        }
    }
    else {
        color.r = max_number;
        color.g = max_number;
        color.b = max_number;
    }

    color.a = max_number;

    return color;
}

void Data::Mission::ObjResource::Triangle::switchPoints() {
    std::swap(points[0], points[2]);
}

void Data::Mission::ObjResource::Triangle::generateNormals() {
    points[0].normal = Utilities::ModelBuilder::normalizeFromTriangle( points[0].position, points[1].position, points[2].position );
    points[1].normal = points[0].normal;
    points[2].normal = points[0].normal;
}

void Data::Mission::ObjResource::MorphTriangle::generateNormals() {
    points[0].normal = Utilities::ModelBuilder::normalizeFromTriangle( points[0].position, points[1].position, points[2].position );
    points[1].normal = points[0].normal;
    points[2].normal = points[0].normal;
}

uint32_t Data::Mission::ObjResource::Primitive::getBmpID() const {
    if( !visual.uses_texture || face_type_r == nullptr )
        return 0;
    else
        return face_type_r->bmp_id + 1;
}

bool Data::Mission::ObjResource::Primitive::isWithinBounds( uint32_t vertex_limit, uint32_t normal_limit ) const {
    bool is_valid = true;

    if( this->v[0] >= vertex_limit )
        is_valid = false;
    else
    if( this->v[1] >= vertex_limit )
        is_valid = false;
    else
    if( this->v[2] >= vertex_limit )
        is_valid = false;
    else
    if( normal_limit != 0 && this->n[0] >= normal_limit )
        is_valid = false;
    else
    if( normal_limit != 0 && this->n[1] >= normal_limit )
        is_valid = false;
    else
    if( normal_limit != 0 && this->n[2] >= normal_limit )
        is_valid = false;

    return is_valid;
}

bool Data::Mission::ObjResource::Primitive::operator() ( const Primitive & l_operand, const Primitive & r_operand ) const {
    if( l_operand.getBmpID() != r_operand.getBmpID() )
        return (l_operand.getBmpID() < r_operand.getBmpID());
    else
        return (l_operand.visual.visability < r_operand.visual.visability);
}

int Data::Mission::ObjResource::Primitive::setCircle(const VertexData& vertex_data, std::vector<Triangle> &triangles, std::vector<MorphTriangle> &morph_triangles, const std::vector<Bone> &bones) const {
    Triangle triangle;
    MorphTriangle morph_triangle;
    int16_t     tex_animation_index[2][3];
    glm::u8vec4 weights, joints;
    glm::vec3 center, morph_center;
    float morph_length_90d;

    // Future Cop only uses one joint, so it only needs one weight.
    weights.x = 0xFF;
    weights.y = weights.z = weights.w = 0;

    // The joint needs to be set to zero.
    joints.x = joints.y = joints.z = joints.w = 0;

    triangle.bmp_id = getBmpID();
    triangle.visual = visual;

    triangle.points[0].normal = glm::vec3(0, 1, 0);
    triangle.points[1].normal = glm::vec3(0, 1, 0);
    triangle.points[2].normal = glm::vec3(0, 1, 0);

    const uint32_t id_position = vertex_data.get3DRFItem(VertexData::C_4DVL, 0);
    const glm::i16vec3* const positions_r = vertex_data.get4DVLPointer(id_position);
    const uint32_t id_length = vertex_data.get3DRFItem(VertexData::C_3DRL, 0);
    const uint16_t* const lengths_r = vertex_data.get3DRLPointer(id_length);

    // face_type_offset // Unknown. They range from 4, 8, and 12.

    // this->v[0] // Actual vertex index. Origin of circle.

    // this->v[1] // Red
    // this->v[2] // Green
    // this->v[3] // Blue

    // this->n[0] // Actual length index. Radius of circle.

    // this->n[1] // Always Zero
    // this->n[2] // Always Zero
    // this->n[3] // Always Zero

    handlePositions( center, positions_r, v[0] );
    const float length_90d = lengths_r[ n[0] ] * FIXED_POINT_UNIT;

    // Bone animation.
    if( !bones.empty() ) {
        for( auto bone = bones.begin(); bone != bones.end(); bone++) {
            if( (*bone).vertex_start > v[0] ) {
                break;
            }
            else if( (*bone).vertex_start + (*bone).vertex_stride > v[0] )
            {
                joints.x = bone - bones.begin();
            }
        }
    }
    for( unsigned i = 0; i < 3; i++ ) {
        triangle.points[i].joints = joints;
        triangle.points[i].weights = weights;
    }

    triangle.color = glm::u8vec4( v[1], v[2], v[3], 0xff );

    const float UNIT_45_DEGREES = 0.707106781187; // M_SQRT2 / 2.0;

    const glm::vec2 circle_quadrant[2][3] = {
        // Triangle 0
            { { UNIT_45_DEGREES, UNIT_45_DEGREES}, { 0, 0}, { 0, 1} },
        // Triangle 1
            { { 1, 0}, { 0, 0}, {UNIT_45_DEGREES, UNIT_45_DEGREES} }
    };

    const glm::mat2 rotate_90d = { {0, -1}, {1, 0} };

    for( unsigned axis = 0; axis < 3; axis++ ) {
        glm::vec2 q[2][3] = {
            // Triangle 0
                { circle_quadrant[0][0], circle_quadrant[0][1], circle_quadrant[0][2] },
            // Triangle 1
                { circle_quadrant[1][0], circle_quadrant[1][1], circle_quadrant[1][2] }
        };

        for( unsigned quadrant = 0; quadrant < 4; quadrant++ ) {
            glm::vec3 mapped_circle_quadrant[2][3] = {};
            glm::vec3 circle_direction;

            switch(axis) {
                case 0:
                    mapped_circle_quadrant[0][0] = {q[0][0].x, 0, q[0][0].y};
                    mapped_circle_quadrant[0][1] = {q[0][1].x, 0, q[0][1].y};
                    mapped_circle_quadrant[0][2] = {q[0][2].x, 0, q[0][2].y};

                    mapped_circle_quadrant[1][0] = {q[1][0].x, 0, q[1][0].y};
                    mapped_circle_quadrant[1][1] = {q[1][1].x, 0, q[1][1].y};
                    mapped_circle_quadrant[1][2] = {q[1][2].x, 0, q[1][2].y};

                    circle_direction = glm::vec3(0, 1, 0);
                    break;

                case 1:
                    mapped_circle_quadrant[0][0] = {q[0][0].x, q[0][0].y, 0};
                    mapped_circle_quadrant[0][1] = {q[0][1].x, q[0][1].y, 0};
                    mapped_circle_quadrant[0][2] = {q[0][2].x, q[0][2].y, 0};

                    mapped_circle_quadrant[1][0] = {q[1][0].x, q[1][0].y, 0};
                    mapped_circle_quadrant[1][1] = {q[1][1].x, q[1][1].y, 0};
                    mapped_circle_quadrant[1][2] = {q[1][2].x, q[1][2].y, 0};

                    circle_direction = glm::vec3(0, 0, -1);
                    break;

                case 2:
                    mapped_circle_quadrant[0][0] = {0, q[0][0].x, q[0][0].y};
                    mapped_circle_quadrant[0][1] = {0, q[0][1].x, q[0][1].y};
                    mapped_circle_quadrant[0][2] = {0, q[0][2].x, q[0][2].y};

                    mapped_circle_quadrant[1][0] = {0, q[1][0].x, q[1][0].y};
                    mapped_circle_quadrant[1][1] = {0, q[1][1].x, q[1][1].y};
                    mapped_circle_quadrant[1][2] = {0, q[1][2].x, q[1][2].y};

                    circle_direction = glm::vec3(-1, 0, 0);
                    break;
            }

            // Triangle 0
            for( unsigned i = 0; i < 3; i++ ) {
                triangle.points[i].position = center + length_90d * mapped_circle_quadrant[0][i];
                triangle.points[i].normal = circle_direction;
                triangle.points[i].coords = glm::u8vec2( 0x00, 0x00 );
                triangle.points[i].face_override_index = 0;
            }

            triangles.push_back( triangle );

            for( unsigned morph_frames = 0; morph_frames < vertex_data.get3DRFSize() - 1; morph_frames++ ) {
                const uint32_t id_position = vertex_data.get3DRFItem(VertexData::C_4DVL, 1 + morph_frames);
                const glm::i16vec3* const anm_positions_r = vertex_data.get4DVLPointer(id_position);
                const uint32_t id_length = vertex_data.get3DRFItem(VertexData::C_3DRL, 1 + morph_frames);
                const uint16_t* const anm_lengths_r = vertex_data.get3DRLPointer(id_length);

                handlePositions( morph_center, anm_positions_r, v[0] );
                morph_length_90d = anm_lengths_r[ n[0] ] * FIXED_POINT_UNIT;

                for( unsigned i = 0; i < 3; i++ ) {
                    morph_triangle.points[i].position = morph_center + morph_length_90d * mapped_circle_quadrant[0][i];
                    morph_triangle.points[i].normal = circle_direction;
                }

                morph_triangles.push_back( morph_triangle );
            }

            triangle.switchPoints();

            circle_direction = -circle_direction;
            for( unsigned i = 0; i < 3; i++ ) {
                triangle.points[i].normal = circle_direction;
            }

            triangles.push_back( triangle );

            for( unsigned morph_frames = 0; morph_frames < vertex_data.get3DRFSize() - 1; morph_frames++ ) {
                const uint32_t id_position = vertex_data.get3DRFItem(VertexData::C_4DVL, 1 + morph_frames);
                const glm::i16vec3* const anm_positions_r = vertex_data.get4DVLPointer(id_position);
                const uint32_t id_length = vertex_data.get3DRFItem(VertexData::C_3DRL, 1 + morph_frames);
                const uint16_t* const anm_lengths_r = vertex_data.get3DRLPointer(id_length);

                handlePositions( morph_center, anm_positions_r, v[0] );
                morph_length_90d = anm_lengths_r[ n[0] ] * FIXED_POINT_UNIT;

                for( unsigned i = 0; i < 3; i++ ) {
                    morph_triangle.points[i].position = morph_center + morph_length_90d * mapped_circle_quadrant[0][2 - i];
                    morph_triangle.points[i].normal = circle_direction;
                }

                morph_triangles.push_back( morph_triangle );
            }

            circle_direction = -circle_direction;

            // Triangle 1
            for( unsigned i = 0; i < 3; i++ ) {
                triangle.points[i].position = center + length_90d * mapped_circle_quadrant[1][i];
                triangle.points[i].normal = circle_direction;
                triangle.points[i].coords = glm::u8vec2( 0x00, 0x00 );
                triangle.points[i].face_override_index = 0;
            }

            triangles.push_back( triangle );

            for( unsigned morph_frames = 0; morph_frames < vertex_data.get3DRFSize() - 1; morph_frames++ ) {
                const uint32_t id_position = vertex_data.get3DRFItem(VertexData::C_4DVL, 1 + morph_frames);
                const glm::i16vec3* const anm_positions_r = vertex_data.get4DVLPointer(id_position);
                const uint32_t id_length = vertex_data.get3DRFItem(VertexData::C_3DRL, 1 + morph_frames);
                const uint16_t* const anm_lengths_r = vertex_data.get3DRLPointer(id_length);

                handlePositions( morph_center, anm_positions_r, v[0] );
                morph_length_90d = anm_lengths_r[ n[0] ] * FIXED_POINT_UNIT;

                for( unsigned i = 0; i < 3; i++ ) {
                    morph_triangle.points[i].position = morph_center + morph_length_90d * mapped_circle_quadrant[1][i];
                    morph_triangle.points[i].normal = circle_direction;
                }

                morph_triangles.push_back( morph_triangle );
            }

            triangle.switchPoints();
            circle_direction = -circle_direction;
            for( unsigned i = 0; i < 3; i++ ) {
                triangle.points[i].normal = circle_direction;
            }
            triangles.push_back( triangle );

            for( unsigned morph_frames = 0; morph_frames < vertex_data.get3DRFSize() - 1; morph_frames++ ) {
                const uint32_t id_position = vertex_data.get3DRFItem(VertexData::C_4DVL, 1 + morph_frames);
                const glm::i16vec3* const anm_positions_r = vertex_data.get4DVLPointer(id_position);
                const uint32_t id_length = vertex_data.get3DRFItem(VertexData::C_3DRL, 1 + morph_frames);
                const uint16_t* const anm_lengths_r = vertex_data.get3DRLPointer(id_length);

                handlePositions( morph_center, anm_positions_r, v[0] );
                morph_length_90d = anm_lengths_r[ n[0] ] * FIXED_POINT_UNIT;

                for( unsigned i = 0; i < 3; i++ ) {
                    morph_triangle.points[i].position = morph_center + morph_length_90d * mapped_circle_quadrant[1][2 - i];
                    morph_triangle.points[i].normal = circle_direction;
                }

                morph_triangles.push_back( morph_triangle );
            }

            for(unsigned x = 0; x < 2; x++) {
                for(unsigned y = 0; y < 3; y++) {
                    q[x][y] = rotate_90d * q[x][y];
                }
            }
        }
    }

    return getTriangleAmount( PrimitiveType::STAR );
}

int Data::Mission::ObjResource::Primitive::setTriangle(const VertexData& vertex_data, std::vector<Triangle> &triangles, std::vector<MorphTriangle> &morph_triangles, const std::vector<Bone> &bones) const {
    if( !isWithinBounds( vertex_data.get4DVLSize(), vertex_data.get4DNLSize() ) )
        return 0;

    Triangle triangle;
    MorphTriangle morph_triangle;
    glm::u8vec2 coords[3];
    int16_t face_override_indexes[3];
    glm::u8vec4 weights, joints;

    // Future Cop only uses one joint, so it only needs one weight.
    weights.x = 0xFF;
    weights.y = weights.z = weights.w = 0;

    // The joint needs to be set to zero.
    joints.x = joints.y = joints.z = joints.w = 0;

    triangle.bmp_id = getBmpID();
    triangle.visual = visual;

    triangle.color = glm::u8vec4( 0xff, 0xff, 0xff, 0xff );

    const uint32_t id_position = vertex_data.get3DRFItem(VertexData::C_4DVL, 0);
    const glm::i16vec3* const positions_r = vertex_data.get4DVLPointer(id_position);

    handlePositions( triangle.points[0].position, positions_r, v[2] );
    handlePositions( triangle.points[1].position, positions_r, v[1] );
    handlePositions( triangle.points[2].position, positions_r, v[0] );

    if( visual.normal_shading && vertex_data.get4DNLSize() > 0 ) {
        const uint32_t id_normal = vertex_data.get3DRFItem(VertexData::C_4DNL, 0);
        const glm::i16vec3* const normals_r = vertex_data.get4DNLPointer(id_normal);

        handleNormals( triangle.points[0].normal, normals_r, n[2] );
        handleNormals( triangle.points[1].normal, normals_r, n[1] );
        handleNormals( triangle.points[2].normal, normals_r, n[0] );
    }
    else
        triangle.generateNormals();

    triangleToCoords( *this, *face_type_r, coords, face_override_indexes );

    if( face_type_r != nullptr ) {
        triangle.color = face_type_r->getColor( triangle.visual );
    }

    for( unsigned t = 0; t < 3; t++ ) {
        triangle.points[t].coords = coords[2 - t];
        triangle.points[t].face_override_index = face_override_indexes[2 - t];
    }

    for( unsigned morph_frames = 0; morph_frames < vertex_data.get3DRFSize() - 1; morph_frames++ ) {
        const uint32_t id_position = vertex_data.get3DRFItem(VertexData::C_4DVL, 1 + morph_frames);
        const glm::i16vec3* const anm_positions_r = vertex_data.get4DVLPointer(id_position);

        handlePositions( morph_triangle.points[0].position, anm_positions_r, v[2] );
        handlePositions( morph_triangle.points[1].position, anm_positions_r, v[1] );
        handlePositions( morph_triangle.points[2].position, anm_positions_r, v[0] );

        if( visual.normal_shading && vertex_data.get4DNLSize() > 0 ) {
            const uint32_t id_normal = vertex_data.get3DRFItem(VertexData::C_4DNL, 1 + morph_frames);
            const glm::i16vec3* const anm_normals_r = vertex_data.get4DNLPointer(id_normal);

            handleNormals( morph_triangle.points[0].normal, anm_normals_r, n[2] );
            handleNormals( morph_triangle.points[1].normal, anm_normals_r, n[1] );
            handleNormals( morph_triangle.points[2].normal, anm_normals_r, n[0] );
        }
        else
            morph_triangle.generateNormals();

        morph_triangles.push_back( morph_triangle );
    }
    if( !bones.empty() ) {
        for( unsigned t = 0; t < 3; t++ ) {
            triangle.points[2 - t].joints = glm::u8vec4(0,0,0,0);

            for( auto bone = bones.begin(); bone != bones.end(); bone++) {
                if( (*bone).vertex_start > v[t] ) {
                    break;
                }
                else if( (*bone).vertex_start + (*bone).vertex_stride > v[t] )
                {
                    joints.x = bone - bones.begin();
                    triangle.points[2 - t].joints = joints;
                }
            }
            triangle.points[2 - t].weights = weights;
        }
    }
    triangles.push_back( triangle );

    return getTriangleAmount( PrimitiveType::TRIANGLE );
}

int Data::Mission::ObjResource::Primitive::setQuad(const VertexData& vertex_data, std::vector<Triangle> &triangles, std::vector<MorphTriangle> &morph_triangles, const std::vector<Bone> &bones) const {
    const PrimitiveType TYPES[] = {PrimitiveType::TRIANGLE, PrimitiveType::TRIANGLE_OTHER};

    Primitive new_tri;

    new_tri.visual = visual;

    new_tri.face_type_offset = face_type_offset;
    new_tri.face_type_r = face_type_r;

    new_tri.v[3] = 0;
    new_tri.n[3] = 0;

    for( unsigned i = 0; i < 2; i++ ) {
        new_tri.type = TYPES[i];

        new_tri.v[0] = v[QUAD_TABLE[i][0]];
        new_tri.v[1] = v[QUAD_TABLE[i][1]];
        new_tri.v[2] = v[QUAD_TABLE[i][2]];

        new_tri.n[0] = n[QUAD_TABLE[i][0]];
        new_tri.n[1] = n[QUAD_TABLE[i][1]];
        new_tri.n[2] = n[QUAD_TABLE[i][2]];

        new_tri.setTriangle(vertex_data, triangles, morph_triangles, bones);
    }

    return getTriangleAmount( PrimitiveType::QUAD );
}

int Data::Mission::ObjResource::Primitive::setBillboard(const VertexData& vertex_data, std::vector<Triangle> &triangles, std::vector<MorphTriangle> &morph_triangles, const std::vector<Bone> &bones) const {
    Triangle triangle;
    MorphTriangle morph_triangle;
    glm::u8vec2 coords[2][3];
    int16_t     tex_animation_index[2][3];
    glm::u8vec4 weights, joints;
    glm::vec3 center, morph_center;
    float length, morph_length;

    const glm::vec3 billboard_star[3][4] = {
        // Quad 0
            { { 1.0, 1.0, 0.0}, {-1.0, 1.0, 0.0}, {-1.0,-1.0, 0.0}, { 1.0,-1.0, 0.0} },
        // Quad 1
            { { 1.0, 0.0, 1.0}, {-1.0, 0.0, 1.0}, {-1.0, 0.0,-1.0}, { 1.0, 0.0,-1.0} },
        // Quad 2
            { { 0.0, 1.0, 1.0}, { 0.0,-1.0, 1.0}, { 0.0,-1.0,-1.0}, { 0.0, 1.0,-1.0} }
    };

    const glm::vec3 billboard_star_normals[3] = {
        {0,  0, 1},
        {0, -1, 0},
        {1,  0, 0}
    };

    // Future Cop only uses one joint, so it only needs one weight.
    weights.x = 0xFF;
    weights.y = weights.z = weights.w = 0;

    // The joint needs to be set to zero.
    joints.x = joints.y = joints.z = joints.w = 0;

    triangle.bmp_id = getBmpID();
    triangle.visual = visual;

    if( face_type_r != nullptr ) {
        triangle.color = face_type_r->getColor( triangle.visual );

        coords[0][0] = face_type_r->coords[QUAD_TABLE[0][0]];
        coords[0][1] = face_type_r->coords[QUAD_TABLE[0][1]];
        coords[0][2] = face_type_r->coords[QUAD_TABLE[0][2]];
        coords[1][0] = face_type_r->coords[QUAD_TABLE[1][0]];
        coords[1][1] = face_type_r->coords[QUAD_TABLE[1][1]];
        coords[1][2] = face_type_r->coords[QUAD_TABLE[1][2]];
    }
    else {
        triangle.color = glm::u8vec4( 0xff, 0xff, 0xff, 0xff );

        coords[0][0] = glm::u8vec2( 0x00, 0x00 );
        coords[0][1] = glm::u8vec2( 0xFF, 0x00 );
        coords[0][2] = glm::u8vec2( 0xFF, 0xFF );
        coords[1][0] = glm::u8vec2( 0x00, 0x00 );
        coords[1][1] = glm::u8vec2( 0x00, 0xFF );
        coords[1][2] = glm::u8vec2( 0xFF, 0xFF );
    }

    if(face_type_r != nullptr && face_type_r->face_override_index != 0) {
        tex_animation_index[0][0] = 4 * (face_type_r->face_override_index - 1) + QUAD_TABLE[0][0] + 1;
        tex_animation_index[0][1] = 4 * (face_type_r->face_override_index - 1) + QUAD_TABLE[0][1] + 1;
        tex_animation_index[0][2] = 4 * (face_type_r->face_override_index - 1) + QUAD_TABLE[0][2] + 1;
        tex_animation_index[1][0] = 4 * (face_type_r->face_override_index - 1) + QUAD_TABLE[1][0] + 1;
        tex_animation_index[1][1] = 4 * (face_type_r->face_override_index - 1) + QUAD_TABLE[1][1] + 1;
        tex_animation_index[1][2] = 4 * (face_type_r->face_override_index - 1) + QUAD_TABLE[1][2] + 1;
    }
    else {
        // No tex animation index.
        for(int i = 0; i < 2; i++) {
            for(int t = 0; t < 3; t++) {
                tex_animation_index[i][t] = 0;
            }
        }
    }

    const uint32_t id_position = vertex_data.get3DRFItem(VertexData::C_4DVL, 0);
    const glm::i16vec3* const positions_r = vertex_data.get4DVLPointer(id_position);
    const uint32_t id_length = vertex_data.get3DRFItem(VertexData::C_3DRL, 0);
    const uint16_t* const lengths_r = vertex_data.get3DRLPointer(id_length);

     // v[0] is a vertex position and v[2] is a width offset. v[1] and v[3] are just 0xFF. All normals are 0 probably unused.
    handlePositions( center, positions_r, v[0] );
    length = lengths_r[ v[2] ] * FIXED_POINT_UNIT;

    // Bone animation.
    if( !bones.empty() ) {
        for( auto bone = bones.begin(); bone != bones.end(); bone++) {
            if( (*bone).vertex_start > v[0] ) {
                break;
            }
            else if( (*bone).vertex_start + (*bone).vertex_stride > v[0] )
            {
                joints.x = bone - bones.begin();
            }
        }
    }
    for( unsigned i = 0; i < 3; i++ ) {
        triangle.points[i].joints = joints;
        triangle.points[i].weights = weights;
    }

    for( unsigned quad_index = 0; quad_index < 3; quad_index++ ) {
        // Triangle 0
        for( unsigned i = 0; i < 3; i++ ) {
            triangle.points[i].position = center + length * billboard_star[quad_index][QUAD_TABLE[0][i]];
            triangle.points[i].normal = billboard_star_normals[quad_index];
            triangle.points[i].coords = coords[0][i];
            triangle.points[i].face_override_index = tex_animation_index[0][i];
        }

        triangles.push_back( triangle );

        for( unsigned morph_frames = 0; morph_frames < vertex_data.get3DRFSize() - 1; morph_frames++ ) {
            const uint32_t id_position = vertex_data.get3DRFItem(VertexData::C_4DVL, 1 + morph_frames);
            const glm::i16vec3* const anm_positions_r = vertex_data.get4DVLPointer(id_position);
            const uint32_t id_length = vertex_data.get3DRFItem(VertexData::C_3DRL, 1 + morph_frames);
            const uint16_t* const anm_lengths_r = vertex_data.get3DRLPointer(id_length);

            handlePositions( morph_center, anm_positions_r, v[0] );
            morph_length = anm_lengths_r[ v[2] ] * FIXED_POINT_UNIT;

            for( unsigned i = 0; i < 3; i++ ) {
                morph_triangle.points[i].position = morph_center + morph_length * billboard_star[quad_index][QUAD_TABLE[0][i]];
                morph_triangle.points[i].normal = billboard_star_normals[quad_index];
            }

            morph_triangles.push_back( morph_triangle );
        }

        triangle.switchPoints();
        for( unsigned i = 0; i < 3; i++ ) {
            triangle.points[i].normal = -billboard_star_normals[quad_index];
        }
        triangles.push_back( triangle );

        for( unsigned morph_frames = 0; morph_frames < vertex_data.get3DRFSize() - 1; morph_frames++ ) {
            const uint32_t id_position = vertex_data.get3DRFItem(VertexData::C_4DVL, 1 + morph_frames);
            const glm::i16vec3* const anm_positions_r = vertex_data.get4DVLPointer(id_position);
            const uint32_t id_length = vertex_data.get3DRFItem(VertexData::C_3DRL, 1 + morph_frames);
            const uint16_t* const anm_lengths_r = vertex_data.get3DRLPointer(id_length);

            handlePositions( morph_center, anm_positions_r, v[0] );
            morph_length = anm_lengths_r[ v[2] ] * FIXED_POINT_UNIT;

            for( unsigned i = 0; i < 3; i++ ) {
                morph_triangle.points[i].position = morph_center + morph_length * billboard_star[quad_index][QUAD_TABLE[0][2 - i]];
                morph_triangle.points[i].normal = -billboard_star_normals[quad_index];
            }

            morph_triangles.push_back( morph_triangle );
        }

        // Triangle 1
        for( unsigned i = 0; i < 3; i++ ) {
            triangle.points[i].position = center + length * billboard_star[quad_index][QUAD_TABLE[1][i]];
            triangle.points[i].normal = billboard_star_normals[quad_index];
            triangle.points[i].coords = coords[1][i];
            triangle.points[i].face_override_index = tex_animation_index[1][i];
        }

        triangles.push_back( triangle );

        for( unsigned morph_frames = 0; morph_frames < vertex_data.get3DRFSize() - 1; morph_frames++ ) {
            const uint32_t id_position = vertex_data.get3DRFItem(VertexData::C_4DVL, 1 + morph_frames);
            const glm::i16vec3* const anm_positions_r = vertex_data.get4DVLPointer(id_position);
            const uint32_t id_length = vertex_data.get3DRFItem(VertexData::C_3DRL, 1 + morph_frames);
            const uint16_t* const anm_lengths_r = vertex_data.get3DRLPointer(id_length);

            handlePositions( morph_center, anm_positions_r, v[0] );
            morph_length = anm_lengths_r[ v[2] ] * FIXED_POINT_UNIT;

            for( unsigned i = 0; i < 3; i++ ) {
                morph_triangle.points[i].position = morph_center + morph_length * billboard_star[quad_index][QUAD_TABLE[1][i]];
                morph_triangle.points[i].normal = billboard_star_normals[quad_index];
            }

            morph_triangles.push_back( morph_triangle );
        }

        triangle.switchPoints();
        for( unsigned i = 0; i < 3; i++ ) {
            triangle.points[i].normal = -billboard_star_normals[quad_index];
        }
        triangles.push_back( triangle );

        for( unsigned morph_frames = 0; morph_frames < vertex_data.get3DRFSize() - 1; morph_frames++ ) {
            const uint32_t id_position = vertex_data.get3DRFItem(VertexData::C_4DVL, 1 + morph_frames);
            const glm::i16vec3* const anm_positions_r = vertex_data.get4DVLPointer(id_position);
            const uint32_t id_length = vertex_data.get3DRFItem(VertexData::C_3DRL, 1 + morph_frames);
            const uint16_t* const anm_lengths_r = vertex_data.get3DRLPointer(id_length);

            handlePositions( morph_center, anm_positions_r, v[0] );
            morph_length = anm_lengths_r[ v[2] ] * FIXED_POINT_UNIT;

            for( unsigned i = 0; i < 3; i++ ) {
                morph_triangle.points[i].position = morph_center + morph_length * billboard_star[quad_index][QUAD_TABLE[1][2 - i]];
                morph_triangle.points[i].normal = -billboard_star_normals[quad_index];
            }

            morph_triangles.push_back( morph_triangle );
        }
    }

    return getTriangleAmount( PrimitiveType::BILLBOARD );
}

int Data::Mission::ObjResource::Primitive::setLine(const VertexData& vertex_data, std::vector<Triangle> &triangles, std::vector<MorphTriangle> &morph_triangles, const std::vector<Bone> &bones) const {
    Triangle      triangle;
    MorphTriangle morph_triangle;
    glm::u8vec2   coords[2][3];
    int16_t       tex_animation_index[2][3];

    triangle.bmp_id = getBmpID();
    triangle.visual = visual;

    triangle.points[0].normal = glm::vec3(0, 1, 0);
    triangle.points[1].normal = glm::vec3(0, 1, 0);
    triangle.points[2].normal = glm::vec3(0, 1, 0);
    triangle.points[0].weights = glm::u8vec4(0xFF, 0x00, 0x00, 0x00);
    triangle.points[1].weights = glm::u8vec4(0xFF, 0x00, 0x00, 0x00);
    triangle.points[2].weights = glm::u8vec4(0xFF, 0x00, 0x00, 0x00);
    morph_triangle.points[0].normal = triangle.points[0].normal;
    morph_triangle.points[1].normal = triangle.points[1].normal;
    morph_triangle.points[2].normal = triangle.points[2].normal;

    if( face_type_r != nullptr ) {
        triangle.color = face_type_r->getColor( triangle.visual );

        coords[0][0] = face_type_r->coords[QUAD_TABLE[0][0]];
        coords[0][1] = face_type_r->coords[QUAD_TABLE[0][1]];
        coords[0][2] = face_type_r->coords[QUAD_TABLE[0][2]];
        coords[1][0] = face_type_r->coords[QUAD_TABLE[1][0]];
        coords[1][1] = face_type_r->coords[QUAD_TABLE[1][1]];
        coords[1][2] = face_type_r->coords[QUAD_TABLE[1][2]];
    }
    else {
        triangle.color = glm::u8vec4( 0xff, 0xff, 0xff, 0xff );

        coords[0][0] = glm::u8vec2( 0x00, 0x00 );
        coords[0][1] = glm::u8vec2( 0xFF, 0x00 );
        coords[0][2] = glm::u8vec2( 0xFF, 0xFF );
        coords[1][0] = glm::u8vec2( 0x00, 0x00 );
        coords[1][1] = glm::u8vec2( 0x00, 0xFF );
        coords[1][2] = glm::u8vec2( 0xFF, 0xFF );
    }

    if(face_type_r != nullptr && face_type_r->face_override_index != 0) {
        tex_animation_index[0][0] = 4 * (face_type_r->face_override_index - 1) + QUAD_TABLE[0][0] + 1;
        tex_animation_index[0][1] = 4 * (face_type_r->face_override_index - 1) + QUAD_TABLE[0][1] + 1;
        tex_animation_index[0][2] = 4 * (face_type_r->face_override_index - 1) + QUAD_TABLE[0][2] + 1;
        tex_animation_index[1][0] = 4 * (face_type_r->face_override_index - 1) + QUAD_TABLE[1][0] + 1;
        tex_animation_index[1][1] = 4 * (face_type_r->face_override_index - 1) + QUAD_TABLE[1][1] + 1;
        tex_animation_index[1][2] = 4 * (face_type_r->face_override_index - 1) + QUAD_TABLE[1][2] + 1;
    }
    else {
        // No tex animation index.
        for(int i = 0; i < 2; i++) {
            for(int t = 0; t < 3; t++) {
                tex_animation_index[i][t] = 0;
            }
        }
    }

    glm::vec3 segments[2];
    glm::vec3 &offset = segments[0];

    const uint32_t id_position = vertex_data.get3DRFItem(VertexData::C_4DVL, 0);
    const glm::i16vec3* const positions_r = vertex_data.get4DVLPointer(id_position);
    const uint32_t id_length = vertex_data.get3DRFItem(VertexData::C_3DRL, 0);
    const uint16_t* const lengths_r = vertex_data.get3DRLPointer(id_length);

    // v[0] and v[1] are vertex position offsets, v[2] and v[3] are width offsets. All normals are 0 probably unused.
    handlePositions( segments[0], positions_r, v[0] );
    handlePositions( segments[1], positions_r, v[1] );

    const float thickness[2] = {
        lengths_r[ v[2] ] * FIXED_POINT_UNIT,
        lengths_r[ v[3] ] * FIXED_POINT_UNIT };

    glm::u8vec4 joints[2];

    for( unsigned q = 0; q < 2; q++ ) {
        joints[q]  = glm::u8vec4(0,0,0,0);

        for( auto bone = bones.begin(); bone != bones.end(); bone++) {
            if( (*bone).vertex_start > v[q] ) {
                break;
            }
            else if( (*bone).vertex_start + (*bone).vertex_stride > v[q] )
            {
                joints[q].x = bone - bones.begin();
            }
        }
    }

    const glm::vec3 unnormalized = segments[1] - offset;
    const glm::ivec2 placements[3] = {
        glm::vec2( 1, 0 ),
        glm::vec2( 2, 0 ),
        glm::vec2( 2, 1 ) };
    const glm::vec2 othro[3] = {
        glm::vec2( unnormalized.x, unnormalized.y ),
        glm::vec2( unnormalized.x, unnormalized.z ),
        glm::vec2( unnormalized.y, unnormalized.z ) };

    // Calculate 3 lengths calculated from 2D planes of the lines in an othrographic perspective. TOP, RIGHT, and FRONT
    const float othro_lengths[3] = {
        glm::length( othro[0] ),
        glm::length( othro[1] ),
        glm::length( othro[2] ) };

    // Choose the longest length to generate first quaderlateral.
    unsigned selected_indexes[2] = {0, 0};
    float current_length = -1.0;

    for( unsigned i = 0; i < 3; i++ ) {
        if( othro_lengths[i] > current_length ) {
            selected_indexes[0] = i;
            current_length = othro_lengths[i];
        }
    }

    // Choose the 2nd longest length to generate second quaderlateral.
    current_length = -1.0;

    for( unsigned i = 0; i < 3; i++ ) {
        if( i != selected_indexes[0] &&
            othro_lengths[i] > current_length ) {
            selected_indexes[1] = i;
            current_length = othro_lengths[i];
        }
    }

    assert( selected_indexes[0] != selected_indexes[1] );
    assert( othro_lengths[selected_indexes[0]] >= othro_lengths[selected_indexes[1]] );

    for( unsigned q = 0; q < 2; q++ ) {
        const glm::vec2  &current_othro = othro[selected_indexes[q]];
        const glm::vec2   current_othro_normal = glm::normalize(current_othro);
        const glm::vec2   current_othro_axis = current_othro_normal * glm::vec2(-1.0, 1.0);
        const glm::ivec2 &current_placement = placements[selected_indexes[q]];

        glm::vec3 quaderlateral[4];

        glm::vec3 flat_3 = glm::vec3(0, 0, 0);
        glm::vec2 flat_2;
        flat_2 = thickness[0] * current_othro_axis;
        flat_3[ current_placement.x ] = flat_2.x;
        flat_3[ current_placement.y ] = flat_2.y;
        quaderlateral[0] = flat_3;

        flat_2 = thickness[0] * -current_othro_axis;
        flat_3[ current_placement.x ] = flat_2.x;
        flat_3[ current_placement.y ] = flat_2.y;
        quaderlateral[1] = flat_3;

        flat_2 = thickness[1] * -current_othro_axis;
        flat_3[ current_placement.x ] = flat_2.x;
        flat_3[ current_placement.y ] = flat_2.y;
        quaderlateral[2] = flat_3;

        flat_2 = thickness[1] *  current_othro_axis;
        flat_3[ current_placement.x ] = flat_2.x;
        flat_3[ current_placement.y ] = flat_2.y;
        quaderlateral[3] = flat_3;

        glm::vec3 current_normal = Utilities::ModelBuilder::normalizeFromTriangle(
            segments[QUAD_TABLE[0][0] / 2] + quaderlateral[QUAD_TABLE[0][0]],
            segments[QUAD_TABLE[0][1] / 2] + quaderlateral[QUAD_TABLE[0][1]],
            segments[QUAD_TABLE[0][2] / 2] + quaderlateral[QUAD_TABLE[0][2]]);

        for( unsigned t = 0; t < 2; t++ ) {
            for( unsigned i = 0; i < 3; i++ ) {
                triangle.points[i].position = segments[QUAD_TABLE[t][i] / 2] + quaderlateral[QUAD_TABLE[t][i]];
                triangle.points[i].normal = current_normal;
                triangle.points[i].coords = coords[t][i];
                triangle.points[i].face_override_index = tex_animation_index[t][i];
                triangle.points[i].joints = joints[QUAD_TABLE[t][i] / 2]; // Untested.
            }
            triangles.push_back( triangle );

            // TODO morph_thickness is not used. Make a test case to fix this.
            for( unsigned morph_frames = 0; morph_frames < vertex_data.get3DRFSize() - 1; morph_frames++ ) {
                const uint32_t id_position = vertex_data.get3DRFItem(VertexData::C_4DVL, 1 + morph_frames);
                const glm::i16vec3* const anm_positions_r = vertex_data.get4DVLPointer(id_position);
                const uint32_t id_length = vertex_data.get3DRFItem(VertexData::C_3DRL, 1 + morph_frames);
                const uint16_t* const anm_lengths_r = vertex_data.get3DRLPointer(id_length);

                glm::vec3 morph_segments[2];
                handlePositions( morph_segments[0], anm_positions_r, v[0] );
                handlePositions( morph_segments[1], anm_positions_r, v[1] );

                const float morph_thickness[2] = {
                    anm_lengths_r[ v[2] ] * FIXED_POINT_UNIT,
                    anm_lengths_r[ v[3] ] * FIXED_POINT_UNIT };

                for( unsigned i = 0; i < 3; i++ )
                    morph_triangle.points[i].position = morph_segments[QUAD_TABLE[t][i] / 2] + quaderlateral[QUAD_TABLE[t][i]];

                morph_triangle.points[0].normal = Utilities::ModelBuilder::normalizeFromTriangle(morph_triangle.points[0].position, morph_triangle.points[1].position, morph_triangle.points[2].position);

                morph_triangle.points[1].normal = morph_triangle.points[0].normal;
                morph_triangle.points[2].normal = morph_triangle.points[0].normal;

                morph_triangles.push_back( morph_triangle );
            }

            triangle.switchPoints();
            for( unsigned i = 0; i < 3; i++ ) {
                triangle.points[i].normal = -current_normal;
            }
            triangles.push_back( triangle );

            // TODO morph_thickness is not used. Make a test case to fix this.
            for( unsigned morph_frames = 0; morph_frames < vertex_data.get3DRFSize() - 1; morph_frames++ ) {
                const uint32_t id_position = vertex_data.get3DRFItem(VertexData::C_4DVL, 1 + morph_frames);
                const glm::i16vec3* const anm_positions_r = vertex_data.get4DVLPointer(id_position);
                const uint32_t id_length = vertex_data.get3DRFItem(VertexData::C_3DRL, 1 + morph_frames);
                const uint16_t* const anm_lengths_r = vertex_data.get3DRLPointer(id_length);

                glm::vec3 morph_segments[2];
                handlePositions( morph_segments[0], anm_positions_r, v[0] );
                handlePositions( morph_segments[1], anm_positions_r, v[1] );

                const float morph_thickness[2] = {
                    anm_lengths_r[ v[2] ] * FIXED_POINT_UNIT,
                    anm_lengths_r[ v[3] ] * FIXED_POINT_UNIT };

                for( unsigned i = 0; i < 3; i++ )
                    morph_triangle.points[i].position = morph_segments[QUAD_TABLE[t][2 - i] / 2] + quaderlateral[QUAD_TABLE[t][2 - i]];

                morph_triangle.points[0].normal = Utilities::ModelBuilder::normalizeFromTriangle(morph_triangle.points[0].position, morph_triangle.points[1].position, morph_triangle.points[2].position);

                morph_triangle.points[1].normal = morph_triangle.points[0].normal;
                morph_triangle.points[2].normal = morph_triangle.points[0].normal;

                morph_triangles.push_back( morph_triangle );
            }
        }
    }

    return getTriangleAmount( PrimitiveType::LINE );
}

size_t Data::Mission::ObjResource::Primitive::getTriangleAmount( PrimitiveType type ) {
    switch( type ) {
        case PrimitiveType::STAR:
            return 48;
        case PrimitiveType::TRIANGLE:
        case PrimitiveType::TRIANGLE_OTHER:
            return 1;
        case PrimitiveType::QUAD:
            return 2;
        case PrimitiveType::BILLBOARD:
            return 12;
        case PrimitiveType::LINE:
            return 8;
        default:
            return 0;
    }
}

Data::Mission::ObjResource::VertexData::VertexData() {
    size_of_4DVL = -1;
    size_of_4DNL = -1;
    size_of_3DRL = -1;
}

uint32_t Data::Mission::ObjResource::VertexData::get3DRFSize() const {
    return reference_ids.size() / 3;
}

void Data::Mission::ObjResource::VertexData::set3DRFSize(uint32_t size) {
    reference_ids.resize(size * 3);
}

void Data::Mission::ObjResource::VertexData::set3DRFItem(Data::Mission::ObjResource::VertexData::Tag tag, uint32_t index, uint32_t id) {
    reference_ids[tag * (reference_ids.size() / 3) + index] = id;
}
uint32_t Data::Mission::ObjResource::VertexData::get3DRFItem(Data::Mission::ObjResource::VertexData::Tag tag, uint32_t index) const {
    return reference_ids[tag * (reference_ids.size() / 3) + index];
}

void Data::Mission::ObjResource::VertexData::set4DVLSize(int32_t size_of_4DVL) {
    this->size_of_4DVL = size_of_4DVL;

    positions.resize(size_of_4DVL * get3DRFSize(), glm::i16vec3(0, 0, 0));
}
void Data::Mission::ObjResource::VertexData::set4DNLSize(int32_t size_of_4DNL) {
    this->size_of_4DNL = size_of_4DNL;

    normals.resize(size_of_4DNL * get3DRFSize(), glm::i16vec3(0, 0x1000, 0));
}
void Data::Mission::ObjResource::VertexData::set3DRLSize(int32_t size_of_3DRL) {
    this->size_of_3DRL = size_of_3DRL;

    lengths.resize(size_of_3DRL * get3DRFSize(), 0x100);
}

int32_t Data::Mission::ObjResource::VertexData::get4DVLSize() const {
    return this->size_of_4DVL;
}
int32_t Data::Mission::ObjResource::VertexData::get4DNLSize() const {
    return this->size_of_4DNL;
}
int32_t Data::Mission::ObjResource::VertexData::get3DRLSize() const {
    return this->size_of_3DRL;
}

glm::i16vec3* Data::Mission::ObjResource::VertexData::get4DVLPointer(uint32_t id) {
    return const_cast<glm::i16vec3*>(const_cast<const VertexData *const>(this)->get4DVLPointer(id));
}

const glm::i16vec3* const Data::Mission::ObjResource::VertexData::get4DVLPointer(uint32_t id) const {
    for(uint32_t index = 0; index < get3DRFSize(); index++)
    {
        if(get3DRFItem(C_4DVL, index) == id)
            return &positions[index * this->size_of_4DVL];
    }

    return nullptr;
}

glm::i16vec3* Data::Mission::ObjResource::VertexData::get4DNLPointer(uint32_t id) {
    return const_cast<glm::i16vec3*>(const_cast<const VertexData *const>(this)->get4DNLPointer(id));
}

const glm::i16vec3* const Data::Mission::ObjResource::VertexData::get4DNLPointer(uint32_t id) const {
    for(uint32_t index = 0; index < get3DRFSize(); index++)
    {
        if(get3DRFItem(C_4DNL, index) == id)
            return &normals[index * this->size_of_4DNL];
    }

    return nullptr;
}

uint16_t* Data::Mission::ObjResource::VertexData::get3DRLPointer(uint32_t id) {
    return const_cast<uint16_t*>(const_cast<const VertexData *const>(this)->get3DRLPointer(id));
}

const uint16_t* const Data::Mission::ObjResource::VertexData::get3DRLPointer(uint32_t id) const {
    for(uint32_t index = 0; index < get3DRFSize(); index++)
    {
        if(get3DRFItem(C_3DRL, index) == id)
            return &lengths[index * this->size_of_3DRL];
    }

    return nullptr;
}

unsigned int Data::Mission::ObjResource::Bone::getNumAttributes() const {
    return (getOpcodeBytesPerFrame( this->opcode ) / 2);
}

std::string Data::Mission::ObjResource::Bone::getString() const {
    std::stringstream form;

    form << "opcode 0b" << opcode.position.x_const << opcode.position.y_const << opcode.position.z_const << opcode.rotation.x_const << opcode.rotation.y_const << opcode.rotation.z_const << "; ";
    form << "parent_level = " << (parent_amount - 1) << "; ";
    form << "normal {start = " << normal_start << ", stride = " << normal_stride << "}; ";
    form << "vertex {start = " << vertex_start << ", stride = " << vertex_stride << "}; ";
    form << "position( " << position.x << ", " << position.y << ", " << position.z << " ); ";
    form << "rotation( " << rotation.x << ", " << rotation.y << ", " << rotation.z << " ) ";

    return form.str();
}
        
const std::string Data::Mission::ObjResource::FILE_EXTENSION = "cobj";
const uint32_t    Data::Mission::ObjResource::IDENTIFIER_TAG = 0x436F626A; // which is { 0x43, 0x6F, 0x62, 0x6A } or { 'C', 'o', 'b', 'j' } or "Cobj"
const std::string Data::Mission::ObjResource::METADATA_COMPONENT_NAME = "_METADATA";

const float Data::Mission::ObjResource::FIXED_POINT_UNIT  = 1.0 / 512.0;
const float Data::Mission::ObjResource::FIXED_NORMAL_UNIT = 1.0 / 4096.0;
const float Data::Mission::ObjResource::ANGLE_UNIT        = glm::pi<float>() / 2048.0;

Data::Mission::ObjResource::ObjResource() {
    this->bone_frames = 0;
    this->max_bone_childern = 0;
    this->bone_animation_data = nullptr;
}

Data::Mission::ObjResource::~ObjResource() {
    delete [] this->bone_animation_data;
} 

std::string Data::Mission::ObjResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::ObjResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool Data::Mission::ObjResource::parse( const ParseSettings &settings ) {
    auto debug_log = settings.logger_r->getLog( Utilities::Logger::DEBUG );
    debug_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";
    auto warning_log = settings.logger_r->getLog( Utilities::Logger::WARNING );
    warning_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";
    auto error_log = settings.logger_r->getLog( Utilities::Logger::ERROR );
    error_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

    if( this->data_p != nullptr )
    {
        auto reader = this->data_p->getReader();

        bool file_is_not_valid = true;
        
        // This is for testing mostly.
        int bytes_per_frame_3DMI =  0;
        int frames_gen_3DHS      = -1;
        uint16_t num_frames_4DGI =  0;

        while(reader.getPosition( Utilities::Buffer::BEGIN ) < reader.totalSize()) {
            auto identifier    = reader.readU32( settings.endian );
            auto tag_size      = reader.readU32( settings.endian );
            auto data_tag_size = tag_size - sizeof( uint32_t ) * 2;

            if( identifier == TAG_4DGI ) {
                file_is_not_valid = false;
                auto reader4DGI = reader.getReader( data_tag_size );

                // It always has a size of 0x3C for the full chunk size;
                if( tag_size != 0x3C )
                {
                    error_log.output << "4DGI should have size of 0x3c not 0x" << std::hex << tag_size << "\n";
                }
                else
                {
                    debug_log.output << "4DGI at location 0x" << std::hex << this->getOffset() << "\n";

                    auto un1 = reader4DGI.readU32( settings.endian );
                    if( un1 != 1 ) // Always 1 for Mac, Playstation, and Windows
                        warning_log.output << "4DGI un1 is 0x" << std::hex << un1 << "\n";

                    num_frames_4DGI = reader4DGI.readU16( settings.endian );

                    auto id       = reader4DGI.readU8(); // 0x10 for mac and 0x01 for windows
                    auto bitfield = reader4DGI.readU8();

                    // Reverse the bit order if the resource is mac.
                    if( settings.endian != Utilities::Buffer::Endian::LITTLE )
                        bitfield = reverse( bitfield );

                    info.has_skeleton     = (bitfield & 0x02) != 0;
                    info.always_on        = (bitfield & 0x08) != 0;
                    info.semi_transparent = (bitfield & 0x20) != 0;
                    info.environment_map  = (bitfield & 0x40) != 0;
                    info.animation        = (bitfield & 0x80) != 0;

                    // Check for bits that goes outside of what I found.
                    if( (bitfield & 0x15) != 0 ) {
                        warning_log.output << "4DGI has unknown bit values for info bitfield 0x" << std::hex << (unsigned)bitfield << ". Note: If this is the Mac version, then this bitfield is converted to Windows.\n";
                    }

                    // Skip the zeros.
                    auto position_index = reader4DGI.getPosition( Utilities::Buffer::BEGIN );
                    
                    // The next 12 bytes are all zeros.
                    for( int i = 0; i < 3; i++ ) {
                        auto zero_field = reader4DGI.readU32();

                        if( zero_field != 0 )
                            warning_log.output << "4DGI zero_field["<< std::dec << i << "]" << std::hex << id << "\n";
                    }
                    
                    reader4DGI.setPosition( position_index + 0xC, Utilities::Buffer::BEGIN );

                    auto un4 = reader4DGI.readU32( settings.endian );
                    if( un4 != 1 ) // Always 1 for Mac, Playstation, and Windows.
                        warning_log.output << "4DGI un4 is 0x" << std::hex << un4 << "\n";
                    auto un5 = reader4DGI.readU32( settings.endian );
                    if( un5 != 2 ) // Always 2 for Mac, Playstation, and Windows.
                        warning_log.output << "4DGI un5 is 0x" << std::hex << un5 << "\n";
                    auto un6 = reader4DGI.readU32( settings.endian );
                    if( un6 != 1 ) // Always 1 for Mac, Playstation, and Windows.
                        warning_log.output << "4DGI un6 is 0x" << std::hex << un6 << "\n";
                    auto un7 = reader4DGI.readU32( settings.endian );
                    if( un7 != 1 ) // Always 1 for Mac, Playstation, and Windows.
                        warning_log.output << "4DGI un7 is 0x" << std::hex << un7 << "\n";
                    auto un8 = reader4DGI.readU32( settings.endian ); // 0x30?
                    if( un8 != 3 ) // Always 3 for Mac, Playstation, and Windows.
                        warning_log.output << "4DGI un8 is 0x" << std::hex << un8 << "\n";

                    position_indexes[0] = reader4DGI.readU8();
                    position_indexes[1] = reader4DGI.readU8();
                    position_indexes[2] = reader4DGI.readU8();
                    position_indexes[3] = reader4DGI.readU8();

                    auto un13 = reader4DGI.readU32( settings.endian ); // 0x38
                    if( un13 != 4 ) // Always 4 for Mac, Playstation, and Windows.
                        warning_log.output << "4DGI un13 is 0x" << std::hex << un13 << "\n";
                    auto un14 = reader4DGI.readU32( settings.endian ); // 0x3C
                    if( un14 != 5 ) // Always 5 for Mac, Playstation, and Windows.
                        warning_log.output << "4DGI un14 is 0x" << std::hex << un14 << "\n";

                    // The file is then proven to be valid.
                    file_is_not_valid = false;
                }
            }
            else
            if( identifier == TAG_3DTL ) {
                auto reader3DTL = reader.getReader( data_tag_size );
                
                if( reader3DTL.readU32( settings.endian ) != 1 )
                {
                    warning_log.output << "3DTL unexpected number at beginning!\n";
                }

                bool valid_opcodes = true;

                while( valid_opcodes && reader3DTL.getPosition( Utilities::Buffer::BEGIN ) < reader3DTL.totalSize() ) {
                    uint16_t offset = reader3DTL.getPosition( Utilities::Buffer::BEGIN ) - sizeof(uint32_t);

                    face_types[offset] = FaceType();

                    face_types[offset].opcodes[0] = reader3DTL.readU8();
                    face_types[offset].opcodes[1] = reader3DTL.readU8();
                    face_types[offset].opcodes[2] = reader3DTL.readU8();
                    face_types[offset].opcodes[3] = reader3DTL.readU8();

                    face_types[offset].has_transparent_pixel_t0 = false;
                    face_types[offset].has_transparent_pixel_t1 = false;

                    face_types[offset].face_override_index = 0;

                    switch( face_types[offset].opcodes[0] ) {
                        case 1:
                        {
                            // Placeholder code
                            face_types[offset].bmp_id = 0;
                        }
                        break;
                        case 2:
                        case 3:
                        {
                            face_types[offset].coords[0].x = reader3DTL.readU8();
                            face_types[offset].coords[0].y = reader3DTL.readU8();

                            face_types[offset].coords[1].x = reader3DTL.readU8();
                            face_types[offset].coords[1].y = reader3DTL.readU8();

                            face_types[offset].coords[2].x = reader3DTL.readU8();
                            face_types[offset].coords[2].y = reader3DTL.readU8();

                            face_types[offset].coords[3].x = reader3DTL.readU8();
                            face_types[offset].coords[3].y = reader3DTL.readU8();

                            face_types[offset].bmp_id = reader3DTL.readU32( settings.endian );
                        }
                        break;
                        default:
                        {
                            error_log.output << std::hex << "0x" << (unsigned)face_types[offset].opcodes[0] << " opcode is not recognized for 3DTL.";
                            error_log.output << " Offset 0x" << this->getOffset() << ".\n";
                            valid_opcodes = false;
                        }
                    }
                }
            }
            else
            if( identifier == TAG_3DTA ) {
                auto reader3DTA = reader.getReader( data_tag_size );

                uint32_t number_of_face_overrides = reader3DTA.readU32( settings.endian );

                FaceOverrideType face_override_type;

                for( unsigned int i = 0; i < number_of_face_overrides; i++ ) {
                    face_override_type.number_of_frames = reader3DTA.readU8();
                    face_override_type.zero_0 = reader3DTA.readU8();
                    face_override_type.one = reader3DTA.readU8();
                    face_override_type.unknown_bitfield = reader3DTA.readU8();

                    face_override_type.frame_duration = reader3DTA.readU16( settings.endian );
                    face_override_type.zero_1 = reader3DTA.readU16( settings.endian );

                    face_override_type.uv_data_offset = reader3DTA.readU32( settings.endian );
                    face_override_type.offset_to_3DTL_uv = reader3DTA.readU32( settings.endian );

                    // Macintosh, Windows, and PS1 shows that these values never changed.
                    if(face_override_type.zero_0 != 0) {
                        warning_log.output << "3DTA index " << std::dec << i << " expected 0 BYTE, but got " << static_cast<uint32_t>(face_override_type.zero_0) << " instead.\n";
                    }
                    if(face_override_type.one != 1) {
                        warning_log.output << "3DTA index " << std::dec << i << " expected 1, but got " << static_cast<uint32_t>(face_override_type.one) << " instead.\n";
                    }
                    if((face_override_type.unknown_bitfield & 0xC6) != 0) {
                        warning_log.output << "3DTA index " << std::dec << i << " has an unusual bitfield 0x" << std::hex << static_cast<uint32_t>(face_override_type.unknown_bitfield) << ". This might cause inaccuracies in the frame by frame animation.\n";
                    }
                    if((face_override_type.unknown_bitfield & 0x01) != 1) {
                        warning_log.output << "3DTA index " << std::dec << i << " animation type not supported. An incorrect animation will be shown.\n";
                    }
                    if(face_override_type.zero_1 != 0) {
                        warning_log.output << "3DTA index " << std::dec << i << " expected 0 SHORT, but got " << static_cast<uint32_t>(face_override_type.zero_1) << " instead.\n";
                    }

                    face_type_overrides.push_back(face_override_type);
                }

                this->override_uvs.resize((reader3DTA.totalSize() - reader3DTA.getPosition()) / sizeof(glm::u8vec2));

                for(auto i = this->override_uvs.begin(); i != this->override_uvs.end(); i++) {
                    (*i).x = reader3DTA.readU8();
                    (*i).y = reader3DTA.readU8();
                }
            }
            else
            if( identifier == TAG_3DQL ) {
                auto reader3DQL = reader.getReader( data_tag_size );
                
                if( reader3DQL.readU32( settings.endian ) != 1 )
                {
                    warning_log.output << "3DQL unexpected number at beginning!\n";
                }
                
                auto number_of_faces = reader3DQL.readU32( settings.endian );
                
                if( number_of_faces == 0 ) // Each model has one or more faces.
                    warning_log.output << "3DQL the number of faces is missing.\n";

                debug_log.output << "3DQL has 0x" << std::hex << number_of_faces << " faces\n";

                for( unsigned int i = 0; i < number_of_faces; i++ )
                {
                    auto opcode_0 = reader3DQL.readU8();
                    auto opcode_1 = reader3DQL.readU8();

                    // Check if this file is not Windows or Playstation.
                    if( settings.endian != Utilities::Buffer::Endian::LITTLE ) {

                        // I do not know why Mac CObj stores this opcode like this, but this might have something to do with the PowerPC Instruction Set Architecture.
                        opcode_1 = ((opcode_1 & 0xf0) >> 4) |
                                   ((opcode_1 & 0x0e) << 3) |
                                   ((opcode_1 & 0x01) << 7);
                    }

                    const uint16_t face_type_offset = reader3DQL.readU16( settings.endian );

                    // Read the opcodes
                    const bool is_texture         = ((opcode_0 & 0x80) != 0);
                    const uint8_t un_array_amount =  (opcode_0 & 0x07);
                    const uint8_t bitfield        =  (opcode_0 & 0x78) >> 3;

                    bool            normal_shadows = false;
                    VisabilityMode  visability_mode = VisabilityMode::OPAQUE;
                    VertexColorMode vertex_color_mode = VertexColorMode::NON;

                    switch( bitfield ) {
                        case 0b0000:
                            normal_shadows    = false;
                            visability_mode   = VisabilityMode::OPAQUE;
                            vertex_color_mode = VertexColorMode::NON;
                            break;
                        case 0b0001:
                        case 0b0011:
                            normal_shadows    = false;
                            visability_mode   = VisabilityMode::MIX;
                            vertex_color_mode = VertexColorMode::NON;
                            break;
                        case 0b0010:
                            normal_shadows    = false;
                            visability_mode   = VisabilityMode::OPAQUE;
                            vertex_color_mode = VertexColorMode::MONOCHROME;
                            break;
                        case 0b0100:
                            normal_shadows    = true;
                            visability_mode   = VisabilityMode::OPAQUE;
                            vertex_color_mode = VertexColorMode::NON;
                            break;
                        case 0b0101:
                            normal_shadows    = true;
                            visability_mode   = VisabilityMode::MIX;
                            vertex_color_mode = VertexColorMode::NON;
                            break;
                        case 0b0110:
                        case 0b1010:
                        case 0b1011:
                            normal_shadows    = true;
                            visability_mode   = VisabilityMode::OPAQUE;
                            vertex_color_mode = VertexColorMode::FULL;
                            break;
                        case 0b0111:
                            normal_shadows    = true;
                            visability_mode   = VisabilityMode::MIX;
                            vertex_color_mode = VertexColorMode::FULL;
                            break;
                        case 0b1000:
                            normal_shadows    = true;
                            visability_mode   = VisabilityMode::OPAQUE;
                            vertex_color_mode = VertexColorMode::NON;
                            break;
                        case 0b1001:
                            normal_shadows    = true;
                            visability_mode   = VisabilityMode::MIX;
                            vertex_color_mode = VertexColorMode::NON;
                            break;
                        case 0b1101:
                            normal_shadows    = false;
                            visability_mode   = VisabilityMode::ADDITION;
                            vertex_color_mode = VertexColorMode::FULL;
                            break;
                        default: // Nothing
                            break;
                    }

                    // TODO Remove this workaround
                    if(visability_mode != VisabilityMode::ADDITION)
                        vertex_color_mode = VertexColorMode::NON;

                    const bool is_reflect   = ((opcode_1 & 0x80) != 0) & info.environment_map;
                    const uint16_t un_unk   =  (opcode_1 & 0x78) >> 3;
                    const uint8_t face_type =  (opcode_1 & 0x07);

                    if( is_reflect && !info.semi_transparent ) {
                        visability_mode = VisabilityMode::OPAQUE;
                    }

                    Primitive primitive;

                    primitive.face_type_offset = face_type_offset;
                    primitive.face_type_r      = nullptr;

                    primitive.visual.uses_texture       = is_texture;
                    primitive.visual.normal_shading     = normal_shadows;
                    primitive.visual.polygon_color_type = vertex_color_mode;
                    primitive.visual.visability         = visability_mode;
                    primitive.visual.is_reflective      = is_reflect;
                    primitive.visual.is_color_fade      = false;
                    
                    primitive.v[0] = reader3DQL.readU8();
                    primitive.v[1] = reader3DQL.readU8();
                    primitive.v[2] = reader3DQL.readU8();
                    primitive.v[3] = reader3DQL.readU8();

                    primitive.n[0] = reader3DQL.readU8();
                    primitive.n[1] = reader3DQL.readU8();
                    primitive.n[2] = reader3DQL.readU8();
                    primitive.n[3] = reader3DQL.readU8();

                    // If bitfield is 4 or 5 then the normals are flatened.
                    if(bitfield == 4 || bitfield == 5) {
                        primitive.n[1] = primitive.n[0];
                        primitive.n[2] = primitive.n[0];
                        primitive.n[3] = primitive.n[0];
                    }

                    switch( face_type ) {
                        case 7:
                        {
                            primitive.type = PrimitiveType::LINE;
                            face_lines.push_back( primitive );
                            break;
                        }
                        case 5:
                        {
                            primitive.type = PrimitiveType::BILLBOARD;
                            face_billboards.push_back( primitive );
                            break;
                        }
                        case 4:
                        {
                            primitive.type = PrimitiveType::QUAD;
                            face_quads.push_back( primitive );
                            break;
                        }
                        case 3:
                        {
                            primitive.type = PrimitiveType::TRIANGLE;
                            face_triangles.push_back( primitive );
                            break;
                        }
                        case 0:
                            // This code forces the display mode to be vertex color only.
                            // TODO Check if this code is even necessary
                            primitive.visual.uses_texture       = false;
                            primitive.visual.normal_shading     = false;
                            primitive.visual.polygon_color_type = VertexColorMode::FULL;
                            primitive.visual.visability         = VisabilityMode::ADDITION;
                            primitive.visual.is_reflective      = false;
                            primitive.visual.is_color_fade      = true;

                            primitive.type = PrimitiveType::STAR;
                            face_stars.push_back( primitive );
                            break;
                        default:
                        {
                            warning_log.output << std::dec << "Unknown Primative type = " << static_cast<unsigned>(face_type) << "\n";
                        }
                    }
                }
            }
            else
            if( identifier == TAG_3DHY ) {
                auto reader3DHY = reader.getReader( data_tag_size );
                
                const auto ONE_VALUE = reader3DHY.readU32( settings.endian );
                
                if( ONE_VALUE != 1 )
                {
                    warning_log.output << "3DHY unexpected number 0x" << std::hex << ONE_VALUE << " at beginning!\n";
                }

                const auto bones_space = reader3DHY.totalSize() - reader3DHY.getPosition();
                const auto amount_of_bones = bones_space / 0x14;

                debug_log.output << "3DHY: size = " << std::dec << amount_of_bones << "\n";

                if( (bones_space % 0x14) != 0 )
                    warning_log.output << "3DHY bones are not right!\n";
                
                this->bones.reserve( amount_of_bones );
                
                this->max_bone_childern = 0;

                for( size_t i = 0; i < amount_of_bones; i++ ) {
                    // This statement allocates a bone, but it reads the opcode of the bone first since I want the opcode to only be written once.
                    bones.push_back( Bone() );
                    
                    bones.at(i).parent_amount = reader3DHY.readU8();
                    bones.at(i).normal_start  = reader3DHY.readU8();
                    bones.at(i).normal_stride = reader3DHY.readU8();
                    bones.at(i).vertex_start  = reader3DHY.readU8();
                    bones.at(i).vertex_stride = reader3DHY.readU8();
                    auto un_byte0 = reader3DHY.readU8();
                    auto un_byte1 = reader3DHY.readU8();
                    // assert( Utilities::DataHandler::read_u8(start_data + 0x5) ); // can be zero
                    // assert( Utilities::DataHandler::read_u8(start_data + 0x6) ); // can be zero
                    auto opcode = reader3DHY.readU8();
                    
                    bones.at(i).opcode.unknown          = (opcode & 0b11000000) >> 6;
                    bones.at(i).opcode.position.x_const = (opcode & 0b00100000) >> 5;
                    bones.at(i).opcode.position.y_const = (opcode & 0b00010000) >> 4;
                    bones.at(i).opcode.position.z_const = (opcode & 0b00001000) >> 3;
                    bones.at(i).opcode.rotation.x_const = (opcode & 0b00000100) >> 2;
                    bones.at(i).opcode.rotation.y_const = (opcode & 0b00000010) >> 1;
                    bones.at(i).opcode.rotation.z_const = (opcode & 0b00000001) >> 0;
                    
                    bones.at(i).position.x = reader3DHY.readI16( settings.endian );
                    bones.at(i).position.y = reader3DHY.readI16( settings.endian );
                    bones.at(i).position.z = reader3DHY.readI16( settings.endian );
                    bones.at(i).rotation.x = reader3DHY.readI16( settings.endian );
                    bones.at(i).rotation.y = reader3DHY.readI16( settings.endian );
                    bones.at(i).rotation.z = reader3DHY.readI16( settings.endian );

                    bytes_per_frame_3DMI += getOpcodeBytesPerFrame( bones.at(i).opcode );
                    
                    this->max_bone_childern = std::max( bones.at(i).parent_amount, this->max_bone_childern );

                    // error_log.output << i << " = {" << bones.at(i).getString() << "}\n";
                }
                
                // The bytes_per_frame_3DMI might not actually hold true.
                // I found out that the position and the rotation contains the index to 3DMI.
                debug_log.output << "bytes_per_frame_3DMI = 0x" << std::hex << bytes_per_frame_3DMI << "\n";
            }
            else
            if( identifier == TAG_3DHS ) {
                auto reader3DHS = reader.getReader( data_tag_size );
                
                auto bone_depth_number = reader3DHS.readU32( settings.endian );
                auto data_size = reader3DHS.totalSize() - reader3DHS.getPosition();
                const auto BONE_SIZE = 4 * sizeof( uint16_t );

                auto read_3D_positions = data_size / BONE_SIZE; // vec3 with an empty space.
                
                frames_gen_3DHS = data_size / ( BONE_SIZE * bone_depth_number );

                debug_log.output << std::dec << "3DHS has " << read_3D_positions << " 3D vectors, and contains about " << frames_gen_3DHS << " frames.\n";
                
                for( int d = 0; d < frames_gen_3DHS; d++ )
                {
                    for( size_t i = 0; i < bone_depth_number; i++ )
                    {
                        auto u_x = reader3DHS.readU16( settings.endian );
                        auto u_y = reader3DHS.readU16( settings.endian );
                        auto u_z = reader3DHS.readU16( settings.endian );
                        // I determined that this value stays zero, so no reading needs to be done.
                        auto u_w = reader3DHS.readU16( settings.endian );
                    }
                }
            }
            else
            if( identifier == TAG_3DMI ) {
                auto reader3DMI = reader.getReader( data_tag_size );
                
                if( reader3DMI.readU32( settings.endian ) != 1 )
                    debug_log.output << "3DMI unexpected number at beginning!\n";
                
                this->bone_animation_data_size = (reader3DMI.totalSize() - reader3DMI.getPosition()) / sizeof( uint16_t );
                
                if( this->bone_animation_data != nullptr )
                    delete [] this->bone_animation_data;
                
                this->bone_animation_data = new int16_t [ this->bone_animation_data_size ];
                
                for( size_t d = 0; d < this->bone_animation_data_size; d++ )
                    this->bone_animation_data[ d ] = reader3DMI.readU16( settings.endian );
            }
            else
            if( identifier == TAG_3DAL ) {
                auto reader3DAL = reader.getReader( data_tag_size );

                if(tag_size != 0x14)
                    warning_log.output << "3DAL size is unusual.\n";

                if(tag_size < 0x14)
                    error_log.output << "3DAL chunk cannot be parsed. It is too small!\n";
                else {
                    auto count = reader3DAL.readU32( settings.endian );

                    if(count != 1)
                        warning_log.output << "3DAL count = " << std::dec << count << ".\n";

                    // TODO 3DAL is almost like 3DTA

                    // c_3DAL_data[0] = reader3DAL.readU8(); // 3DQL index to primative type circle or zero.
                    // c_3DAL_data[1] = reader3DAL.readU8(); // Speed value?
                    // c_3DAL_data[2] = reader3DAL.readU8(); // Red[0]
                    // c_3DAL_data[3] = reader3DAL.readU8(); // Green[0]
                    // c_3DAL_data[4] = reader3DAL.readU8(); // Blue[0]
                    // c_3DAL_data[5] = reader3DAL.readU8(); // Red[1]
                    // c_3DAL_data[6] = reader3DAL.readU8(); // Green[1]
                    // c_3DAL_data[7] = reader3DAL.readU8(); // Blue[1]
                }
            }
            else
            if( identifier == TAG_3DRF ) {
                auto reader3DRF = reader.getReader( data_tag_size );

                auto reference_number = reader3DRF.readU32( settings.endian );
                auto reference_tag    = reader3DRF.readU32( settings.endian );
                auto reference_count  = reader3DRF.readU32( settings.endian );

                VertexData::Tag tag;
                uint32_t expected_reference_number;
                bool tag_not_recognized = false;

                switch(reference_tag) {
                    case TAG_3DRL:
                        tag = VertexData::C_3DRL;
                        expected_reference_number = 3;
                        break;
                    case TAG_4DNL:
                        tag = VertexData::C_4DNL;
                        expected_reference_number = 2;
                        break;
                    case TAG_4DVL:
                        tag = VertexData::C_4DVL;
                        expected_reference_number = 1;
                        break;
                    default:
                        tag_not_recognized = true;
                }

                if(expected_reference_number != reference_number) {
                    warning_log.output << "3DRF reference number on tag 0x" << std::hex << reference_tag << " has an unexpected reference number " << std::dec << reference_number << "! Expected " << expected_reference_number << ". However, this parser will ignore this value.\n";
                }

                if(reference_count == 0) {
                    error_log.output << "3DRF reference tag 0x" << std::hex << reference_tag << " has a zero reference count!\n";
                    file_is_not_valid = true;
                }

                if(vertex_data.get3DRFSize() == 0) {
                    vertex_data.set3DRFSize(reference_count);
                }

                bool has_unnormalized_number = false;
                bool id_beyond_ff_ff = false; // For some reason Future Cop does not like ids beyond 0xFFFF despite the storage medium having 32 bits.

                if(tag_not_recognized) {
                    error_log.output << "3DRF has a reference tag 0x" << std::hex << reference_tag << " that is not recognized. Ignoring the rest of this chunk.\n";
                    file_is_not_valid = true;
                }
                else {
                    for(uint32_t i = 0; i < reference_count; i++) {
                        const auto current_id_number = reader3DRF.readU32( settings.endian );

                        vertex_data.set3DRFItem(tag, i, current_id_number);

                        if(current_id_number != i + 1)
                            has_unnormalized_number = true;

                        if(current_id_number > 0xFFFF)
                            id_beyond_ff_ff = true;
                    }
                }

                if(has_unnormalized_number) {
                    warning_log.output << "3DRF reference number on tag 0x" << std::hex << reference_tag << " contains chunk id(s) that are not normalized. Future Cop will most likely accept them.\n";
                }

                if(id_beyond_ff_ff) {
                    warning_log.output << "3DRF reference number on tag 0x" << std::hex << reference_tag << " contains chunk id(s) that exceed(s) 0xFFFF. The original Future Cop engine will most likely CRASH with id(s) that high.\n";
                }
            }
            else
            if( identifier == TAG_4DVL ) {
                auto reader4DVL = reader.getReader( data_tag_size );
                
                auto frame_id = reader4DVL.readU32( settings.endian );

                auto amount_of_vertices = reader4DVL.readU32( settings.endian );

                if(vertex_data.get4DVLSize() < 0)
                    vertex_data.set4DVLSize(amount_of_vertices);
                else if(static_cast<uint32_t>(vertex_data.get4DVLSize()) != amount_of_vertices) {
                    error_log.output << "4DVL has a sizing problem expected " << vertex_data.get4DVLSize() << " but got " << amount_of_vertices  << ". It will be capped.\n";

                    amount_of_vertices = std::min(static_cast<uint32_t>(vertex_data.get4DVLSize()), amount_of_vertices);
                    file_is_not_valid = true;
                }

                glm::i16vec3 *positions_r = vertex_data.get4DVLPointer(frame_id);

                if(positions_r == nullptr) {
                    error_log.output << "4DVL: Cannot find identifier " << frame_id << " for positions.\n";
                    file_is_not_valid = true;
                }
                else {
                    for( unsigned int i = 0; i < amount_of_vertices; i++ ) {
                        positions_r[i].x = reader4DVL.readI16( settings.endian );
                        positions_r[i].y = reader4DVL.readI16( settings.endian );
                        positions_r[i].z = reader4DVL.readI16( settings.endian );
                        reader4DVL.readI16( settings.endian );
                    }
                }
            }
            else
            if( identifier == TAG_4DNL ) {
                auto reader4DNL = reader.getReader( data_tag_size );
                
                auto frame_id = reader4DNL.readU32( settings.endian );

                auto amount_of_normals = reader4DNL.readU32( settings.endian );

                if(vertex_data.get4DNLSize() < 0)
                    vertex_data.set4DNLSize(amount_of_normals);
                else if(static_cast<uint32_t>(vertex_data.get4DNLSize()) != amount_of_normals) {
                    error_log.output << "4DNL has a sizing problem expected " << vertex_data.get4DNLSize() << " but got " << amount_of_normals << ". It will be capped.\n";

                    amount_of_normals = std::min(static_cast<uint32_t>(vertex_data.get4DNLSize()), amount_of_normals);
                    file_is_not_valid = true;
                }

                glm::i16vec3 *normals_r = vertex_data.get4DNLPointer(frame_id);

                if(vertex_data.get4DNLSize() != 0 && normals_r == nullptr) {
                    error_log.output << "4DNL: Cannot find identifier " << frame_id << " for normals.\n";
                    file_is_not_valid = true;
                }
                else if(normals_r != nullptr) {
                    for( unsigned int i = 0; i < amount_of_normals; i++ ) {
                        normals_r[i].x = reader4DNL.readI16( settings.endian );
                        normals_r[i].y = reader4DNL.readI16( settings.endian );
                        normals_r[i].z = reader4DNL.readI16( settings.endian );
                        reader4DNL.readI16( settings.endian );
                    }
                }
            }
            else
            if( identifier == TAG_3DRL ) {
                debug_log.output << "3DRL" << std::endl;
                auto reader3DRL = reader.getReader(data_tag_size);

                auto frame_id = reader3DRL.readU32(settings.endian);

                auto count = reader3DRL.readU32(settings.endian);

                if(vertex_data.get3DRLSize() < 0)
                    vertex_data.set3DRLSize(count);
                else if(static_cast<uint32_t>(vertex_data.get3DRLSize()) != count) {
                    error_log.output << "3DRL has a sizing problem expected " << vertex_data.get3DRLSize() << " but got " << count  << ". It will be capped.\n";

                    count = std::min(static_cast<uint32_t>(vertex_data.get3DRLSize()), count);
                    file_is_not_valid = true;
                }

                uint16_t *length_data_r = vertex_data.get3DRLPointer(frame_id);

                if(vertex_data.get3DRLSize() != 0 && length_data_r == nullptr) {
                    error_log.output << "3DRL: Cannot find identifier " << frame_id << " for normals.\n";
                    file_is_not_valid = true;
                }
                else if(length_data_r != nullptr) {
                    for( uint32_t i = 0; i < count; i++ ) {
                        length_data_r[i] = reader3DRL.readU16(settings.endian);
                    }
                }
            }
            else
            if( identifier == TAG_AnmD ) {
                auto readerAnmD = reader.getReader( data_tag_size );

                if( readerAnmD.readU32( settings.endian ) != 1 )
                    warning_log.output << "AnmD unexpected number at beginning!\n";

                auto data_size = readerAnmD.totalSize() - readerAnmD.getPosition();
                const auto TRACK_AMOUNT = data_size / 0x10;

                debug_log.output << std::dec << "AnmD has " << TRACK_AMOUNT << " tracks\n";
                
                for( unsigned int i = 0; i < TRACK_AMOUNT; i++ ) {
                    auto u0 = readerAnmD.readU8();
                    auto un_speed = readerAnmD.readU8(); // The bigger the slower
                    auto u2 = readerAnmD.readU8();
                    auto un_skip_frame = readerAnmD.readU8(); // Wild guess.
                    auto un_from_frame = readerAnmD.readU16( settings.endian );
                    auto un_to_frame   = readerAnmD.readU16( settings.endian );
                    auto u6 = readerAnmD.readU8();
                    auto u7 = readerAnmD.readU8();
                    auto u8 = readerAnmD.readU16( settings.endian );
                    auto u9 = readerAnmD.readU32( settings.endian );

                    // from_frame and to_frame can be (from_frame == to_frame), (from_frame > to_frame), and (from_frame < to_frame)
                }
            }
            else
            if( identifier == TAG_3DBB ) {
                auto reader3DBB = reader.getReader( data_tag_size );
                
                bounding_box_per_frame = reader3DBB.readU32( settings.endian );
                
                if( bounding_box_per_frame >= 1 )
                {
                    // Numbers 1, 2, 3, 4, 5, 6, 7 appears throughout the English version of Future Cop on the ps1, Mac, and Windows.
                    if( bounding_box_per_frame > 7 && bounding_box_per_frame == 0 )
                        warning_log.output << "bounding_box_per_frame = " << std::dec << bounding_box_per_frame << "\n";
                    
                    const auto bounding_box_amount = reader3DBB.readU32( settings.endian );
                    
                    bounding_boxes.reserve( bounding_box_amount );
                    
                    bounding_box_frames = bounding_box_amount / bounding_box_per_frame;
                    
                    size_t BOUNDING_BOX_SIZE = 8 * sizeof( uint16_t );
                    
                    size_t bounding_boxes_amount = bounding_box_frames * bounding_box_per_frame;
                    
                    size_t test_tag_size = bounding_boxes_amount * BOUNDING_BOX_SIZE + 4 * sizeof( uint32_t );
                    
                    if( test_tag_size != tag_size )
                    {
                        warning_log.output << "bounding box per frame is " << std::dec << bounding_box_per_frame << "\n";
                        warning_log.output << "3DBB frames is " << bounding_box_frames << "\n";
                        warning_log.output << "The tag size should be " << test_tag_size << " instead it is " << tag_size << "\n";
                    }
                    
                    for( size_t frame_index = 0; frame_index < bounding_box_frames; frame_index++ )
                    {
                        for( size_t box_index = 0; box_index < bounding_box_per_frame; box_index++ )
                        {
                            bounding_boxes.push_back( BoundingBox3D() );

                            // NOTE: To convert these variables to floating point multiply them be the FIXED_POINT_UNIT.
                            
                            // Fact Positive and negative: position x
                            bounding_boxes.back().x = reader3DBB.readI16( settings.endian );
                            
                            // Fact Positive and negative: position y
                            bounding_boxes.back().y = reader3DBB.readI16( settings.endian );
                            
                            // Fact Positive and negative: position z
                            bounding_boxes.back().z = reader3DBB.readI16( settings.endian );
                            
                            // Fact [0, 4224]: length x
                            bounding_boxes.back().length_x = reader3DBB.readU16( settings.endian );
                            
                            // Fact [0, 1438]: length y
                            bounding_boxes.back().length_y = reader3DBB.readU16( settings.endian );
                            
                            // Fact [0, 3584]: length z
                            bounding_boxes.back().length_z = reader3DBB.readU16( settings.endian );
                            
                            // Fact [0, 4293]: This uses the pythagorean theorem on the three lengths. Roughly (length_x^2 + length_y^2 + length_z^2) square rooted.
                            bounding_boxes.back().length_pyth_3 = reader3DBB.readU16( settings.endian );
                            
                            // Fact [0, 4293]: This uses the pythagorean theorem on length_x and length_z. Roughly (length_x^2 + length_z^2) square rooted.
                            bounding_boxes.back().length_pyth_2 = reader3DBB.readU16( settings.endian );

                            //uint32_t x_c = bounding_boxes.back().length_x * bounding_boxes.back().length_x;
                            //uint32_t y_c = bounding_boxes.back().length_y * bounding_boxes.back().length_y;
                            //uint32_t z_c = bounding_boxes.back().length_z * bounding_boxes.back().length_z;

                            //assert( std::abs((FIXED_POINT_UNIT * std::sqrt(x_c + y_c + z_c)) - (FIXED_POINT_UNIT * static_cast<double>(bounding_boxes.back().length_pyth_3))) < 0.125 );
                            //assert( std::abs((FIXED_POINT_UNIT * std::sqrt(x_c    +    z_c)) - (FIXED_POINT_UNIT * static_cast<double>(bounding_boxes.back().length_pyth_2))) < 0.125 );
                        }
                    }
                }
                else
                {
                    error_log.output << "3DBB unexpected number at beginning! " << std::dec << bounding_box_per_frame << "\n";
                }
            }
            else
            {
                reader.setPosition( data_tag_size, Utilities::Buffer::CURRENT );

                error_log.output << std::hex << "0x" << identifier << " not recognized\n";
            }

            if( file_is_not_valid ) {
                error_log.output << "This is not a valid Obj file!\n";
                reader.setPosition( 0, Utilities::Buffer::END );
            }
        }

        for( auto i = face_type_overrides.size(); i != 0; i-- ) {
            FaceOverrideType &face_override = face_type_overrides[ i - 1 ];

            size_t data_size = 4 * face_override.number_of_frames;

            if(face_override.uv_data_offset % sizeof(glm::u8vec2) != 0) {
                error_log.output << "UV data offset 0x" << std::hex << face_override.uv_data_offset << " is not even\n";
                error_log.output << "  Culling 3DTL index " << std::dec << (i - 1) << "\n";

                face_type_overrides.erase(face_type_overrides.begin() + (i - 1));
                continue;
            }

            if(override_uvs.size() < data_size + face_override.uv_data_offset / sizeof(glm::u8vec2)) {
                error_log.output << "UV data offset 0x" << std::hex << face_override.uv_data_offset << " is too big for size 0x" << override_uvs.size() << "\n";
                error_log.output << "  Culling 3DTL index " << std::dec << (i - 1) << "\n";

                face_type_overrides.erase(face_type_overrides.begin() + (i - 1));
                continue;
            }

            if(face_types.find(face_override.offset_to_3DTL_uv - 4) == face_types.end()) {
                error_log.output << "Offset to 3DTL 0x" << std::hex << face_override.offset_to_3DTL_uv << " is not found.\n";
                error_log.output << "  Anyways culling 3DTL index " << std::dec << (i - 1) << ". It might be a feature that I do not know about yet.\n";

                face_type_overrides.erase(face_type_overrides.begin() + (i - 1));
                continue;
            }

            face_types[face_override.offset_to_3DTL_uv - 4].face_override_index = i;
        }
        
        // This warning tells that there are only two options for animations either morphing or bone animation.
        if( !( !(( bytes_per_frame_3DMI > 0 ) & ( vertex_data.get3DRFSize() > 1 )) ) )
            warning_log.output << "Both animation systems are used. This might be a problem because normal Future Cop models do not have both bone and morph animations.\n";
        
        if( bytes_per_frame_3DMI > 0 )
        {
            this->bone_frames = this->bone_animation_data_size / (bytes_per_frame_3DMI / sizeof(int16_t));
            
            // This proves that the obj resource bone frame are roughly equvilent to the number of
            // bounding box frames.
            if( bounding_box_frames != bone_frames && bounding_box_frames + 1 != bone_frames )
            {
                error_log.output << "bounding box per frame is " << std::dec << bounding_box_per_frame << "\n";
                error_log.output << "3DBB frames is " << bounding_box_frames << "\n";
                error_log.output << "3DBB frames not equal to " << bone_frames << "\n";
            }
            
            if( num_frames_4DGI != bone_frames )
            {
                error_log.output << "4DGI frames is " << std::dec << num_frames_4DGI << "\n";
                error_log.output << "4DGI frames not equal to " << bone_frames << "\n";
            }

            if(!override_uvs.empty()) {
                debug_log.output << "3DTA is with bone animation data!\n";
            }
        }
        else
        if( vertex_data.get3DRFSize() > 1 )
        {
            // This proves that each model with morph animation has an equal number of
            // vertex frames as the bounding box frames.
            if( bounding_box_frames != vertex_data.get3DRFSize() )
            {
                error_log.output << "bounding box per frame is " << std::dec << bounding_box_per_frame << "\n";
                error_log.output << "3DBB frames is " << bounding_box_frames << "\n";
                error_log.output << "3DBB frames not equal to " << (vertex_data.get3DRFSize() - 1) << "\n";
            }
            
            if( num_frames_4DGI != vertex_data.get3DRFSize() )
            {
                error_log.output << "4DGI frames is " << std::dec << num_frames_4DGI << "\n";
                error_log.output << "4DGI frames not equal to " << vertex_data.get3DRFSize() << "\n";
            }

            if(!override_uvs.empty()) {
                debug_log.output << "3DTA is with morph data!\n";
            }
        }
        else
        {
            // This statement proves that each model without animation only has one bounding_box frame.
            if( bounding_box_frames != 1 )
                error_log.output << "4DGI bounding_box_frames = " << std::dec << bounding_box_frames << "\n";
        }

        for( auto &primitive : this->face_triangles ) {
            if( this->face_types.find( primitive.face_type_offset ) != this->face_types.end() ) {
                primitive.face_type_r = &this->face_types[ primitive.face_type_offset ];
            }
        }
        for( auto &primitive : this->face_quads ) {
            if( this->face_types.find( primitive.face_type_offset ) != this->face_types.end() ) {
                primitive.face_type_r = &this->face_types[ primitive.face_type_offset ];
            }
        }
        for( auto &primitive : this->face_billboards ) {
            if( this->face_types.find( primitive.face_type_offset ) != this->face_types.end() ) {
                primitive.face_type_r = &this->face_types[ primitive.face_type_offset ];
            }
        }
        for( auto &primitive : this->face_lines ) {
            if( this->face_types.find( primitive.face_type_offset ) != this->face_types.end() ) {
                primitive.face_type_r = &this->face_types[ primitive.face_type_offset ];
            }
        }

        return !file_is_not_valid;
    }
    else
        return false;
}

Data::Mission::Resource * Data::Mission::ObjResource::duplicate() const {
    return new ObjResource( *this );
}

bool Data::Mission::ObjResource::isPositionValid( unsigned index ) const {
    if( index < 4 && vertex_data.get4DVLSize() >= 0 && static_cast<uint32_t>(vertex_data.get4DVLSize()) > position_indexes[index] )
        return true;
    else
        return false;
}

glm::vec3 Data::Mission::ObjResource::getPosition( unsigned index ) const {
    glm::vec3 position(0, 0, 0);

    if( isPositionValid( index ) ) {
        const uint32_t id_length = vertex_data.get3DRFItem(VertexData::C_4DVL, 0);
        const glm::i16vec3* const vertex_positions_r = vertex_data.get4DVLPointer(id_length);

        position  = vertex_positions_r[ position_indexes[index] ];
        position *= glm::vec3(FIXED_POINT_UNIT, FIXED_POINT_UNIT, FIXED_POINT_UNIT);
    }

    return position;
}

int Data::Mission::ObjResource::write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    int glTF_return = 0;

    AllowedPrimitives allowed_primitives;

    allowed_primitives.star      = true;
    allowed_primitives.triangle  = true;
    allowed_primitives.quad      = true;
    allowed_primitives.billboard = true;
    allowed_primitives.line      = true;

    Utilities::ModelBuilder *model_output = createMesh(!iff_options.obj.export_metadata, false, allowed_primitives);

    if( iff_options.obj.shouldWrite( iff_options.enable_global_dry_default ) ) {
        // Make sure that the model has some vertex data.
        if( !iff_options.obj.no_model ) {
            if( model_output->getNumVertices() >= 3 ) {

                if( !bones.empty() )
                    model_output->applyJointTransforms( 0 );

                glTF_return = model_output->write( std::string( file_path ), "cobj_" + std::to_string( getResourceID() ) );
            }
            else {
                // Make it easier on the user to identify empty Obj's
                std::ofstream resource;

                resource.open( std::string(file_path) + "_empty.txt", std::ios::out );

                if( resource.is_open() ) {
                    resource << "Obj with index number of " << getIndexNumber() << " or with id number " << getResourceID() << " is empty." << std::endl;
                    resource.close();
                }
            }
        }

        if(iff_options.obj.export_bounding_box) {
            Utilities::ModelBuilder *bounding_boxes_p = createBoundingBoxes();

            if(bounding_boxes_p != nullptr) {
                bounding_boxes_p->write( std::string( file_path + "_bb"), "cobj_" + std::to_string( getResourceID() )+ "_bb"  );

                delete bounding_boxes_p;
            }
            else {
                std::ofstream resource;

                resource.open( std::string(file_path) + "_empty_bb.txt", std::ios::out );

                if( resource.is_open() ) {
                    resource << "Obj with index number of " << getIndexNumber() << " or with id number " << getResourceID() << " has failed!" << std::endl;
                    resource.close();
                }
            }
        }
    }

    delete model_output;

    return glTF_return;
}

std::vector<Data::Mission::ObjResource::FacerPolygon> Data::Mission::ObjResource::generateFacingPolygons(uint32_t index) const {
    std::vector<Data::Mission::ObjResource::FacerPolygon> polys;
    FacerPolygon facer_polygon;

    const auto v_frame_id = vertex_data.get3DRFItem(VertexData::C_4DVL, index);
    const auto n_frame_id = vertex_data.get3DRFItem(VertexData::C_4DNL, index);
    const auto r_frame_id = vertex_data.get3DRFItem(VertexData::C_3DRL, index);

    const glm::i16vec3 *positions_r = vertex_data.get4DVLPointer(v_frame_id);
    const glm::i16vec3 *normals_r   = vertex_data.get4DNLPointer(n_frame_id);
    const uint16_t     *lengths_r   = vertex_data.get3DRLPointer(r_frame_id);

    // Add the stars.
    for( auto i = face_stars.begin(); i != face_stars.end(); i++ ) {
        facer_polygon.type = FacerPolygon::STAR;
        facer_polygon.visability_mode = VisabilityMode::ADDITION;
        facer_polygon.color.r = (*i).v[1] * (1. / 256.);
        facer_polygon.color.g = (*i).v[2] * (1. / 256.);
        facer_polygon.color.b = (*i).v[3] * (1. / 256.);
        facer_polygon.width = lengths_r[(*i).n[0]] * FIXED_POINT_UNIT;
        facer_polygon.primitive.star.position = glm::vec3(positions_r[(*i).v[0]]) * FIXED_POINT_UNIT;

        polys.push_back( facer_polygon );
    }

    return polys;
}

bool Data::Mission::ObjResource::loadTextures( const std::vector<BMPResource*> &textures ) {
    if( textures.size() != 0 ) {
        bool valid = true;

        std::map< uint32_t, BMPResource* > resource_id_to_bmp;
        std::map< uint32_t, TextureReference > resource_id_to_reference;

        glm::vec2 points[6];

        for( uint32_t i = 0; i < textures.size(); i++ )
            resource_id_to_bmp[ textures[ i ]->getResourceID() ] = textures[ i ];

        // Make a null texture.
        resource_id_to_reference[0].resource_id = 0;
        resource_id_to_reference[0].name = "";

        for( auto i = face_types.begin(); i != face_types.end(); i++ ) {
            // Check for the texture bit on the opcode.
            if( ((*i).second.opcodes[0] & 0x2) == 0 )
                continue;

            const auto RESOURCE_ID = (*i).second.bmp_id + 1;
            
            if( resource_id_to_reference.count( RESOURCE_ID ) == 0 ) {
                resource_id_to_reference[ RESOURCE_ID ].resource_id = RESOURCE_ID;

                if( resource_id_to_bmp.count( RESOURCE_ID ) != 0 ) {
                    if( resource_id_to_bmp[ RESOURCE_ID ]->getImageFormat() != nullptr ) {
                        auto bmp_reference_r = resource_id_to_bmp[ RESOURCE_ID ];

                        resource_id_to_reference[ RESOURCE_ID ].name = bmp_reference_r->getImageFormat()->appendExtension( bmp_reference_r->getFullName( RESOURCE_ID ) );
                        
                        assert( !resource_id_to_reference[ RESOURCE_ID ].name.empty() );
                    }
                }
                else
                    valid = false;
            }
        }

        for( auto i = face_types.begin(); i != face_types.end(); i++ ) {
            // Check for the texture bit on the opcode.
            if( ((*i).second.opcodes[0] & 0x2) == 0 )
                continue;

            const auto RESOURCE_ID = (*i).second.bmp_id + 1;

            if( resource_id_to_bmp.count( RESOURCE_ID ) != 0 ) {
                auto bmp_reference_r = resource_id_to_bmp[ RESOURCE_ID ];

                points[0] = (*i).second.coords[QUAD_TABLE[0][0]];
                points[1] = (*i).second.coords[QUAD_TABLE[0][1]];
                points[2] = (*i).second.coords[QUAD_TABLE[0][2]];

                points[3] = (*i).second.coords[QUAD_TABLE[1][0]];
                points[4] = (*i).second.coords[QUAD_TABLE[1][1]];
                points[5] = (*i).second.coords[QUAD_TABLE[1][2]];

                if( Data::Mission::BMPResource::isSemiTransparent( *bmp_reference_r->getImage(), &points[0] ) )
                    (*i).second.has_transparent_pixel_t0 = true;
                if( Data::Mission::BMPResource::isSemiTransparent( *bmp_reference_r->getImage(), &points[3] ) )
                    (*i).second.has_transparent_pixel_t1 = true;
            }
        }

        texture_references.reserve( resource_id_to_reference.size() );

        for( auto it = resource_id_to_reference.begin(); it != resource_id_to_reference.end(); it++ ) {
            texture_references.push_back( (*it).second );
        }

        return valid;
    }
    else {
        return false;
    }
}

Utilities::ModelBuilder * Data::Mission::ObjResource::createModel() const {
    AllowedPrimitives allowed_primitives;

    allowed_primitives.star      = false;
    allowed_primitives.triangle  = true;
    allowed_primitives.quad      = true;
    allowed_primitives.billboard = true;
    allowed_primitives.line      = true;

    return createMesh(
        false, // False as metadata would NOT be excluded.
        true,  // True normals will be created.
        allowed_primitives );
}

Utilities::ModelBuilder * Data::Mission::ObjResource::createMesh( bool exclude_metadata, bool force_normal, AllowedPrimitives allowed_primitives ) const {
    Utilities::ModelBuilder *model_output = new Utilities::ModelBuilder();

    // This buffer will be used to store every triangle that the write function has.
    std::vector<Triangle> triangle_buffer;
    std::vector<MorphTriangle> morph_triangle_buffer;
    std::map<unsigned int, unsigned int> triangle_counts;

    {
        std::vector<Primitive> primitive_buffer;
        size_t triangle_buffer_size = 0;

        triangle_buffer_size += face_triangles.size() * Primitive::getTriangleAmount( PrimitiveType::TRIANGLE );
        triangle_buffer_size += face_quads.size() * Primitive::getTriangleAmount( PrimitiveType::QUAD );

        triangle_buffer.reserve( triangle_buffer_size );
        morph_triangle_buffer.reserve( triangle_buffer_size * (vertex_data.get3DRFSize() - 1) );
        primitive_buffer.reserve( triangle_buffer_size );

        if(allowed_primitives.star) {
            // Add the stars.
            for( auto i = face_stars.begin(); i != face_stars.end(); i++ )
                primitive_buffer.push_back( (*i) );
        }

        if(allowed_primitives.triangle) {
            // Add the triangles.
            for( auto i = face_triangles.begin(); i != face_triangles.end(); i++ )
                primitive_buffer.push_back( (*i) );
        }

        if(allowed_primitives.quad) {
            // Add the quaderlaterals.
            for( auto i = face_quads.begin(); i != face_quads.end(); i++ )
                primitive_buffer.push_back( (*i) );
        }

        if(allowed_primitives.billboard) {
            // Add the billboards
            for( auto i = face_billboards.begin(); i != face_billboards.end(); i++ )
                primitive_buffer.push_back( (*i) );
        }

        if(allowed_primitives.line) {
            // Add the lines.
            for( auto i = face_lines.begin(); i != face_lines.end(); i++ )
                primitive_buffer.push_back( (*i) );
        }

        // Sort the triangle list.
        std::sort(primitive_buffer.begin(), primitive_buffer.end(), Primitive() );

        // Get the list of the used textures
        for( auto i = primitive_buffer.begin(); i != primitive_buffer.end(); i++ ) {
            uint32_t bmp_id = (*i).getBmpID();

            if( triangle_counts.find(bmp_id) == triangle_counts.end() ) {
                triangle_counts[bmp_id] = 0;
            }
            triangle_counts[bmp_id] += Primitive::getTriangleAmount( (*i).type );
        }

        for( auto i = primitive_buffer.begin(); i != primitive_buffer.end(); i++ ) {
            if( (*i).type == PrimitiveType::STAR )
                (*i).setCircle(vertex_data, triangle_buffer, morph_triangle_buffer, bones);
            else if( (*i).type == PrimitiveType::TRIANGLE )
                (*i).setTriangle(vertex_data, triangle_buffer, morph_triangle_buffer, bones);
            else if( (*i).type == PrimitiveType::QUAD )
                (*i).setQuad(vertex_data, triangle_buffer, morph_triangle_buffer, bones);
            else if( (*i).type == PrimitiveType::BILLBOARD )
                (*i).setBillboard(vertex_data, triangle_buffer, morph_triangle_buffer, bones);
            else if( (*i).type == PrimitiveType::LINE )
                (*i).setLine(vertex_data, triangle_buffer, morph_triangle_buffer, bones);
        }
    }

    bool build_normals = (vertex_data.get4DNLSize() != 0) | force_normal;

    unsigned int position_component_index = model_output->addVertexComponent( Utilities::ModelBuilder::POSITION_COMPONENT_NAME, Utilities::DataTypes::ComponentType::FLOAT, Utilities::DataTypes::Type::VEC3 );
    unsigned int normal_component_index = -1;

    if(build_normals)
        normal_component_index = model_output->addVertexComponent( Utilities::ModelBuilder::NORMAL_COMPONENT_NAME, Utilities::DataTypes::ComponentType::FLOAT, Utilities::DataTypes::Type::VEC3 );

    unsigned int color_component_index = model_output->addVertexComponent( Utilities::ModelBuilder::COLORS_0_COMPONENT_NAME, Utilities::DataTypes::ComponentType::UNSIGNED_BYTE, Utilities::DataTypes::Type::VEC4, true );
    unsigned int tex_coord_component_index = model_output->addVertexComponent( Utilities::ModelBuilder::TEX_COORD_0_COMPONENT_NAME, Utilities::DataTypes::ComponentType::UNSIGNED_BYTE, Utilities::DataTypes::Type::VEC2, true );
    unsigned int metadata_component_index = -1;
    unsigned int joints_0_component_index = -1;
    unsigned int weights_0_component_index = -1;

    if(!exclude_metadata)
        metadata_component_index = model_output->addVertexComponent( METADATA_COMPONENT_NAME, Utilities::DataTypes::ComponentType::SHORT, Utilities::DataTypes::Type::VEC2, false );

    if( !bones.empty() ) {
        joints_0_component_index  = model_output->addVertexComponent( Utilities::ModelBuilder::JOINTS_INDEX_0_COMPONENT_NAME, Utilities::DataTypes::ComponentType::UNSIGNED_BYTE, Utilities::DataTypes::Type::VEC4, false );
        weights_0_component_index = model_output->addVertexComponent( Utilities::ModelBuilder::WEIGHTS_INDEX_0_COMPONENT_NAME, Utilities::DataTypes::ComponentType::UNSIGNED_BYTE, Utilities::DataTypes::Type::VEC4, true );

        model_output->allocateJoints( bones.size(), bone_frames );
        
        glm::mat4 bone_matrix;
        
        // Make joint relations.
        unsigned int childern[ max_bone_childern ];
        for( unsigned int bone_index = 0; bone_index < bones.size(); bone_index++ ) {
            childern[ bones.at( bone_index ).parent_amount - 1 ] = bone_index;
            
            if( bones.at( bone_index ).parent_amount > 1 )
                model_output->setJointParent( childern[ bones.at( bone_index ).parent_amount - 2 ], bone_index );
        }

        for( unsigned int bone_index = 0; bone_index < bones.size(); bone_index++ ) {
            auto current_bone = bones.begin() + bone_index;
            
            childern[ (*current_bone).parent_amount - 1 ] = bone_index;
            
            for( unsigned int frame = 0; frame < bone_frames; frame++ )
            {
                auto frame_position = (*current_bone).position;
                auto frame_rotation = (*current_bone).rotation;
                
                if( !(*current_bone).opcode.position.x_const )
                    frame_position.x = bone_animation_data[ (*current_bone).position.x + frame ];
                if( !(*current_bone).opcode.position.y_const )
                    frame_position.y = bone_animation_data[ (*current_bone).position.y + frame ];
                if( !(*current_bone).opcode.position.z_const )
                    frame_position.z = bone_animation_data[ (*current_bone).position.z + frame ];
                if( !(*current_bone).opcode.rotation.x_const )
                    frame_rotation.x = bone_animation_data[ (*current_bone).rotation.x + frame ];
                if( !(*current_bone).opcode.rotation.y_const )
                    frame_rotation.y = bone_animation_data[ (*current_bone).rotation.y + frame ];
                if( !(*current_bone).opcode.rotation.z_const )
                    frame_rotation.z = bone_animation_data[ (*current_bone).rotation.z + frame ];
                
                bone_matrix = glm::rotate( glm::mat4(1.0f), -static_cast<float>( frame_rotation.x ) * ANGLE_UNIT, glm::vec3( 0, 1, 0 ) );
                bone_matrix = glm::rotate( bone_matrix, static_cast<float>( frame_rotation.y ) * ANGLE_UNIT, glm::vec3( 1, 0, 0 ) );
                bone_matrix = glm::rotate( glm::mat4(1.0f), -static_cast<float>( frame_rotation.z ) * ANGLE_UNIT, glm::vec3( 0, 0, 1 ) ) * bone_matrix;
                
                auto position  = glm::vec3( -frame_position.x, frame_position.y, frame_position.z ) * static_cast<float>( FIXED_POINT_UNIT );
                auto quaterion = glm::quat_cast( bone_matrix );
                
                model_output->setJointFrame( frame, bone_index, position, quaterion );
            }
        }
    }
    unsigned int position_morph_component_index = -1;
    unsigned int normal_morph_component_index = -1;

    if( vertex_data.get3DRFSize() > 1 ) {
        position_morph_component_index = model_output->setVertexComponentMorph( position_component_index );

        if(build_normals)
            normal_morph_component_index = model_output->setVertexComponentMorph( normal_component_index );
    }

    // Setup the vertex components now that every field had been entered.
    model_output->setupVertexComponents( vertex_data.get3DRFSize() - 1 );

    model_output->allocateVertices( triangle_buffer.size() * 3 );

    if( texture_references.size() == 0 )
        model_output->setMaterial( "" );

    glm::i16vec2 metadata;

    auto triangle = triangle_buffer.begin();
    auto previous_triangle = triangle_buffer.begin();
    auto morph_triangle = morph_triangle_buffer.begin();

    for( auto count_it = triangle_counts.begin(); count_it != triangle_counts.end(); count_it++ )
    {
        // If there are no triangle counts then do not even write down the material.
        if( (*count_it).second == 0 )
            continue;

        bool found = false;

        // Do a linear search to find the resource id stored in first of count_it.
        for( unsigned t_index = 0; t_index < texture_references.size(); t_index++ ) {
            if( texture_references.at( t_index ).resource_id == (*count_it).first ) {

                // Set this material.
                model_output->setMaterial( texture_references.at( t_index ).name, texture_references.at( t_index ).resource_id, true );

                // The material is found.
                found = true;

                // Stop this search.
                t_index = texture_references.size();
            }
        }

        assert(found);

        for( unsigned int i = 0; i < (*count_it).second; i++ )
        {
            assert( triangle != triangle_buffer.end() );

            if( (*triangle).visual.normal_shading )
                metadata[0] = 2;
            else
                metadata[0] = 1;

            if( !(*triangle).visual.is_reflective )
                metadata[0] = -metadata[0];

            if( triangle != previous_triangle ) {
                if( (*triangle).bmp_id == (*previous_triangle).bmp_id )
                {
                    if( (*previous_triangle).visual.visability < (*triangle).visual.visability )
                        model_output->beginSemiTransperency( (*triangle).visual.visability == VisabilityMode::ADDITION );
                    else if( (*previous_triangle).visual.visability > (*triangle).visual.visability )
                        assert( false && "Sorting is wrong!" );
                }
                else if( (*triangle).visual.visability != VisabilityMode::OPAQUE ) {
                    model_output->beginSemiTransperency( (*triangle).visual.visability == VisabilityMode::ADDITION );
                }
            }
            else if( (*triangle).visual.visability != VisabilityMode::OPAQUE ) {
                model_output->beginSemiTransperency( (*triangle).visual.visability == VisabilityMode::ADDITION );
            }

            for( unsigned vertex_index = 0; vertex_index < 3; vertex_index++ ) {
                const Point point = (*triangle).points[vertex_index];

                metadata[1] = point.face_override_index;

                assert(point.face_override_index <= 4 * face_type_overrides.size());

                model_output->startVertex();

                model_output->setVertexData( position_component_index, Utilities::DataTypes::Vec3Type( point.position ) );

                if(build_normals)
                    model_output->setVertexData( normal_component_index, Utilities::DataTypes::Vec3Type( point.normal ) );

                if(vertex_index != 1 && (*triangle).visual.is_color_fade)
                    model_output->setVertexData( color_component_index, Utilities::DataTypes::Vec4UByteType( glm::u8vec4(0, 0, 0, 0) ) );
                else
                    model_output->setVertexData( color_component_index, Utilities::DataTypes::Vec4UByteType( (*triangle).color ) );

                model_output->setVertexData( tex_coord_component_index, Utilities::DataTypes::Vec2UByteType(  point.coords ) );

                if(!exclude_metadata)
                    model_output->setVertexData( metadata_component_index, Utilities::DataTypes::Vec2SShortType( metadata ) );

                auto morph_triangle_frame = morph_triangle;

                for( unsigned morph_frames = 0; morph_frames < vertex_data.get3DRFSize() - 1; morph_frames++ )
                {
                    const MorphPoint morph_point = (*morph_triangle_frame).points[vertex_index];

                    model_output->addMorphVertexData( position_morph_component_index, morph_frames, Utilities::DataTypes::Vec3Type( point.position ), Utilities::DataTypes::Vec3Type( morph_point.position ) );

                    if(build_normals)
                        model_output->addMorphVertexData( normal_morph_component_index, morph_frames, Utilities::DataTypes::Vec3Type( point.normal ),   Utilities::DataTypes::Vec3Type( morph_point.normal ) );

                    morph_triangle_frame++;
                }
                if( !bones.empty() ) {
                    model_output->setVertexData( joints_0_component_index, Utilities::DataTypes::Vec4UByteType( point.joints ) );
                    model_output->setVertexData( weights_0_component_index, Utilities::DataTypes::Vec4UByteType( point.weights ) );
                }
            }

            triangle++;
            previous_triangle = triangle - 1;

            if( morph_triangle != morph_triangle_buffer.end() )
                morph_triangle += vertex_data.get3DRFSize() - 1;
        }
    }
    
    model_output->finish();
    
    return model_output;
}

Utilities::ModelBuilder * Data::Mission::ObjResource::createBoundingBoxes() const {
    const unsigned int BOX_EDGES = 12;
    const unsigned int EDGE_AMOUNT = 2;

    if(bounding_box_per_frame > 0 && bounding_box_frames > 0) {
        Utilities::ModelBuilder *box_output = new Utilities::ModelBuilder( Utilities::ModelBuilder::LINES );

        unsigned int position_component_index = box_output->addVertexComponent( Utilities::ModelBuilder::POSITION_COMPONENT_NAME, Utilities::DataTypes::ComponentType::FLOAT, Utilities::DataTypes::Type::VEC3 );
        unsigned int color_coord_component_index = box_output->addVertexComponent( Utilities::ModelBuilder::COLORS_0_COMPONENT_NAME, Utilities::DataTypes::ComponentType::UNSIGNED_BYTE, Utilities::DataTypes::Type::VEC4, true );
        unsigned int position_morph_component_index = 0;

        if( bounding_box_frames > 1 )
            position_morph_component_index = box_output->setVertexComponentMorph( position_component_index );

        glm::vec3 position;
        glm::vec3 morph_position;
        glm::u8vec4 color(0, 255, 0, 255);

        // At this point it is time to start generating bounding box.

        box_output->setupVertexComponents(bounding_box_frames - 1);

        box_output->allocateVertices(bounding_box_per_frame * BOX_EDGES * EDGE_AMOUNT);

        // No texture should be used for this bounding box.
        box_output->setMaterial( "" );

        for( unsigned int box_index = 0; box_index < this->bounding_box_per_frame; box_index++ )
        {
            const BoundingBox3D &current_box = this->bounding_boxes[ box_index * bounding_box_frames ];
            const glm::vec3 bb_center = FIXED_POINT_UNIT * glm::vec3(current_box.x, current_box.y, current_box.z);
            const glm::vec3 bb_scale  = FIXED_POINT_UNIT * glm::vec3(current_box.length_x + 1, current_box.length_y + 1, current_box.length_z + 1);

            auto buildPoint = [&](bool x, bool y, bool z) {
                position = bb_scale;

                if(x) {
                    position.x = -position.x;
                    color.r = 255;
                }
                else
                    color.r = 0;

                if(y) {
                    position.y = -position.y;
                    color.g = 255;
                }
                else
                    color.g = 0;

                if(z) {
                    position.z = -position.z;
                    color.b = 255;
                }
                else
                    color.b = 0;

                position += bb_center;

                position.x = -position.x;

                box_output->startVertex();
                box_output->setVertexData( position_component_index, Utilities::DataTypes::Vec3Type( position ) );
                box_output->setVertexData( color_coord_component_index, Utilities::DataTypes::Vec4UByteType( color ) );

                for(unsigned int f = 1; f < bounding_box_frames; f++) {
                    const BoundingBox3D &morph_box = this->bounding_boxes[ box_index * bounding_box_frames + f ];
                    const glm::vec3 morph_center = FIXED_POINT_UNIT * glm::vec3(morph_box.x, morph_box.y, morph_box.z);
                    const glm::vec3 morph_scale  = FIXED_POINT_UNIT * glm::vec3(morph_box.length_x, morph_box.length_y, morph_box.length_z);

                    morph_position = morph_scale;

                    if(x)
                        morph_position.x = -morph_position.x;

                    if(y)
                        morph_position.y = -morph_position.y;

                    if(z)
                        morph_position.z = -morph_position.z;

                    morph_position += morph_center;

                    morph_position.x = -morph_position.x;

                    box_output->addMorphVertexData( position_morph_component_index, f - 1, Utilities::DataTypes::Vec3Type( position ), Utilities::DataTypes::Vec3Type( morph_position ) );
                }
            };
            bool is_upper = false;

            for(int i = 0; i < 2; i++) {
                buildPoint( true, is_upper,  true);
                buildPoint(false, is_upper,  true);

                buildPoint(false, is_upper,  true);
                buildPoint(false, is_upper, false);

                buildPoint(false, is_upper, false);
                buildPoint( true, is_upper, false);

                buildPoint( true, is_upper, false);
                buildPoint( true, is_upper,  true);

                is_upper = true;
            }

            buildPoint( true,  true,  true);
            buildPoint( true, false,  true);

            buildPoint(false,  true,  true);
            buildPoint(false, false,  true);

            buildPoint(false,  true, false);
            buildPoint(false, false, false);

            buildPoint( true,  true, false);
            buildPoint( true, false, false);
        }

        box_output->finish();

        return box_output;
    }
    else
        return nullptr;
}

unsigned int Data::Mission::ObjResource::getOpcodeBytesPerFrame( Data::Mission::ObjResource::Bone::Opcode opcode ) {
    unsigned int number_of_16bit_numbers = 0;

    // I hope the compiler optimizes this.
    if( !opcode.position.x_const )
        number_of_16bit_numbers++;
    if( !opcode.position.y_const )
        number_of_16bit_numbers++;
    if( !opcode.position.z_const )
        number_of_16bit_numbers++;
    
    if( !opcode.rotation.x_const )
        number_of_16bit_numbers++;
    if( !opcode.rotation.y_const )
        number_of_16bit_numbers++;
    if( !opcode.rotation.z_const )
        number_of_16bit_numbers++;
    
    return 2 * number_of_16bit_numbers;
}

bool Data::Mission::IFFOptions::ObjOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {
    if( !singleArgument( arguments, "--" + getNameSpace() + "_NO_MODEL", output_r, no_model ) )
        return false; // The single argument is not valid.
    if( !singleArgument( arguments, "--" + getNameSpace() + "_METADATA", output_r, export_metadata ) )
        return false; // The single argument is not valid.
    if( !singleArgument( arguments, "--" + getNameSpace() + "_BOUNDING_BOXES", output_r, export_bounding_box ) )
        return false; // The single argument is not valid.

    return IFFOptions::ResourceOption::readParams( arguments, output_r );
}

std::string Data::Mission::IFFOptions::ObjOption::getOptions() const {
    std::string information_text = getBuiltInOptions(8);

    information_text += "  --" + getNameSpace() + "_NO_MODEL       Disable model exporting for the map.\n";
    information_text += "  --" + getNameSpace() + "_METADATA       Export the primary glTF file without its metadata.\n";
    information_text += "  --" + getNameSpace() + "_BOUNDING_BOXES Export a glTF file per resource containing bounding boxes.\n";

    return information_text;
}
