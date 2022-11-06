#include "ObjResource.h"

#include "IFF.h"

#include "../../Utilities/DataHandler.h"
#include <glm/geometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include <cassert>

namespace {
    // The header
    const uint32_t TAG_4DGI = 0x34444749; // which is { 0x34, 0x44, 0x47, 0x49 } or { '4', 'D', 'G', 'I' } or "4DGI"
    // Texture reference list
    const uint32_t TAG_3DTL = 0x3344544C; // which is { 0x33, 0x44, 0x54, 0x4C } or { '3', 'D', 'T', 'L' } or "3DTL"
    // Face Quad list holding offsets of vertices and normals.
    const uint32_t TAG_3DQL = 0x3344514C; // which is { 0x33, 0x44, 0x51, 0x4C } or { '3', 'D', 'Q', 'L' } or "3DQL"
    // 3D reference?
    const uint32_t TAG_3DRF = 0x33445246; // which is { 0x33, 0x44, 0x52, 0x46 } or { '3', 'D', 'R', 'F' } or "3DRF"
    // 3D reference list?
    const uint32_t TAG_3DRL = 0x3344524C; // which is { 0x33, 0x44, 0x52, 0x4C } or { '3', 'D', 'R', 'L' } or "3DRL"
    // Bones.
    const uint32_t TAG_3DHY = 0x33444859; // which is { 0x33, 0x44, 0x48, 0x59 } or { '3', 'D', 'H', 'Y' } or "3DHY"
    // Positions of other objects
    const uint32_t TAG_3DHS = 0x33444853; // which is { 0x33, 0x44, 0x48, 0x53 } or { '3', 'D', 'H', 'S' } or "3DHS"
    // Bone Animation Attributes.
    const uint32_t TAG_3DMI = 0x33444D49; // which is { 0x33, 0x44, 0x4d, 0x49 } or { '3', 'D', 'M', 'I' } or "3DMI"
    // 3D triangle array?
    const uint32_t TAG_3DTA = 0x33445441; // which is { 0x33, 0x44, 0x54, 0x41 } or { '3', 'D', 'T', 'A' } or "3DTA"
    // 3D array list?
    const uint32_t TAG_3DAL = 0x3344414C; // which is { 0x33, 0x44, 0x41, 0x4C } or { '3', 'D', 'A', 'L' } or "3DAL"
    // 4D vertex list (Note: Ignore the 4D data).
    const uint32_t TAG_4DVL = 0x3444564C; // which is { 0x34, 0x44, 0x56, 0x4C } or { '4', 'D', 'V', 'L' } or "4DVL"
    // 4D normal list
    const uint32_t TAG_4DNL = 0x34444E4C; // which is { 0x34, 0x44, 0x4E, 0x4C } or { '4', 'D', 'N', 'L' } or "4DNL"
    // Animation track data
    const uint32_t TAG_AnmD = 0x416e6d44; // which is { 0x41, 0x6e, 0x6d, 0x44 } or { 'A', 'n', 'm', 'D' } or "AnmD"
    // 3D bounding box
    const uint32_t TAG_3DBB = 0x33444242; // which is { 0x33, 0x44, 0x42, 0x42 } or { '3', 'D', 'B', 'B' } or "3DBB"

    const auto INTEGER_FACTOR = 1.0 / 256.0;

    void triangleToCoords( const Data::Mission::ObjResource::FaceTriangle &triangle, glm::u8vec2 *cords )
    {
        if( !triangle.is_other_side )
        {
            cords[0] = triangle.texture_quad_ref->coords[0];
            cords[1] = triangle.texture_quad_ref->coords[1];
            cords[2] = triangle.texture_quad_ref->coords[2];
        }
        else
        {
            cords[0] = triangle.texture_quad_ref->coords[2];
            cords[1] = triangle.texture_quad_ref->coords[3];
            cords[2] = triangle.texture_quad_ref->coords[0];
        }
    }

    void handlePositions( glm::vec3 &position, const glm::i16vec3 *array, int index ) {
        position.x = -array[ index ].x * INTEGER_FACTOR;
        position.y =  array[ index ].y * INTEGER_FACTOR;
        position.z =  array[ index ].z * INTEGER_FACTOR;
    }
    void handleNormals( glm::vec3 &normal, const glm::i16vec3 *array, int index ) {
        normal.x = array[ index ].x;
        normal.y = array[ index ].y;
        normal.z = array[ index ].z;

        normal = glm::normalize( normal );
    }
uint8_t reverse(uint8_t b) {
   b = (b & 0b11110000) >> 4 | (b & 0b00001111) << 4;
   b = (b & 0b11001100) >> 2 | (b & 0b00110011) << 2;
   b = (b & 0b10101010) >> 1 | (b & 0b01010101) << 1;
   return b;
}
}

bool Data::Mission::ObjResource::TextureQuad::isWithinBounds( size_t texture_amount ) const {
    if( index >= 0 )
    {
        return (static_cast<size_t>(index) > texture_amount);
    }
    else
    if( index < 0 )
    {
        return (static_cast<size_t>(-index - 1) > texture_amount);
    }
    else
        return false; // This statement should not be reached.
}

bool Data::Mission::ObjResource::FaceTriangle::isWithinBounds( size_t vertex_limit, size_t normal_limit, size_t texture_quad_limit, const TextureQuad *origin ) const {
    bool is_valid = true;

    if( static_cast<unsigned int>(texture_quad_ref - origin) >= texture_quad_limit )
        is_valid = false;
    else
    if( this->v0 >= vertex_limit )
        is_valid = false;
    else
    if( this->v1 >= vertex_limit )
        is_valid = false;
    else
    if( this->v2 >= vertex_limit )
        is_valid = false;
    else
    if( normal_limit != 0 && this->n0 >= normal_limit )
        is_valid = false;
    else
    if( normal_limit != 0 && this->n1 >= normal_limit )
        is_valid = false;
    else
    if( normal_limit != 0 && this->n2 >= normal_limit )
        is_valid = false;

    return is_valid;
}

bool Data::Mission::ObjResource::FaceTriangle::operator() ( const FaceTriangle & l_operand, const FaceTriangle & r_operand ) const {
    if( l_operand.texture_quad_ref != nullptr && r_operand.texture_quad_ref != nullptr )
        return (l_operand.texture_quad_ref->index < r_operand.texture_quad_ref->index);
    else
    if( l_operand.texture_quad_ref != nullptr )
        return false;
    else
        return true;
}

Data::Mission::ObjResource::FaceTriangle Data::Mission::ObjResource::FaceQuad::firstTriangle() const {
    FaceTriangle newTri;

    newTri.is_other_side = false;
    newTri.is_reflective = is_reflective;
    newTri.texture_quad_ref = texture_quad_ref;
    newTri.v0 = v0;
    newTri.v1 = v1;
    newTri.v2 = v2;
    newTri.n0 = n0;
    newTri.n1 = n1;
    newTri.n2 = n2;

    return newTri;
}

