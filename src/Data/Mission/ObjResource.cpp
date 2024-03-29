#include "ObjResource.h"

#include "IFF.h"

#include <glm/gtc/constants.hpp>
#include <glm/geometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <algorithm>
#include <cmath>
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

    void triangleToCoords( const Data::Mission::ObjResource::FaceTriangle &triangle, const Data::Mission::ObjResource::TextureQuad &texture_quad, glm::u8vec2 *cords )
    {
        if( !triangle.is_other_side )
        {
            cords[0] = texture_quad.coords[0];
            cords[1] = texture_quad.coords[1];
            cords[2] = texture_quad.coords[2];
        }
        else
        {
            cords[0] = texture_quad.coords[2];
            cords[1] = texture_quad.coords[3];
            cords[2] = texture_quad.coords[0];
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
    if( bmp_id >= 0 )
    {
        return (static_cast<size_t>(bmp_id) > texture_amount);
    }
    else
    if( bmp_id < 0 )
    {
        return (static_cast<size_t>(-bmp_id - 1) > texture_amount);
    }
    else
        return false; // This statement should not be reached.
}

bool Data::Mission::ObjResource::FaceTriangle::isWithinBounds( uint32_t vertex_limit, uint32_t normal_limit, uint32_t texture_quad_limit ) const {
    bool is_valid = true;

    if( texture_quad_index >= texture_quad_limit )
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

bool Data::Mission::ObjResource::FaceTriangle::getTransparency() const {
    if( is_reflective || texture_quad_r == nullptr )
        return false;
    else
    if( is_other_side )
        return texture_quad_r->has_transparent_pixel_t1;
    else
        return texture_quad_r->has_transparent_pixel_t0;
}

bool Data::Mission::ObjResource::FaceTriangle::operator() ( const FaceTriangle & l_operand, const FaceTriangle & r_operand ) const {
    if( l_operand.texture_quad_r != nullptr && r_operand.texture_quad_r != nullptr ) {
        if( l_operand.texture_quad_r->bmp_id != r_operand.texture_quad_r->bmp_id )
            return (l_operand.texture_quad_r->bmp_id < r_operand.texture_quad_r->bmp_id);
        else
        if( l_operand.getTransparency() == false && r_operand.getTransparency() == true )
            return true;
        else
            return false;
    }
    else
    if( l_operand.texture_quad_r != nullptr )
        return false;
    else
        return true;
}

Data::Mission::ObjResource::FaceTriangle Data::Mission::ObjResource::FaceQuad::firstTriangle() const {
    FaceTriangle new_tri;

    new_tri.is_other_side = false;
    new_tri.is_reflective = is_reflective;
    new_tri.texture_quad_index = texture_quad_index;
    new_tri.texture_quad_r = texture_quad_r;
    new_tri.v0 = v0;
    new_tri.v1 = v1;
    new_tri.v2 = v2;
    new_tri.n0 = n0;
    new_tri.n1 = n1;
    new_tri.n2 = n2;

    return new_tri;
}

Data::Mission::ObjResource::FaceTriangle Data::Mission::ObjResource::FaceQuad::secondTriangle() const {
    FaceTriangle new_tri;

    new_tri.is_other_side = true;
    new_tri.is_reflective = is_reflective;
    new_tri.texture_quad_index = texture_quad_index;
    new_tri.texture_quad_r = texture_quad_r;
    new_tri.v0 = v2;
    new_tri.v1 = v3;
    new_tri.v2 = v0;
    new_tri.n0 = n2;
    new_tri.n1 = n3;
    new_tri.n2 = n0;

    return new_tri;

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

        while( reader.getPosition( Utilities::Buffer::BEGIN ) < reader.totalSize() ) {
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
                            warning_log.output << "4DGI bitfield actually is 0x" << std::hex << bitfield << "\n";
                        }
                    }
                    else
                        warning_log.output << "4DGI id is 0x" << std::hex << id << "\n";

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
                    auto un9 = reader4DGI.readU8(); // Unknown 0xFF and 0x6F values found.
                    auto un10 = reader4DGI.readU8(); // Unknown 0xFF and 0x12 values found.
                    auto un11 = reader4DGI.readU8(); // Unknown 0xFF and 0x00 values found.
                    auto un12 = reader4DGI.readU8(); // Unknown 0xFF and 0x00 values found.
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

                size_t texture_ref_amount = (reader3DTL.totalSize() - reader3DTL.getPosition()) / 0x10;
                debug_log.output << "triangle amount " << std::dec << texture_ref_amount << "\n";

                for( size_t i = 0; i < texture_ref_amount; i++ )
                {
                    reader3DTL.readU32( settings.endian ); // Skip unknown bytes

                    texture_quads.push_back( TextureQuad() );

                    texture_quads.back().has_transparent_pixel_t0 = false;
                    texture_quads.back().has_transparent_pixel_t1 = false;

                    texture_quads.back().coords[0].x = reader3DTL.readU8();
                    texture_quads.back().coords[0].y = reader3DTL.readU8();

                    texture_quads.back().coords[1].x = reader3DTL.readU8();
                    texture_quads.back().coords[1].y = reader3DTL.readU8();

                    texture_quads.back().coords[2].x = reader3DTL.readU8();
                    texture_quads.back().coords[2].y = reader3DTL.readU8();

                    texture_quads.back().coords[3].x = reader3DTL.readU8();
                    texture_quads.back().coords[3].y = reader3DTL.readU8();
                    
                    // For some reason the Slim's Windows English version has a 64th Cobj that goes beyond 10 textures.
                    
                    texture_quads.back().bmp_id = reader3DTL.readU32( settings.endian );
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
                    auto face_type   = reader3DQL.readU8();
                    if( !(( (face_type & 0x07) == 3 ) || ((face_type & 0x07) == 4 )) )
                        warning_log.output << "3DQL has 0x" << std::hex  << static_cast<uint32_t>(face_type) << " face_type\n";
                    auto face_type_2 = reader3DQL.readU8();

                    const size_t texture_quad_index = reader3DQL.readU16( settings.endian ) / 0x10;

                    bool reflect = ((face_type_2 & 0xF0) == 0x80);
                    
                    // Data::Mission::Resource::ParseSettings::Macintosh
                    
                    if( (face_type & 0x07) == 4 ) {
                        face_quads.push_back( FaceQuad() );

                        face_quads.back().texture_quad_index = texture_quad_index;
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
                        face_trinagles.back().texture_quad_index = texture_quad_index;
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
                auto reader3DRF = reader.getReader( data_tag_size );
                
                debug_log.output << "3DRF" << std::hex << "\n"
                    << "Index " << getIndexNumber() << "\n"
                    << "reader3DRF.totalSize() = " << reader3DRF.totalSize() << "\n";

                if( reader3DRF.totalSize() != 0x10 )
                    warning_log.output << "reader3DRF.totalSize() is not 0x10, but 0x" << std::hex << reader3DRF.totalSize() << ".\n";
            }
            else
            if( identifier == TAG_3DRL ) {
                debug_log.output << "3DRL" << std::endl;
                auto reader3DRL = reader.getReader( data_tag_size );
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
                    
                    debug_log.output << "bone: ";

                    debug_log.output
                        << "parent index: 0x" << std::hex << bones.at(i).parent_amount << ", "
                        << "0x" <<    bones.at(i).normal_start << " with 0x" <<  bones.at(i).normal_stride << " normals, "
                        << "0x" <<    bones.at(i).vertex_start << " with 0x" <<  bones.at(i).vertex_stride << " vertices, "
                        << "opcode: 0x" << opcode << std::dec
                        << ", position( " << bones.at(i).position.x << ", " << bones.at(i).position.y << ", " << bones.at(i).position.z << " )"
                        << ", rotation( " << bones.at(i).rotation.x << ", " << bones.at(i).rotation.y << ", " << bones.at(i).rotation.z << " )\n";
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
            if( identifier == TAG_3DTA ) {
                auto reader3DTA = reader.getReader( data_tag_size );
                
                debug_log.output << "3DTA is not handled yet.\n";
            }
            else
            if( identifier == TAG_3DAL ) {
                auto reader3DAL = reader.getReader( data_tag_size );
                
                debug_log.output << "3DAL is not handled yet.\n";
            }
            else
            if( identifier == TAG_4DVL ) {
                auto reader4DVL = reader.getReader( data_tag_size );
                
                auto start_number = reader4DVL.readU32( settings.endian );
                auto amount_of_vertices = reader4DVL.readU32( settings.endian );
                
                debug_log.output << "4DVL has 0x" << std::hex << amount_of_vertices << " vertices.\n";

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

                debug_log.output << "4DNL has 0x" << std::hex << amount_of_normals << " normals.\n";

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

                if( readerAnmD.readU32( settings.endian ) != 1 )
                    warning_log.output << "AnmD unexpected number at beginning!\n";

                auto data_size = readerAnmD.totalSize() - readerAnmD.getPosition();
                const auto TRACK_AMOUNT = data_size / 0x10;

                debug_log.output << std::dec << "AnmD has " << TRACK_AMOUNT << " tracks\n";

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

                    debug_log.output << std::dec << "f: " << from_frame << " t: " << to_frame << " f.d: " << frame_duration << ".\n";
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
                            
                            // Fact Positive and negative: Assumption position x
                            bounding_boxes.back().x = reader3DBB.readI16( settings.endian );
                            
                            // Fact Positive and negative: Assumption position y
                            bounding_boxes.back().y = reader3DBB.readI16( settings.endian );
                            
                            // Fact Positive and negative: Assumption position z
                            bounding_boxes.back().z = reader3DBB.readI16( settings.endian );
                            
                            // Fact [0, 4224]: Assumption length x
                            bounding_boxes.back().length_x = reader3DBB.readU16( settings.endian );
                            
                            // Fact [0, 1438]: Assumption length y
                            bounding_boxes.back().length_y = reader3DBB.readU16( settings.endian );
                            
                            // Fact [0, 3584]: Assumption length z
                            bounding_boxes.back().length_z = reader3DBB.readU16( settings.endian );
                            
                            // Fact [0, 4293]: Assumption rotation x
                            bounding_boxes.back().rotation_x = reader3DBB.readU16( settings.endian );
                            
                            // Fact [0, 4293]: Assumption rotation y
                            bounding_boxes.back().rotation_y = reader3DBB.readU16( settings.endian );
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
        
        // This warning tells that there are only two options for animations either morphing or bone animation.
        if( !( !(( bytes_per_frame_3DMI > 0 ) & ( vertex_anm_positions.size() > 0 )) ) )
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
        }
        else
        if( vertex_anm_positions.size() > 0 )
        {
            // This proves that each model with morph animation has an equal number of
            // vertex frames as the bounding box frames.
            if( bounding_box_frames != vertex_anm_positions.size() + 1 )
            {
                error_log.output << "bounding box per frame is " << std::dec << bounding_box_per_frame << "\n";
                error_log.output << "3DBB frames is " << bounding_box_frames << "\n";
                error_log.output << "3DBB frames not equal to " << vertex_anm_positions.size() << "\n";
            }
            
            if( num_frames_4DGI != vertex_anm_positions.size() + 1 )
            {
                error_log.output << "4DGI frames is " << std::dec << num_frames_4DGI << "\n";
                error_log.output << "4DGI frames not equal to " << (vertex_anm_positions.size() + 1) << "\n";
            }
        }
        else
        {
            // This statement proves that each model without animation only has one bounding_box frame.
            if( bounding_box_frames != 1 )
                error_log.output << "4DGI bounding_box_frames = " << std::dec << bounding_box_frames << "\n";
        }

        for( auto &triangle : this->face_trinagles ) {
            if( this->texture_quads.size() > triangle.texture_quad_index ) {
                triangle.texture_quad_r = &this->texture_quads[ triangle.texture_quad_index ];
            }
            else
                triangle.texture_quad_r = nullptr;
        }
        for( auto &quad : this->face_quads ) {
            if( this->texture_quads.size() > quad.texture_quad_index ) {
                quad.texture_quad_r = &this->texture_quads[ quad.texture_quad_index ];
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

int Data::Mission::ObjResource::write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    int glTF_return = 0;

    Utilities::ModelBuilder *model_output = createModel();

    if( iff_options.obj.shouldWrite( iff_options.enable_global_dry_default ) ) {
        // Make sure that the model has some vertex data.
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

    delete model_output;

    return glTF_return;
}

bool Data::Mission::ObjResource::loadTextures( const std::vector<BMPResource*> &textures ) {
    if( textures.size() != 0 ) {
        bool valid = true;

        std::map< uint32_t, BMPResource* > resource_id_to_bmp;
        std::map< uint32_t, TextureReference > resource_id_to_reference;

        glm::vec2 points[6];

        for( uint32_t i = 0; i < textures.size(); i++ )
            resource_id_to_bmp[ textures[ i ]->getResourceID() ] = textures[ i ];

        for( size_t i = 0; i < texture_quads.size(); i++ ) {
            
            const auto RESOURCE_ID = texture_quads[ i ].bmp_id + 1;
            
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

        for( size_t i = 0; i < texture_quads.size(); i++ ) {
            const auto RESOURCE_ID = texture_quads[ i ].bmp_id + 1;

            if( resource_id_to_bmp.count( RESOURCE_ID ) != 0 ) {
                auto bmp_reference_r = resource_id_to_bmp[ RESOURCE_ID ];

                points[0] = this->texture_quads[ i ].coords[0];
                points[1] = this->texture_quads[ i ].coords[1];
                points[2] = this->texture_quads[ i ].coords[2];

                points[3] = this->texture_quads[ i ].coords[2];
                points[4] = this->texture_quads[ i ].coords[3];
                points[5] = this->texture_quads[ i ].coords[0];

                if( Data::Mission::BMPResource::isSemiTransparent( *bmp_reference_r->getImage(), &points[0] ) )
                    this->texture_quads[ i ].has_transparent_pixel_t0 = true;
                if( Data::Mission::BMPResource::isSemiTransparent( *bmp_reference_r->getImage(), &points[3] ) )
                    this->texture_quads[ i ].has_transparent_pixel_t1 = true;
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
    Utilities::ModelBuilder *model_output = new Utilities::ModelBuilder();

    // This buffer will be used to store every triangle that the write function has.
    std::vector< FaceTriangle > triangle_buffer;
    std::vector<unsigned int> triangle_counts;
    bool is_specular = false;

    {
        triangle_buffer.reserve( face_trinagles.size() + face_quads.size() * 2 );

        for( auto i = face_trinagles.begin(); i != face_trinagles.end(); i++ ) {
            is_specular |= (*i).is_reflective;
            if( (*i).isWithinBounds( vertex_positions.size(), vertex_normals.size(), texture_quads.size() ) )
            {
                triangle_buffer.push_back( (*i) );
            }
        }

        for( auto i = face_quads.begin(); i != face_quads.end(); i++ ) {
            is_specular |= (*i).is_reflective;
            if( (*i).firstTriangle().isWithinBounds( vertex_positions.size(), vertex_normals.size(), texture_quads.size() ) )
            {
                triangle_buffer.push_back( (*i).firstTriangle() );
            }

            if( (*i).secondTriangle().isWithinBounds( vertex_positions.size(), vertex_normals.size(), texture_quads.size() ) )
            {
                triangle_buffer.push_back( (*i).secondTriangle() );
            }
        }

        // Sort the triangle list.
        std::sort(triangle_buffer.begin(), triangle_buffer.end(), FaceTriangle() );

        uint32_t last_texture_quad_index = 0;

        // Get the list of the used textures
        for( auto i = triangle_buffer.begin(); i != triangle_buffer.end(); i++ ) {
            uint32_t bmp_id = (*i).texture_quad_r->bmp_id;

            if( triangle_buffer.begin() == i || last_texture_quad_index != bmp_id ) {
                triangle_counts.push_back( 0 );
                last_texture_quad_index = bmp_id;
            }
            triangle_counts.back()++;
        }
    }

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
        
        glm::mat4 bone_matrix;
        
        const float ANGLE_UNITS_TO_RADIANS = glm::pi<float>() / 2048.0;
        
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
                
                bone_matrix = glm::rotate( glm::mat4(1.0f), -static_cast<float>( frame_rotation.x ) * ANGLE_UNITS_TO_RADIANS, glm::vec3( 0, 1, 0 ) );
                bone_matrix = glm::rotate( bone_matrix, static_cast<float>( frame_rotation.y ) * ANGLE_UNITS_TO_RADIANS, glm::vec3( 1, 0, 0 ) );
                bone_matrix = glm::rotate( glm::mat4(1.0f), -static_cast<float>( frame_rotation.z ) * ANGLE_UNITS_TO_RADIANS, glm::vec3( 0, 0, 1 ) ) * bone_matrix;
                
                auto position  = glm::vec3( -frame_position.x, frame_position.y, frame_position.z ) * static_cast<float>( INTEGER_FACTOR );
                auto quaterion = glm::quat_cast( bone_matrix );
                
                model_output->setJointFrame( frame, bone_index, position, quaterion );
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
    auto previous_triangle = triangle_buffer.begin();

    for( unsigned int mat = 0; mat < triangle_counts.size(); mat++ )
    {
        model_output->setMaterial( texture_references.at( mat ).name, texture_references.at( mat ).resource_id, true );

        for( unsigned int i = 0; i < triangle_counts.at(mat); i++ )
        {
            triangleToCoords( (*triangle), *(*triangle).texture_quad_r, coords );
            
            if( (*triangle).is_reflective )
                specular = 1.0f;
            else
                specular = 0.0f;

            if( triangle != previous_triangle ) {
                if( (*triangle).texture_quad_r != nullptr && (*previous_triangle).texture_quad_r != nullptr && // Memory safety.
                    (*triangle).texture_quad_r->bmp_id == (*previous_triangle).texture_quad_r->bmp_id )
                {
                    if( (*previous_triangle).getTransparency() == false && (*triangle).getTransparency() == true )
                        model_output->beginSemiTransperency();
                    else
                    if( (*previous_triangle).getTransparency() == true  && (*triangle).getTransparency() == false )
                        assert( false && "Sorting is wrong!" );
                }
                else if( (*triangle).getTransparency() ) {
                    model_output->beginSemiTransperency();
                }
            }
            else if( (*triangle).getTransparency() ) {
                model_output->beginSemiTransperency();
            }

            model_output->startVertex();

            handlePositions( position, vertex_positions.data(), (*triangle).v2 );
            
            if( vertex_normals.size() != 0 )
                handleNormals( normal, vertex_normals.data(), (*triangle).n2 );
            
            model_output->setVertexData( position_component_index, Utilities::DataTypes::Vec3Type( position ) );
            
            model_output->setVertexData( normal_component_index, Utilities::DataTypes::Vec3Type( normal ) );
            
            model_output->setVertexData( tex_coord_component_index, Utilities::DataTypes::Vec2UByteType( coords[2] ) );

            if( is_specular )
                model_output->setVertexData( specular_component_index, Utilities::DataTypes::ScalarType( specular ) );

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

            triangle++;
            previous_triangle = triangle - 1;
        }
    }
    
    model_output->finish();
    
    return model_output;
}

Utilities::ModelBuilder * Data::Mission::ObjResource::createBoundingBoxes() const {
    if(bounding_box_per_frame > 0 && bounding_box_frames > 0) {
        Utilities::ModelBuilder *box_output = new Utilities::ModelBuilder( Utilities::ModelBuilder::LINES );
        
        unsigned int position_component_index = box_output->addVertexComponent( Utilities::ModelBuilder::POSITION_COMPONENT_NAME, Utilities::DataTypes::ComponentType::FLOAT, Utilities::DataTypes::Type::VEC3 );
        unsigned int color_coord_component_index = box_output->addVertexComponent( Utilities::ModelBuilder::TEX_COORD_0_COMPONENT_NAME, Utilities::DataTypes::ComponentType::FLOAT, Utilities::DataTypes::Type::VEC2 );
        // unsigned int position_morph_component_index = 0;
        
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

bool Data::Mission::IFFOptions::ObjOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {
    return IFFOptions::ResourceOption::readParams( arguments, output_r );
}

std::string Data::Mission::IFFOptions::ObjOption::getOptions() const {
    std::string information_text = getBuiltInOptions();

    return information_text;
}
