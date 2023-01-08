#include "FUNResource.h"
#include <limits>
#include <cassert>

#include <iostream>

const std::string Data::Mission::FUNResource::FILE_EXTENSION = "fun";
// which is { 0x43, 0x66, 0x75, 0x6E } or { 'C', 'f', 'u', 'n' } or "Cfun"
const uint32_t Data::Mission::FUNResource::IDENTIFIER_TAG = 0x4366756e;

Data::Mission::FUNResource::FUNResource() {
}

Data::Mission::FUNResource::FUNResource( const FUNResource &obj ) {
}

std::string Data::Mission::FUNResource::getFileExtension() const {
    return Data::Mission::FUNResource::FILE_EXTENSION;
}

uint32_t Data::Mission::FUNResource::getResourceTagID() const {
    return Data::Mission::FUNResource::IDENTIFIER_TAG;
}

bool Data::Mission::FUNResource::parse( const ParseSettings &settings ) {
    const size_t  TAG_HEADER_SIZE = 2 * sizeof(uint32_t);
    const size_t NUM_ENTRIES_SIZE = sizeof(uint32_t);
    const size_t       ENTRY_SIZE = 8 * sizeof(uint8_t);
    const uint32_t TAG_tFUN = 0x7446554e;
    // which is { 0x74, 0x46, 0x55, 0x4e } or { 't', 'F', 'U', 'N' } or "tFUN"
    const uint32_t TAG_tEXT = 0x74455854;
    // which is { 0x74, 0x45, 0x58, 0x54 } or { 't', 'E', 'X', 'T' } or "tEXT"
    
    uint32_t data_id;
    Function fun_struct;
    
    if( this->data_p != nullptr )
    {
        auto reader = this->data_p->getReader();

        if( reader.totalSize() >= TAG_HEADER_SIZE ) {
            auto header    = reader.readU32( settings.endian );
            auto size      = reader.readU32( settings.endian );

            if( header == TAG_tFUN ) {
                auto reader_tfun = reader.getReader( size - TAG_HEADER_SIZE );
                
                data_id = reader_tfun.readU32( settings.endian );
                assert( data_id == 1 );
                
                auto unk = std::numeric_limits<int32_t>::min();
                int last = -1;
                
                while( !reader_tfun.ended() ) {
                    fun_struct.type  = reader_tfun.readI32( settings.endian );
                    fun_struct.unk_1 = reader_tfun.readI32( settings.endian );
                    fun_struct.zero  = reader_tfun.readI32( settings.endian );
                    fun_struct.pos_x = reader_tfun.readI32( settings.endian );
                    fun_struct.pos_y = reader_tfun.readI32( settings.endian );
                    
                    unk = std::max( unk, fun_struct.unk_1 );
                    
                    assert( last < fun_struct.pos_x );
                    last = fun_struct.pos_x;
                    
                    assert( ( fun_struct.type == 1 ) | ( fun_struct.type == -1 ) | (fun_struct.type == 0 ) | ( fun_struct.type == 25 ) | ( fun_struct.type == 9999 ) );
                    assert( fun_struct.zero == 0 );
                    assert( fun_struct.pos_x < fun_struct.pos_y );
                    
                    functions.push_back( fun_struct );
                }
                
                assert( functions.size() != 0 );
                
                auto header    = reader.readU32( settings.endian );
                auto size      = reader.readU32( settings.endian );
                
                if( header == TAG_tEXT ) {
                    auto reader_ext = reader.getReader( size - TAG_HEADER_SIZE );
                    
                    data_id = reader_ext.readU32( settings.endian );
                    assert( data_id == 1 );
                    
                    ext_bytes = reader_ext.getBytes();
                    
                    assert( ext_bytes.size() != 0 );
                    assert( ext_bytes.size() > functions.back().pos_y );
                    assert( reader.ended() );
                    
                    std::cout << "ext_bytes.size() = " << ext_bytes.size() << std::endl;
                    std::cout << "functions.size() = " << functions.size() << std::endl;
                    
                    return true;
                }
            }
        }
    }
    
    assert( 0 );
    return false;
}

Data::Mission::Resource * Data::Mission::FUNResource::duplicate() const {
    return new FUNResource( *this );
}

int Data::Mission::FUNResource::write( const char *const file_path, const std::vector<std::string> & arguments ) const {
    return 0;
}