Data::Mission::ObjResource::FaceTriangle Data::Mission::ObjResource::FaceQuad::secondTriangle() const {
    FaceTriangle newTri;

    newTri.is_other_side = true;
    newTri.is_reflective = is_reflective;
    newTri.texture_quad_ref = texture_quad_ref;
    newTri.v0 = v2;
    newTri.v1 = v3;
    newTri.v2 = v0;
    newTri.n0 = n2;
    newTri.n1 = n3;
    newTri.n2 = n0;

    return newTri;

}

unsigned int Data::Mission::ObjResource::Bone::getNumAttributes() const {
    return (getOpcodeBytesPerFrame( this->opcode ) / 2);
}
        
const std::string Data::Mission::ObjResource::FILE_EXTENSION = "cobj";
const uint32_t Data::Mission::ObjResource::IDENTIFIER_TAG = 0x436F626A; // which is { 0x43, 0x6F, 0x62, 0x6A } or { 'C', 'o', 'b', 'j' } or "Cobj"

Data::Mission::ObjResource::ObjResource() {
    this->bone_frames = 0;
    this->max_bone_childern = 0;
    this->bone_animation_data = nullptr;
}

Data::Mission::ObjResource::ObjResource( const ObjResource &obj ) : ModelResource( obj ) {
    this->bounding_box_frames = 0;
    this->bone_frames = 0;
    this->max_bone_childern = 0;
    this->bone_animation_data = nullptr;
    if( obj.bone_animation_data != nullptr )
    {
        this->bone_animation_data_size = obj.bone_animation_data_size;
        this->bone_animation_data = new int16_t [ obj.bone_animation_data_size ];
    }
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
    if( this->data_p != nullptr )
    {
        auto reader = this->data_p->getReader();

        bool file_is_not_valid = true;
        
        // This is for testing mostly.
        uint8_t *data_3DMI       = nullptr;
        int data_3DMI_size       =  0;
        int bytes_per_frame_3DMI =  0;
        int frames_gen_3DHS      = -1;
        int frames_gen_AnmD      = -1;
        uint16_t num_frames_4DGI =  0;

        while( reader.getPosition( Utilities::Buffer::BEGIN ) < reader.totalSize() ) {
            auto identifier    = reader.readU32( settings.endian );
            auto tag_size      = reader.readU32( settings.endian );
            auto data_tag_size = tag_size - sizeof( uint32_t ) * 2;

            *settings.output_ref << std::hex;

            if( identifier == TAG_4DGI ) {
                file_is_not_valid = false;
                auto reader4DGI = reader.getReader( data_tag_size );

                // It always has a size of 0x3C for the full chunk size;
                if( tag_size != 0x3C && settings.output_level >= 1 )
                {
                    *settings.output_ref << "Mission::ObjResource::load() 4DGI should have size of 0x3c not 0x"
                              << tag_size << std::endl;
                    assert( false );
                }
                else
                {
                    if( settings.output_level >= 2 )
                        *settings.output_ref << "Mission::ObjResource::load() index #"
                            << std::dec << getIndexNumber() << std::hex
                            << " at location 0x" << this->getOffset() << std::endl;

                    auto un1 = reader4DGI.readU32( settings.endian );
                    assert( un1 == 1 ); // Always 1 for Mac, Playstation, and Windows
                    num_frames_4DGI = reader4DGI.readU16( settings.endian );
                    auto id       = reader4DGI.readU8();
                    auto bitfield = reader4DGI.readU8();
                    
                    // The first byte is decoded.
                    if( id == 0x10 || id == 0x01 ) {
                        // Reverse the bit order if in Playstation or Windows.
                        if( id == 0x01 )
                            bitfield = reverse( bitfield );
                        
                        // These are all the values that are seen in the CObj format.
                        if( !((bitfield == 0x10) || (bitfield == 0x11) ||
                              (bitfield == 0x12) || (bitfield == 0x13) ||
                              (bitfield == 0x14) || (bitfield == 0x15) ||
                              (bitfield == 0x16) || (bitfield == 0x51) ||
                              (bitfield == 0x53) || (bitfield == 0x55)) )
                        {
                            if( settings.output_ref != nullptr )
                                *settings.output_ref << "Mission::ObjResource::load() bitfield actually is 0x" << std::hex << bitfield << std::dec << std::endl;
                            
                            assert( false );
                        }
                    }
                    else
                        assert( false );

                    // Skip the zeros.
                    auto position_index = reader4DGI.getPosition( Utilities::Buffer::BEGIN );
                    
                    // The next 12 bytes are all zeros.
                    assert( reader4DGI.readU32() == 0 ); // 0x0 - 0x3
                    assert( reader4DGI.readU32() == 0 ); // 0x4 - 0x7
                    assert( reader4DGI.readU32() == 0 ); // 0x8 - 0xA
                    
                    reader4DGI.setPosition( position_index + 0xC, Utilities::Buffer::BEGIN );

                    auto un4  = reader4DGI.readU32( settings.endian );
                    assert( un4 == 1 ); // Always 1 for Mac, Playstation, and Windows.
                    auto un5  = reader4DGI.readU32( settings.endian );
                    assert( un5 == 2 ); // Always 2 for Mac, Playstation, and Windows.
                    auto un6  = reader4DGI.readU32( settings.endian );
                    assert( un6 == 1 ); // Always 1 for Mac, Playstation, and Windows.
                    auto un7  = reader4DGI.readU32( settings.endian );
                    assert( un7 == 1 ); // Always 1 for Mac, Playstation, and Windows.
                    auto un8  = reader4DGI.readU32( settings.endian ); // 0x30?
                    assert( un8 == 3 ); // Always 3 for Mac, Playstation, and Windows.
                    auto un9  = reader4DGI.readU8(); // Unknown 0xFF and 0x6F values found.
                    auto un10 = reader4DGI.readU8(); // Unknown 0xFF and 0x12 values found.
                    auto un11 = reader4DGI.readU8(); // Unknown 0xFF and 0x00 values found.
                    auto un12 = reader4DGI.readU8(); // Unknown 0xFF and 0x00 values found.
                    auto un13 = reader4DGI.readU32( settings.endian ); // 0x38
                    assert( un13 == 4 ); // Always 4 for Mac, Playstation, and Windows.
                    auto un14 = reader4DGI.readU32( settings.endian ); // 0x3C
                    assert( un14 == 5 ); // Always 5 for Mac, Playstation, and Windows.

                    // The file is then proven to be valid.
                    file_is_not_valid = false;
                }
            }
            else
            if( identifier == TAG_3DTL ) {
                auto reader3DTL = reader.getReader( data_tag_size );
                
                if( reader3DTL.readU32( settings.endian ) != 1 && settings.output_level >= 1 )
                {
                    *settings.output_ref << "Mission::ObjResource::load() 3DTL unexpected number at beginning!" << std::endl;
                }

                auto texture_ref_amount = (reader3DTL.totalSize() - reader3DTL.getPosition()) / 0x10;
                if( settings.output_level >= 2 )
                    *settings.output_ref << "triangle amount " << std::dec << texture_ref_amount << std::hex << std::endl;

                for( auto i = 0; i < texture_ref_amount; i++ )
                {
                    /*if( DataHandler::read_u8( start_data ) != 2 )
                        std::cout << "Mission::ObjResource::load() expected 2 at uv not " << std::dec
                                  << (static_cast<uint32_t>(DataHandler::read_u8( start_data )) & 0xFF)
                                  << std::hex << std::endl;*/
                    
                    reader3DTL.readU32( settings.endian ); // Skip unknown bytes

                    texture_quads.push_back( TextureQuad() );

                    texture_quads.back().coords[0].x = reader3DTL.readU8();
                    texture_quads.back().coords[0].y = reader3DTL.readU8();

                    texture_quads.back().coords[1].x = reader3DTL.readU8();
                    texture_quads.back().coords[1].y = reader3DTL.readU8();

                    texture_quads.back().coords[2].x = reader3DTL.readU8();
                    texture_quads.back().coords[2].y = reader3DTL.readU8();

                    texture_quads.back().coords[3].x = reader3DTL.readU8();
                    texture_quads.back().coords[3].y = reader3DTL.readU8();
                    
                    // For some reason the Slim's Windows English version has a 64th Cobj that goes beyond 10 textures.
                    
                    texture_quads.back().index = reader3DTL.readU32( settings.endian );
                }
            }
            else
            if( identifier == TAG_3DQL ) {
                auto reader3DQL = reader.getReader( data_tag_size );
                
                if( reader3DQL.readU32( settings.endian ) != 1 && settings.output_level >= 1 )
                {
                    *settings.output_ref << "Mission::ObjResource::load() 3DQL unexpected number at beginning!" << std::endl;
                }
                
                auto number_of_faces = reader3DQL.readU32( settings.endian );
                
                assert( number_of_faces != 0 ); // Each model has one or more faces.

                if( settings.output_level >= 2 )
                    *settings.output_ref << "Mission::ObjResource::load() 3DQL has 0x" << number_of_faces << " faces" << std::endl;

                for( unsigned int i = 0; i < number_of_faces; i++ )
                {
                    auto face_type   = reader3DQL.readU8();
                    assert(( (face_type & 0x07) == 3 ) || ((face_type & 0x07) == 4 ));
                    auto face_type_2 = reader3DQL.readU8();

                    auto texture_quad_ref = (reader3DQL.readU16( settings.endian ) / 0x10) + texture_quads.data();

                    /*
                    // Add to the indexes of this texture to the list when it is not contained in the data base.
                    if( !std::binary_search( texture_dependences.begin(), texture_dependences.end(), FaceTriangle() ) && texture_quad_ref - texture_quads.data() > texture_quads.size() ) {
                        texture_dependences.push_back( texture_quad_ref->index );
                        std::sort( texture_dependences.begin(), texture_dependences.end() );
                    }*/
                    bool reflect = ((face_type_2 & 0xF0) == 0x80);
                    
                    // Data::Mission::Resource::ParseSettings::Macintosh
                    
                    if( (face_type & 0x07) == 4 ) {
                        face_quads.push_back( FaceQuad() );

                        face_quads.back().texture_quad_ref = texture_quad_ref;
                        // (((face_type & 0xFF) == 0xCC) & ((face_type_2 & 0x84) == 0x84))
                        // 0x07 Appears to be its own face type
                        face_quads.back().is_reflective = reflect;

                        face_quads.back().v0 = reader3DQL.readU8();
                        face_quads.back().v1 = reader3DQL.readU8();
                        face_quads.back().v2 = reader3DQL.readU8();
                        face_quads.back().v3 = reader3DQL.readU8();
                        
                        face_quads.back().n0 = reader3DQL.readU8();
                        face_quads.back().n1 = reader3DQL.readU8();
                        face_quads.back().n2 = reader3DQL.readU8();
                        face_quads.back().n3 = reader3DQL.readU8();
                    }
                    else {
                        face_trinagles.push_back( FaceTriangle() );

                        face_trinagles.back().is_other_side = false;
                        face_trinagles.back().texture_quad_ref = texture_quad_ref;
                        // (face_type & 0x08) seems to be the effect bit.
                        // (face_type & 0x28) seems to be the tranlucent bit.
                        face_trinagles.back().is_reflective = reflect;

                        face_trinagles.back().v0 = reader3DQL.readU8();
                        face_trinagles.back().v1 = reader3DQL.readU8();
                        face_trinagles.back().v2 = reader3DQL.readU8();
                        reader3DQL.readU8();

                        face_trinagles.back().n0 = reader3DQL.readU8();
                        face_trinagles.back().n1 = reader3DQL.readU8();
                        face_trinagles.back().n2 = reader3DQL.readU8();
                        reader3DQL.readU8();
                    }
                }
            }
            else
            if( identifier == TAG_3DRF ) {
                // std::cout << "Mission::ObjResource::load() 3DRF" << std::endl;
                auto reader3DRF = reader.getReader( data_tag_size );
                
                // std::cout << "Index " << getIndexNumber() << std::endl;
                // std::cout << "reader3DRF.totalSize() = " << reader3DRF.totalSize() << std::endl;
                // assert( reader3DRF.totalSize() == 0x10 );
            }
            else
            if( identifier == TAG_3DRL ) {
                // std::cout << "Mission::ObjResource::load() 3DRL" << std::endl;
                auto reader3DRL = reader.getReader( data_tag_size );
            }
            else
            if( identifier == TAG_3DHY ) {
                auto reader3DHY = reader.getReader( data_tag_size );
                
                const auto ONE_VALUE = reader3DHY.readU32( settings.endian );
                
                assert( ONE_VALUE == 1 );

                if( ONE_VALUE != 1 && settings.output_level >= 1)
                {
                    *settings.output_ref << "Mission::ObjResource::load() 3DHY unexpected number at beginning!" << std::endl;
                }

                const auto bones_space = reader3DHY.totalSize() - reader3DHY.getPosition();
                const auto amount_of_bones = bones_space / 0x14;

                if( settings.output_level >= 2 )
                    *settings.output_ref << "Mission::ObjResource::load() 3DHY: size = " << amount_of_bones << std::endl;

                if( (bones_space % 0x14) != 0 && settings.output_level >= 1 )
                    *settings.output_ref << "Mission::ObjResource::load() 3DHY bones are not right!" << std::endl;
                
                this->bones.reserve( amount_of_bones );
                
                this->max_bone_childern = 0;

                for( int i = 0; i < amount_of_bones; i++ ) {
                    // This statement allocates a bone, but it reads the opcode of the bone first since I want the opcode to only be written once.
                    bones.push_back( Bone() );
                    
                    bones.at(i).parent_amount = reader3DHY.readU8();
                    bones.at(i).normal_start  = reader3DHY.readU8();
                    bones.at(i).normal_stride = reader3DHY.readU8();
                    bones.at(i).vertex_start  = reader3DHY.readU8();
                    bones.at(i).vertex_stride = reader3DHY.readU8();
                    auto byte0 = reader3DHY.readU8();
                    auto byte1 = reader3DHY.readU8();
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
                    
                    if( settings.output_level >= 3 )
                    {
                        *settings.output_ref << "bone: ";

                        *settings.output_ref 
                            << "parent index: 0x" << bones.at(i).parent_amount << ", "
                            << "0x" <<    bones.at(i).normal_start << " with 0x" <<  bones.at(i).normal_stride << " normals, "
                            << "0x" <<    bones.at(i).vertex_start << " with 0x" <<  bones.at(i).vertex_stride << " vertices, "
                            << "opcode: 0x" << opcode << std::dec
                            << ", position( " << bones.at(i).position.x << ", " << bones.at(i).position.y << ", " << bones.at(i).position.z << " )"
                            << ", rotation( " << bones.at(i).rotation.x << ", " << bones.at(i).rotation.y << ", " << bones.at(i).rotation.z << " )" << std::hex << std::endl;
                    }
                }
                
                if( settings.output_level >= 3 )
                {
                    // The bytes_per_frame_3DMI might not actually hold true.
                    // I found out that the position and the rotation contains the index to 3DMI.
                    *settings.output_ref << "bytes_per_frame_3DMI = 0x" << bytes_per_frame_3DMI << std::endl;
                }
            }
            else
            if( identifier == TAG_3DHS ) {
                auto reader3DHS = reader.getReader( data_tag_size );
                
                auto bone_depth_number = reader3DHS.readU32( settings.endian );
                auto data_size = reader3DHS.totalSize() - reader3DHS.getPosition();
                const auto BONE_SIZE = 4 * sizeof( uint16_t );

                auto read_3D_positions = data_size / BONE_SIZE; // vec3 with an empty space.
                
                frames_gen_3DHS = data_size / ( BONE_SIZE * bone_depth_number );

                if( settings.output_level >= 2 )
                {
                    *settings.output_ref
                        << std::dec
                        << "Mission::ObjResource::load() 3DHS has " << read_3D_positions << " 3D vectors, and contains about " << frames_gen_3DHS << " frames." << std::endl
                        << std::hex;
                }
                
                for( int d = 0; d < frames_gen_3DHS; d++ )
                {
                    for( int i = 0; i < bone_depth_number; i++ )
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
                
                if( reader3DMI.readU32( settings.endian ) != 1 && settings.output_level >= 1 )
                    *settings.output_ref << "Mission::ObjResource::load() 3DMI unexpected number at beginning!" << std::endl;
                
                this->bone_animation_data_size = (reader3DMI.totalSize() - reader3DMI.getPosition()) / sizeof( uint16_t );
                
                if( this->bone_animation_data != nullptr )
                    delete [] this->bone_animation_data;
                
                this->bone_animation_data = new int16_t [ this->bone_animation_data_size ];
                
                for( int d = 0; d < this->bone_animation_data_size; d++ )
                    this->bone_animation_data[ d ] = reader3DMI.readU16( settings.endian );
            }
            else
            if( identifier == TAG_3DTA ) {
                auto reader3DTA = reader.getReader( data_tag_size );
                
                if( settings.output_level >= 2 )
                    *settings.output_ref << "Mission::ObjResource::load() 3DTA" << std::endl;
            }
            else
            if( identifier == TAG_3DAL ) {
                auto reader3DAL = reader.getReader( data_tag_size );
                
                if( settings.output_level >= 2 )
                    *settings.output_ref << "Mission::ObjResource::load() 3DAL" << std::endl;
            }
            else
            if( identifier == TAG_4DVL ) {
                auto reader4DVL = reader.getReader( data_tag_size );
                
                auto start_number = reader4DVL.readU32( settings.endian );
                auto amount_of_vertices = reader4DVL.readU32( settings.endian );
                
                if( settings.output_level >= 2 )
                    *settings.output_ref << "Mission::ObjResource::load() 4DVL has 0x" << amount_of_vertices << " vertices" << std::endl;

                auto positions_pointer = &vertex_positions;

                // If vertex_positions is not empty then it is a morph target.
                if( positions_pointer->size() != 0 ) {
                    vertex_anm_positions.push_back( std::vector< glm::i16vec3 >() );
                    positions_pointer = &vertex_anm_positions.back();
                }

                positions_pointer->resize( amount_of_vertices );

                for( unsigned int i = 0; i < amount_of_vertices; i++ ) {
                    positions_pointer->at(i).x = reader4DVL.readI16( settings.endian );
                    positions_pointer->at(i).y = reader4DVL.readI16( settings.endian );
                    positions_pointer->at(i).z = reader4DVL.readI16( settings.endian );
                    // positions_pointer->at(i).w = Utilities::DataHandler::read_16( start_data, settings.is_opposite_endian );
                    reader4DVL.readI16( settings.endian );
                }
            }
            else
            if( identifier == TAG_4DNL ) {
                auto reader4DNL = reader.getReader( data_tag_size );
                
                auto start_number = reader4DNL.readU32( settings.endian );
                auto amount_of_normals = reader4DNL.readU32( settings.endian );

                if( settings.output_level >= 2 )
                    *settings.output_ref << "Mission::ObjResource::load() 4DNL has 0x" << amount_of_normals << " normals" << std::endl;

                auto normals_pointer = &vertex_normals;

                if( normals_pointer->size() != 0 ) {
                    vertex_anm_normals.push_back( std::vector< glm::i16vec3 >() );
                    normals_pointer = &vertex_anm_normals.back();
                }

                normals_pointer->resize( amount_of_normals );

                for( unsigned int i = 0; i < amount_of_normals; i++ ) {
                    normals_pointer->at(i).x = reader4DNL.readI16( settings.endian );
                    normals_pointer->at(i).y = reader4DNL.readI16( settings.endian );
                    normals_pointer->at(i).z = reader4DNL.readI16( settings.endian );

                    // normals_pointer->at(i).w = Utilities::DataHandler::read_16( start_data, settings.is_opposite_endian );
                    reader4DNL.readI16( settings.endian );
                }
            }
            else
            if( identifier == TAG_AnmD ) {
                auto readerAnmD = reader.getReader( data_tag_size );

                if( readerAnmD.readU32( settings.endian ) != 1 && settings.output_level >= 1 )
                    *settings.output_ref << "Mission::ObjResource::load() AnmD unexpected number at beginning!" << std::endl;

                auto data_size = readerAnmD.totalSize() - readerAnmD.getPosition();
                const auto TRACK_AMOUNT = data_size / 0x10;

                if( settings.output_level >= 2 )
                    *settings.output_ref << std::dec << "Mission::ObjResource::load() AnmD has " << TRACK_AMOUNT << " tracks" << std::endl << std::hex;

                unsigned int start = 0xFFFF, end = 0x0;
                
                for( unsigned int i = 0; i < TRACK_AMOUNT; i++ ) {
                    auto u0 = readerAnmD.readU8();
                    auto u1 = readerAnmD.readU8();
                    auto u2 = readerAnmD.readU8();
                    auto u3 = readerAnmD.readU8();
                    auto from_frame = readerAnmD.readU16( settings.endian );
                    auto to_frame   = readerAnmD.readU16( settings.endian );
                    auto u6 = readerAnmD.readU8();
                    auto u7 = readerAnmD.readU8();
                    auto u8 = readerAnmD.readU16( settings.endian );
                    auto frame_duration = readerAnmD.readU32( settings.endian );
                    
                    start = std::min( start, static_cast<unsigned int>( from_frame ) );
                    start = std::min( start, static_cast<unsigned int>( to_frame ) );
                    
                    end = std::max( end, static_cast<unsigned int>( from_frame ) );
                    end = std::max( end, static_cast<unsigned int>( to_frame ) );

                    if( settings.output_level >= 2 )
                        *settings.output_ref << std::dec
                              << "f: " << from_frame << " t: " << to_frame
                              << " f.d: " << frame_duration << std::endl
                              << std::hex;
                }
                
                frames_gen_AnmD = end - start + 1;
            }
            else
            if( identifier == TAG_3DBB ) {
                auto reader3DBB = reader.getReader( data_tag_size );
                
                bounding_box_per_frame = reader3DBB.readU32( settings.endian );
                
                if( bounding_box_per_frame >= 1 )
                {
                    // This is a proof by exhastion example of the first numbers that appears in the 3DBB tag.
                    // Numbers 1 through 7 appears throughout the English version of Future Cop on the ps1, Mac, and Windows.
                    // I could of used the greater and less thans, but this would not show that the numbers are every number that is 1, 2, 3, 4, 5, 6, 7
                    // This might be a bit field of some kind, this is most likely something like the number of bounding boxes.
                    assert( (bounding_box_per_frame == 1) | (bounding_box_per_frame == 2) | (bounding_box_per_frame == 3) | (bounding_box_per_frame == 4) | (bounding_box_per_frame == 5) | (bounding_box_per_frame == 6) | (bounding_box_per_frame == 7) );
                    
                    const auto bounding_box_amount = reader3DBB.readU32( settings.endian );
                    
                    bounding_boxes.reserve( bounding_box_amount );
                    
                    bounding_box_frames = bounding_box_amount / bounding_box_per_frame;
                    
                    size_t BOUNDING_BOX_SIZE = 8 * sizeof( uint16_t );
                    
                    size_t bounding_boxes_amount = bounding_box_frames * bounding_box_per_frame;
                    
                    size_t test_tag_size = bounding_boxes_amount * BOUNDING_BOX_SIZE + 4 * sizeof( uint32_t );
                    
                    if( test_tag_size != tag_size )
                    {
                        *settings.output_ref << "Mission::ObjResource::load() " << getIndexNumber() << std::endl;
                        *settings.output_ref << "Mission::ObjResource::load() bounding box per frame is " << bounding_box_per_frame << std::endl;
                        *settings.output_ref << "Mission::ObjResource::load() 3DBB frames is " << bounding_box_frames << std::endl;
                        *settings.output_ref << "Mission::ObjResource::load() The tag size should be " << test_tag_size;
                        *settings.output_ref << " instead it is " << tag_size << std::endl;
                    }
                    
                    // Another proof by exhaustion. The all the remaining data consists
                    // of the bounding_boxes_amount of data structs which are 0x10 in size.
                    assert( test_tag_size == tag_size );
                    
                    for( size_t frame_index = 0; frame_index < bounding_box_frames; frame_index++ )
                    {
                        for( size_t box_index = 0; box_index < bounding_box_per_frame; box_index++ )
                        {
                            bounding_boxes.push_back( BoundingBox3D() );
                            
                            // Fact Positive and negative: Assumption position x
                            bounding_boxes.back().x = reader3DBB.readI16( settings.endian );
                            
                            // Fact Positive and negative: Assumption position y
                            bounding_boxes.back().y = reader3DBB.readI16( settings.endian );
                            
                            // Fact Positive and negative: Assumption position z
                            bounding_boxes.back().z = reader3DBB.readI16( settings.endian );
                            
                            // Fact [0, 4224]: Assumption length x
                            bounding_boxes.back().length_x = reader3DBB.readU16( settings.endian );
                            assert( bounding_boxes.back().length_x >= 0 );
                            
                            // Fact [0, 1438]: Assumption length y
                            bounding_boxes.back().length_y = reader3DBB.readU16( settings.endian );
                            assert( bounding_boxes.back().length_y >= 0 );
                            
                            // Fact [0, 3584]: Assumption length z
                            bounding_boxes.back().length_z = reader3DBB.readU16( settings.endian );
                            assert( bounding_boxes.back().length_z >= 0 );
                            
                            // Fact [0, 4293]: Assumption rotation x
                            bounding_boxes.back().rotation_x = reader3DBB.readU16( settings.endian );
                            assert( bounding_boxes.back().rotation_x >= 0 );
                            
                            // Fact [0, 4293]: Assumption rotation y
                            bounding_boxes.back().rotation_y = reader3DBB.readU16( settings.endian );
                            assert( bounding_boxes.back().rotation_y >= 0 );
                        }
                    }
                }
                else
                {
                    *settings.output_ref << "Mission::ObjResource::load() 3DBB unexpected number at beginning! " << bounding_box_per_frame << std::endl;
                }
            }
            else
            {
                reader.setPosition( data_tag_size, Utilities::Buffer::CURRENT );
                
                char identifier_word[5] = {'\0'};
                const auto IDENTIFIER_SIZE = (sizeof( identifier_word ) - 1) / sizeof(identifier_word[0]);

                for( unsigned int i = 0; i < IDENTIFIER_SIZE; i++ ) {
                    identifier_word[ i ] = reinterpret_cast<char*>( &identifier )[ i ];
                }
                Utilities::DataHandler::swapBytes( reinterpret_cast< uint8_t* >( identifier_word ), 4 );

                if( settings.output_level >= 1 )
                    *settings.output_ref << "Mission::ObjResource::load() " << identifier_word << " not recognized" << std::endl;
                
                assert( false );
            }

            *settings.output_ref << std::dec;

            if( file_is_not_valid ) {
                if( settings.output_level >= 1 )
                    *settings.output_ref << "Mission::ObjResource::load() This is not a valid Obj file!" << std::endl;
                reader.setPosition( 0, Utilities::Buffer::END );
            }
        }
        
        // This assertion statement tells that there are only two options for
        // Animation either morphing or bone animation.
        assert( !(( bytes_per_frame_3DMI > 0 ) & ( vertex_anm_positions.size() > 0 )) );
        
        if( bytes_per_frame_3DMI > 0 )
        {
            this->bone_frames = this->bone_animation_data_size / (bytes_per_frame_3DMI / sizeof(int16_t));
            
            // This proves that the obj resource bone frame are roughly equvilent to the number of
            // bounding box frames.
            if( bounding_box_frames != bone_frames && bounding_box_frames + 1 != bone_frames )
            {
                *settings.output_ref << "Mission::ObjResource::load() " << getIndexNumber() << std::endl;
                *settings.output_ref << "Mission::ObjResource::load() bounding box per frame is " << bounding_box_per_frame << std::endl;
                *settings.output_ref << "Mission::ObjResource::load() 3DBB frames is " << bounding_box_frames << std::endl;
                *settings.output_ref << "Mission::ObjResource::load() 3DBB frames not equal to " << bone_frames << std::endl;
                
                assert( false );
            }
            
            if( num_frames_4DGI != bone_frames )
            {
                *settings.output_ref << "Mission::ObjResource::load() " << getIndexNumber() << std::endl;
                *settings.output_ref << "Mission::ObjResource::load() 4DGI frames is " << num_frames_4DGI << std::endl;
                *settings.output_ref << "Mission::ObjResource::load() 4DGI frames not equal to " << bone_frames << std::endl;
                
                assert( false );
            }
        }
        else
        if( vertex_anm_positions.size() > 0 )
        {
            // This proves that each model with morph animation has an equal number of
            // vertex frames as the bounding box frames.
            if( bounding_box_frames != vertex_anm_positions.size() + 1 )
            {
                *settings.output_ref << "Mission::ObjResource::load() " << getIndexNumber() << std::endl;
                *settings.output_ref << "Mission::ObjResource::load() bounding box per frame is " << bounding_box_per_frame << std::endl;
                *settings.output_ref << "Mission::ObjResource::load() 3DBB frames is " << bounding_box_frames << std::endl;
                *settings.output_ref << "Mission::ObjResource::load() 3DBB frames not equal to " << vertex_anm_positions.size() << std::endl;
                
                assert( false );
            }
            
            if( num_frames_4DGI != vertex_anm_positions.size() + 1 )
            {
                *settings.output_ref << "Mission::ObjResource::load() " << getIndexNumber() << std::endl;
                *settings.output_ref << "Mission::ObjResource::load() 4DGI frames is " << num_frames_4DGI << std::endl;
                *settings.output_ref << "Mission::ObjResource::load() 4DGI frames not equal to " << (vertex_anm_positions.size() + 1) << std::endl;
                
                assert( false );
            }
        }
        else
        {
            // This statement proves that each model without animation only has one bounding_box frame.
            assert( bounding_box_frames == 1 );
        }

        return !file_is_not_valid;
    }
    else
        return false;
}

Data::Mission::Resource * Data::Mission::ObjResource::duplicate() const {
    return new ObjResource( *this );
}

int Data::Mission::ObjResource::write( const char *const file_path, const std::vector<std::string> & arguments ) const {
    bool enable_export = true;
    int glTF_return = 0;

    for( auto arg = arguments.begin(); arg != arguments.end(); arg++ ) {
        if( (*arg).compare("--dry") == 0 )
            enable_export = false;
    }

    Utilities::ModelBuilder *model_output = createModel( &arguments );

    if( enable_export ) {
        // Make sure that the model has some vertex data.
        if( model_output->getNumVertices() >= 3 )
            glTF_return = model_output->write( std::string( file_path ) );
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

    delete model_output;

    return glTF_return;
}

bool Data::Mission::ObjResource::loadTextures( const std::vector<BMPResource*> &textures ) {
    if( textures.size() != 0 ) {
        bool valid = true;

        std::map< uint32_t, BMPResource* > resource_id_to_bmp;
        std::map< uint32_t, TextureReference > resource_id_to_reference;

        for( uint32_t i = 0; i < textures.size(); i++ )
            resource_id_to_bmp[ textures[ i ]->getResourceID() ] = textures[ i ];

        for( size_t i = 0; i < texture_quads.size(); i++ ) {
            if( resource_id_to_reference.count( texture_quads[ i ].index ) == 0 ) {
                resource_id_to_reference[ texture_quads[ i ].index ].resource_id = texture_quads[ i ].index + 1;

                if( resource_id_to_bmp.count( texture_quads[ i ].index ) != 0 ) {
                    if( resource_id_to_bmp[ texture_quads[ i ].index ]->getImageFormat() != nullptr ) {
                        resource_id_to_reference[ texture_quads[ i ].index ].name = resource_id_to_bmp[ texture_quads[ i ].index ]->getImageFormat()->appendExtension( resource_id_to_bmp[ texture_quads[ i ].index ]->getFullName( texture_quads[ i ].index  ) );
                    }
                }
                else
                    valid = false;
            }
        }

        texture_references.reserve( resource_id_to_reference.size() );

        for( auto it = resource_id_to_reference.begin(); it != resource_id_to_reference.end(); it++ ) {
            texture_references.push_back( (*it).second );
        }

        return valid;
    }
    else {
        assert( false );
        return false;
    }
}

Utilities::ModelBuilder * Data::Mission::ObjResource::createModel( const std::vector<std::string> * arguments ) const {
    Utilities::ModelBuilder *model_output = new Utilities::ModelBuilder();

    // This buffer will be used to store every triangle that the write function has.
    std::vector< FaceTriangle > triangle_buffer;
    std::vector<unsigned int> triangle_counts;
    bool is_specular = false;
    
    std::cout << "Cobj " << getResourceID() << " model is being built." << std::endl;
    std::cout << " vertex_positions.size() = " << vertex_positions.size() << std::endl;
    std::cout << " vertex_normals.size() = " << vertex_normals.size() << std::endl;
    std::cout << " texture_quads.size() = " << texture_quads.size() << std::endl;

    {
        triangle_buffer.reserve( face_trinagles.size() + face_quads.size() * 2 );

        for( auto i = face_trinagles.begin(); i != face_trinagles.end(); i++ ) {
            is_specular |= (*i).is_reflective;
            if( (*i).isWithinBounds( vertex_positions.size(), vertex_normals.size(), texture_quads.size(), texture_quads.data() ) )
            {
                triangle_buffer.push_back( (*i) );
            }
        }

        for( auto i = face_quads.begin(); i != face_quads.end(); i++ ) {
            is_specular |= (*i).is_reflective;
            if( (*i).firstTriangle().isWithinBounds( vertex_positions.size(), vertex_normals.size(), texture_quads.size(), texture_quads.data() ) )
            {
                triangle_buffer.push_back( (*i).firstTriangle() );
            }

            if( (*i).secondTriangle().isWithinBounds( vertex_positions.size(), vertex_normals.size(), texture_quads.size(), texture_quads.data() ) )
            {
                triangle_buffer.push_back( (*i).secondTriangle() );
            }
        }

        // Sort the triangle list.
        std::sort(triangle_buffer.begin(), triangle_buffer.end(), FaceTriangle() );

        int last_texture_quad_index = 0;

        // Get the list of the used textures
        for( auto i = triangle_buffer.begin(); i != triangle_buffer.end(); i++ ) {
            int index = (*i).texture_quad_ref->index;

            if( triangle_buffer.begin() == i || last_texture_quad_index != index) {
                triangle_counts.push_back( 0 );
                last_texture_quad_index = index;
            }
            triangle_counts.back()++;
        }
    }
    
    if( texture_quads.size() != 0 )
        assert( triangle_buffer.size() != 0 );

    unsigned int position_component_index = model_output->addVertexComponent( Utilities::ModelBuilder::POSITION_COMPONENT_NAME, Utilities::DataTypes::ComponentType::FLOAT, Utilities::DataTypes::Type::VEC3 );
    unsigned int normal_component_index = -1;
    
    normal_component_index = model_output->addVertexComponent( Utilities::ModelBuilder::NORMAL_COMPONENT_NAME, Utilities::DataTypes::ComponentType::FLOAT, Utilities::DataTypes::Type::VEC3 );
    
    unsigned int tex_coord_component_index = model_output->addVertexComponent( Utilities::ModelBuilder::TEX_COORD_0_COMPONENT_NAME, Utilities::DataTypes::ComponentType::UNSIGNED_BYTE, Utilities::DataTypes::Type::VEC2, true );
    unsigned int joints_0_component_index = -1;
    unsigned int weights_0_component_index = -1;
    unsigned int specular_component_index = -1; 
    
    // Specular is to exist if there is a single triangle or quad with a specular map.
    // if( is_specular )
    specular_component_index = model_output->addVertexComponent( "_Specular", Utilities::DataTypes::ComponentType::FLOAT, Utilities::DataTypes::Type::SCALAR );

    if( !bones.empty() ) {
        joints_0_component_index  = model_output->addVertexComponent( Utilities::ModelBuilder::JOINTS_INDEX_0_COMPONENT_NAME, Utilities::DataTypes::ComponentType::UNSIGNED_BYTE, Utilities::DataTypes::Type::VEC4, false );
        weights_0_component_index = model_output->addVertexComponent( Utilities::ModelBuilder::WEIGHTS_INDEX_0_COMPONENT_NAME, Utilities::DataTypes::ComponentType::UNSIGNED_BYTE, Utilities::DataTypes::Type::VEC4, true );

        model_output->allocateJoints( bones.size(), bone_frames );
        
        unsigned int childern[ max_bone_childern ];
        glm::mat4 bone_matrix;
        unsigned int opcode_decode = 0;
        
        const float ANGLE_UNITS_TO_RADIANS = M_PI / 2048.0;

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
                
                bone_matrix = glm::rotate( glm::mat4(1.0f), -static_cast<float>( frame_rotation.x ) * ANGLE_UNITS_TO_RADIANS, glm::vec3( 0, 1, 0 ) );
                bone_matrix = glm::rotate( bone_matrix, static_cast<float>( frame_rotation.y ) * ANGLE_UNITS_TO_RADIANS, glm::vec3( 1, 0, 0 ) );
                bone_matrix = glm::rotate( glm::mat4(1.0f), -static_cast<float>( frame_rotation.z ) * ANGLE_UNITS_TO_RADIANS, glm::vec3( 0, 0, 1 ) ) * bone_matrix;
                
                bone_matrix = glm::translate( glm::mat4(1.0f), glm::vec3(
                                  -static_cast<float>( frame_position.x ) * INTEGER_FACTOR,
                                   static_cast<float>( frame_position.y ) * INTEGER_FACTOR,
                                   static_cast<float>( frame_position.z ) * INTEGER_FACTOR) ) * bone_matrix;
                
                if( (*current_bone).parent_amount > 1 ) {
                    bone_matrix = model_output->getJointFrame( frame, childern[ (*current_bone).parent_amount - 2 ] ) * bone_matrix;
                }
                
                model_output->setJointFrame( frame, bone_index, bone_matrix );
            }
        }
    }
    unsigned int position_morph_component_index = -1;
    unsigned int normal_morph_component_index = -1;

    if( vertex_anm_positions.size() > 0 ) {
        position_morph_component_index = model_output->setVertexComponentMorph( position_component_index );
        normal_morph_component_index = model_output->setVertexComponentMorph( normal_component_index );
    }

    // TODO add texture settings.
    // Set the sampler for all the textures ever referenced.
    // model_output->setSampler( GLTFWriter::FilterType::NEAREST, GLTFWriter::FilterType::NEAREST_MIPMAP_LINEAR,
        // GLTFWriter::WrapType::CLAMP_TO_EDGE, GLTFWriter::WrapType::CLAMP_TO_EDGE );

    // Setup the vertex components now that every field had been entered.
    model_output->setupVertexComponents( vertex_anm_positions.size() );

    model_output->allocateVertices( triangle_buffer.size() * 3 );

    if( texture_references.size() == 0 )
        model_output->setMaterial( "" );
    /*
    for( unsigned int i = 0; i < texture_references.size(); i++ )
    {
        image_path = std::to_string( texture_references.at(i) ) + ".png";

        glTF_output->addTexturePath( image_path );
    }
    */

    glm::vec3 position;
    glm::vec3 new_position;
    glm::vec3 normal( 1, 0, 0 );
    glm::vec3 new_normal( 1, 0, 0 );
    float specular;
    glm::u8vec2 coords[3];
    glm::u8vec4 weights;
    glm::u8vec4 joints;

    // Future Cop only uses one joint, so it only needs one weight.
    weights.x = 0xFF;
    weights.y = weights.z = weights.w = 0;

    // The joint needs to be set to zero.
    joints.x = joints.y = joints.z = joints.w = 0;

    auto triangle = triangle_buffer.begin();

    for( unsigned int mat = 0; mat < triangle_counts.size(); mat++ )
    {
        model_output->setMaterial( texture_references.at( mat ).name, texture_references.at( mat ).resource_id );

        for( unsigned int i = 0; i < triangle_counts.at(mat); i++ )
        {
            triangleToCoords( (*triangle), coords );
            
            if( (*triangle).is_reflective )
                specular = 1.0f;
            else
                specular = 0.0f;

            model_output->startVertex();

            handlePositions( position, vertex_positions.data(), (*triangle).v0 );
            
            if( vertex_normals.size() != 0 )
                handleNormals( normal, vertex_normals.data(), (*triangle).n0 );
            
            model_output->setVertexData( position_component_index, Utilities::DataTypes::Vec3Type( position ) );
            
            model_output->setVertexData( normal_component_index, Utilities::DataTypes::Vec3Type( normal ) );
            
            model_output->setVertexData( tex_coord_component_index, Utilities::DataTypes::Vec2UByteType( coords[0] ) );
            if( is_specular )
            {
                model_output->setVertexData( specular_component_index, Utilities::DataTypes::ScalarType( specular ) );
            }
            for( unsigned int morph_frames = 0; morph_frames < vertex_anm_positions.size(); morph_frames++ )
            {
                handlePositions( new_position, vertex_anm_positions.at(morph_frames).data(), (*triangle).v0 );
                model_output->addMorphVertexData( position_morph_component_index, morph_frames, Utilities::DataTypes::Vec3Type( position ), Utilities::DataTypes::Vec3Type( new_position ) );
                
                if( vertex_normals.size() != 0 )
                    handleNormals( new_normal, vertex_anm_normals.at(morph_frames).data(), (*triangle).n0 );
                
                model_output->addMorphVertexData( normal_morph_component_index, morph_frames, Utilities::DataTypes::Vec3Type( normal ), Utilities::DataTypes::Vec3Type( new_normal ) );
            }
            if( !bones.empty() ) {
                for( auto bone = bones.begin(); bone != bones.end(); bone++) {
                    if( (*bone).vertex_start > (*triangle).v0 ) {
                        break;
                    }
                    else
                    if( (*bone).vertex_start + (*bone).vertex_stride > (*triangle).v0 )
                    {
                        joints.x = bone - bones.begin();
                        model_output->setVertexData( joints_0_component_index, Utilities::DataTypes::Vec4UByteType( joints ) );
                    }
                }

                model_output->setVertexData( weights_0_component_index, Utilities::DataTypes::Vec4UByteType( weights ) );
            }

            model_output->startVertex();

            handlePositions( position, vertex_positions.data(), (*triangle).v1 );
            
            if( vertex_normals.size() != 0 )
                handleNormals( normal, vertex_normals.data(), (*triangle).n1 );
            
            model_output->setVertexData( position_component_index, Utilities::DataTypes::Vec3Type( position ) );
            
            model_output->setVertexData( normal_component_index, Utilities::DataTypes::Vec3Type( normal ) );
            
            model_output->setVertexData( tex_coord_component_index, Utilities::DataTypes::Vec2UByteType( coords[1] ) );
            if( is_specular )
            {
                model_output->setVertexData( specular_component_index, Utilities::DataTypes::ScalarType( specular ) );
            }
            for( unsigned int morph_frames = 0; morph_frames < vertex_anm_positions.size(); morph_frames++ )
            {
                handlePositions( new_position, vertex_anm_positions.at(morph_frames).data(), (*triangle).v1 );
                model_output->addMorphVertexData( position_morph_component_index, morph_frames, Utilities::DataTypes::Vec3Type( position ), Utilities::DataTypes::Vec3Type( new_position ) );
                
                if( vertex_normals.size() != 0 )
                    handleNormals( new_normal, vertex_anm_normals.at(morph_frames).data(), (*triangle).n1 );
                
                model_output->addMorphVertexData( normal_morph_component_index, morph_frames, Utilities::DataTypes::Vec3Type( normal ), Utilities::DataTypes::Vec3Type( new_normal ) );
            }
            if( !bones.empty() ) {
                for( auto bone = bones.begin(); bone != bones.end(); bone++) {
                    if( (*bone).vertex_start > (*triangle).v1 ) {
                        break;
                    }
                    else
                    if( (*bone).vertex_start + (*bone).vertex_stride > (*triangle).v1 )
                    {
                        joints.x = bone - bones.begin();
                        model_output->setVertexData( joints_0_component_index, Utilities::DataTypes::Vec4UByteType( joints ) );
                    }
                }

                model_output->setVertexData( weights_0_component_index, Utilities::DataTypes::Vec4UByteType( weights ) );
            }

            model_output->startVertex();

            handlePositions( position, vertex_positions.data(), (*triangle).v2 );
            
            if( vertex_normals.size() != 0 )
                handleNormals( normal, vertex_normals.data(), (*triangle).n2 );
            
            model_output->setVertexData( position_component_index, Utilities::DataTypes::Vec3Type( position ) );
            
            model_output->setVertexData( normal_component_index, Utilities::DataTypes::Vec3Type( normal ) );
            
            model_output->setVertexData( tex_coord_component_index, Utilities::DataTypes::Vec2UByteType( coords[2] ) );
            if( is_specular )
            {
                model_output->setVertexData( specular_component_index, Utilities::DataTypes::ScalarType( specular ) );
            }
            for( unsigned int morph_frames = 0; morph_frames < vertex_anm_positions.size(); morph_frames++ )
            {
                handlePositions( new_position, vertex_anm_positions.at(morph_frames).data(), (*triangle).v2 );
                model_output->addMorphVertexData( position_morph_component_index, morph_frames, Utilities::DataTypes::Vec3Type( position ), Utilities::DataTypes::Vec3Type( new_position ) );
                
                if( vertex_normals.size() != 0 )
                    handleNormals( new_normal, vertex_anm_normals.at(morph_frames).data(), (*triangle).n2 );
                
                model_output->addMorphVertexData( normal_morph_component_index, morph_frames, Utilities::DataTypes::Vec3Type( normal ), Utilities::DataTypes::Vec3Type( new_normal ) );
            }
            if( !bones.empty() ) {
                for( auto bone = bones.begin(); bone != bones.end(); bone++) {
                    if( (*bone).vertex_start > (*triangle).v2 ) {
                        break;
                    }
                    else
                    if( (*bone).vertex_start + (*bone).vertex_stride > (*triangle).v2 )
                    {
                        joints.x = bone - bones.begin();
                        model_output->setVertexData( joints_0_component_index, Utilities::DataTypes::Vec4UByteType( joints ) );
                    }
                }

                model_output->setVertexData( weights_0_component_index, Utilities::DataTypes::Vec4UByteType( weights ) );
            }

            triangle++;
        }
    }
    return model_output;
}

