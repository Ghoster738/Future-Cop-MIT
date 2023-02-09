#include "FUNResource.h"
#include <limits>
#include <cassert>

#include <iostream>

const std::string Data::Mission::FUNResource::FILE_EXTENSION = "fun";
// which is { 0x43, 0x66, 0x75, 0x6E } or { 'C', 'f', 'u', 'n' } or "Cfun"
const uint32_t Data::Mission::FUNResource::IDENTIFIER_TAG = 0x4366756e;

namespace {
bool convert( const uint8_t *const number_r, size_t number_limit, uint64_t &big_number, size_t &size_read ) {
    const uint8_t * number_head_r = number_r;
    
    big_number |= (*number_head_r & 0x7F);
    
    while( (*number_head_r & 0x80) == 0 && number_limit > 0) {
        big_number = big_number << 7;
        big_number |= (*number_head_r & 0x7F);
        number_head_r++;
        number_limit--;
        size_read++;
    }
    
    return true;
}
}

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
    
    // std::cout << "FUNResource 0x" << std::hex << getOffset() << std::endl;
    
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
                
                while( !reader_tfun.ended() ) {
                    fun_struct.faction    = reader_tfun.readI32( settings.endian );
                    fun_struct.identifier = reader_tfun.readI32( settings.endian );
                    fun_struct.zero       = reader_tfun.readI32( settings.endian );
                    fun_struct.start_parameter_offset = reader_tfun.readU32( settings.endian );
                    fun_struct.start_code_offset      = reader_tfun.readU32( settings.endian );
                    
                    assert( ( fun_struct.faction == 1 ) | ( fun_struct.faction == -1 ) | (fun_struct.faction == 0 ) | ( fun_struct.faction == 25 ) | ( fun_struct.faction == 9999 ) );
                    assert( fun_struct.zero == 0 );
                    assert( fun_struct.start_parameter_offset < fun_struct.start_code_offset );
                    
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
                    assert( ext_bytes.size() > functions.back().start_code_offset );
                    assert( reader.ended() );
                    
                    while( ext_bytes.back() != 0 ){
                        last_ext.insert( last_ext.begin(), ext_bytes.back() );
                        ext_bytes.pop_back(  );
                    }
                    
                    for( size_t i = 0; i < functions.size(); i++ ) {
                        auto parameters = getFunctionParameters( i );
                        auto code = getFunctionCode( i );
                        
                        std::cout << "i = " << std::dec << i << std::endl;
                        std::cout << "faction = " << std::dec << functions.at( i ).faction << std::endl;
                        std::cout << "identifier = " << std::dec << functions.at( i ).identifier << std::endl;
                        std::cout << "start = " << std::dec << functions.at( i ).start_parameter_offset << "\n" << std::endl;
                        
                        std::cout << std::hex << "Parameters = ";
                        for( auto f = parameters.begin(); f < parameters.end(); f++ ) {
                            std::cout << "0x" << static_cast<unsigned>( (*f) ) << ", ";
                        }
                        for( auto f = parameters.begin(); f < parameters.end() - 1; f++ ) {
                            assert( (*f) != 0 );
                        }
                        std::cout << std::endl;
                        std::cout << std::hex << "Code = ";
                        for( auto f = code.begin(); f < code.end(); f++ ) {
                            std::cout << "0x" << static_cast<unsigned>( (*f) ) << ", ";
                        }
                        std::cout << std::dec << "\n" << std::endl;
                        if( i != functions.size() - 1 ) {
                            for( auto f = code.begin(); f < code.end() - 1; f++ ) {
                                assert( (*f) != 0 );
                            }
                        }
                        
                        assert( parameters.size() > 1 );
                        assert( code.size() > 1 );
                        assert( parameters.back() == 0 );
                        assert( code.back() == 0 );
                    }
                    /* std::cout << std::hex << "Last tEXT = ";
                    for( auto f = last_ext.begin(); f < last_ext.end(); f++ ){
                        std::cout << "0x" << static_cast<unsigned>( (*f) ) << ", ";
                    }
                    std::cout << std::dec << "\n" << std::endl; */
                    
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

int Data::Mission::FUNResource::write( const std::string& file_path, const std::vector<std::string> & arguments ) const {
    return 0;
}

std::vector<uint8_t> Data::Mission::FUNResource::getFunctionParameters( unsigned index ) const {
    const auto THE_FUNCTION = functions.at( index );
    const size_t PARAMETER_SIZE = THE_FUNCTION.start_code_offset - THE_FUNCTION.start_parameter_offset;
    
    std::vector<uint8_t> parameters;
    
    parameters.reserve( PARAMETER_SIZE );
    
    for( size_t i = THE_FUNCTION.start_parameter_offset; i < THE_FUNCTION.start_code_offset; i++ ) {
        parameters.push_back( ext_bytes.at( i ) );
    }
    
    return parameters;
}
std::vector<uint8_t> Data::Mission::FUNResource::getFunctionCode( unsigned index ) const {
    const auto THE_FUNCTION = functions.at( index );
    
    size_t code_size;
    
    if( functions.size() == ( index + 1 ) )
        code_size = ext_bytes.size() - THE_FUNCTION.start_code_offset;
    else
        code_size = functions.at( index + 1 ).start_parameter_offset - THE_FUNCTION.start_code_offset;
    
    std::vector<uint8_t> code;
    
    code.reserve( code_size );
    
    for( size_t i = THE_FUNCTION.start_code_offset; i < THE_FUNCTION.start_code_offset + code_size; i++ ) {
        code.push_back( ext_bytes.at( i ) );
    }
    
    return code;
}
