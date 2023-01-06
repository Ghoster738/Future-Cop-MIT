#include "FUNResource.h"
#include <cassert>

#include <iostream>

const std::string Data::Mission::FUNResource::FILE_EXTENSION = "fun";
// which is { 0x43, 0x66, 0x75, 0x6E } or { 'C', 'f', 'u', 'n' } or "Cfun"
const uint32_t    Data::Mission::FUNResource::IDENTIFIER_TAG = 0x4366756e;

int32_t Data::Mission::FUNResource::min = std::numeric_limits<int32_t>::max();
int32_t Data::Mission::FUNResource::max = std::numeric_limits<int32_t>::min();

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
    
    if( this->data_p != nullptr )
    {
        auto reader = this->data_p->getReader();

        if( reader.totalSize() >= TAG_HEADER_SIZE ) {
            auto header    = reader.readU32( settings.endian );
            auto size      = reader.readU32( settings.endian );

            if( header == TAG_tFUN ) {
                auto reader_tfun = reader.getReader( size - TAG_HEADER_SIZE );
                
                fun_id = reader_tfun.readU32( settings.endian );
                assert( fun_id == 1 );
                
                while( !reader_tfun.ended() ) {
                    fun_numbers.push_back( reader_tfun.readI32( settings.endian ) );
                    
                    min = std::min( min, fun_numbers.back() );
                    max = std::max( max, fun_numbers.back() );
                }
                
                assert( fun_numbers.size() != 0 );
                
                auto header    = reader.readU32( settings.endian );
                auto size      = reader.readU32( settings.endian );
                
                if( header == TAG_tEXT ) {
                    auto reader_ext = reader.getReader( size - TAG_HEADER_SIZE );
                    
                    ext_id = reader_ext.readU32( settings.endian );
                    assert( ext_id == 1 );
                    
                    ext_bytes = reader_ext.getBytes();
                    
                    std::cout << "ext_bytes = " << std::dec << ext_bytes.size() << std::endl;
                    
                    assert( ext_bytes.size() != 0 );
                    assert( reader.ended() );
                    
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