Utilities::ModelBuilder * Data::Mission::ObjResource::createBoundingBoxes() const {
    if(bounding_box_per_frame > 0 && bounding_box_frames > 0) {
        Utilities::ModelBuilder *box_output = new Utilities::ModelBuilder( Utilities::ModelBuilder::LINES );
        
        unsigned int position_component_index = box_output->addVertexComponent( Utilities::ModelBuilder::POSITION_COMPONENT_NAME, Utilities::DataTypes::ComponentType::FLOAT, Utilities::DataTypes::Type::VEC3 );
        unsigned int color_coord_component_index = box_output->addVertexComponent( Utilities::ModelBuilder::TEX_COORD_0_COMPONENT_NAME, Utilities::DataTypes::ComponentType::FLOAT, Utilities::DataTypes::Type::VEC2 );
        unsigned int position_morph_component_index = 0;
        
        // TODO Add morph animations.
        // if( bounding_box_frames > 1 )
        //    position_morph_component_index = box_output->setVertexComponentMorph( position_component_index );
        
        glm::vec3 position;
        glm::vec3 color(0.0f, 1.0f, 0.0f);
        
        // At this point it is time to start generating bounding box.
        
        // No texture should be used for this bounding box.
        box_output->setMaterial( "" );
        
        for( unsigned int box_index = 0; box_index < bounding_box_per_frame; box_index++ )
        {
            // TODO This is right now a line not a box.
            
            const BoundingBox3D &current_box = bounding_boxes[ box_index ];
            
            position.x = -(current_box.x + current_box.length_x) * INTEGER_FACTOR;
            position.y =  (current_box.y + current_box.length_y) * INTEGER_FACTOR;
            position.z =  (current_box.z + current_box.length_z) * INTEGER_FACTOR;
            
            box_output->startVertex();
            box_output->setVertexData( position_component_index, Utilities::DataTypes::Vec3Type( position ) );
            box_output->setVertexData( color_coord_component_index, Utilities::DataTypes::Vec3Type( color ) );
            
            position.x = -(current_box.x - current_box.length_x) * INTEGER_FACTOR;
            position.y =  (current_box.y - current_box.length_y) * INTEGER_FACTOR;
            position.z =  (current_box.z - current_box.length_z) * INTEGER_FACTOR;
            
            box_output->startVertex();
            box_output->setVertexData( position_component_index, Utilities::DataTypes::Vec3Type( position ) );
            box_output->setVertexData( color_coord_component_index, Utilities::DataTypes::Vec3Type( color ) );
        }
        
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

std::vector<Data::Mission::ObjResource*> Data::Mission::ObjResource::getVector( Data::Mission::IFF &mission_file ) {
    std::vector<Resource*> to_copy = mission_file.getResources( Data::Mission::ObjResource::IDENTIFIER_TAG );

    std::vector<ObjResource*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<ObjResource*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::ObjResource*> Data::Mission::ObjResource::getVector( const Data::Mission::IFF &mission_file ) {
    return Data::Mission::ObjResource::getVector( const_cast< IFF& >( mission_file ) );
}
