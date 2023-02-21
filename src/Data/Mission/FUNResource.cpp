#include "FUNResource.h"
#include <limits>
#include <cassert>
#include <iostream>

const std::string Data::Mission::FUNResource::FILE_EXTENSION = "fun";
// which is { 0x43, 0x66, 0x75, 0x6E } or { 'C', 'f', 'u', 'n' } or "Cfun"
const uint32_t Data::Mission::FUNResource::IDENTIFIER_TAG = 0x4366756e;

namespace {
const uint8_t FORCE_ACTOR_SPAWN[] = { 0xC7, 0x80, 0x3C };
const uint8_t NEUTRAL_TURRET_INIT[] = { 0xC7, 0x80, 0x3D }; // Always has 0xB2 or 178. Which might mean a value of 50 if converted.
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
                        
                        if( settings.output_level >= 3 ) {
                            *settings.output_ref << "i = " << std::dec << i << std::endl;
                            *settings.output_ref << "faction = " << std::dec << functions.at( i ).faction << std::endl;
                            *settings.output_ref << "identifier = " << std::dec << functions.at( i ).identifier << std::endl;
                            *settings.output_ref << "start = " << std::dec << functions.at( i ).start_parameter_offset << "\n" << std::endl;
                            
                            *settings.output_ref << std::hex << "Parameters = ";
                            for( auto f = parameters.begin(); f < parameters.end(); f++ ) {
                                *settings.output_ref << "0x" << static_cast<unsigned>( (*f) ) << ", ";
                            }
                            for( auto f = parameters.begin(); f < parameters.end() - 1; f++ ) {
                                assert( (*f) != 0 );
                            }
                            *settings.output_ref << std::endl;
                            *settings.output_ref << std::hex << "Code = ";
                            for( auto f = code.begin(); f < code.end(); f++ ) {
                                *settings.output_ref<< "0x" << static_cast<unsigned>( (*f) ) << ", ";
                            }
                            *settings.output_ref << std::dec << "\n" << std::endl;
                        }
                        
                        bool found_item = false;
                        
                        if( code.size() >= 5) {
                            const size_t ELEMENT = code.at( (code.end() - code.begin()) - 2 );
                            
                            if( ELEMENT == NEUTRAL_TURRET_INIT[2] ) {
                                if( code.at( (code.end() - code.begin()) - 3 ) == NEUTRAL_TURRET_INIT[1] && code.at( (code.end() - code.begin()) - 4 ) == NEUTRAL_TURRET_INIT[0] ) {
                                    std::cout << "Found Neutral Turret!" << std::endl;
                                    found_item = true;
                                }
                            }
                            else
                            if( ELEMENT == FORCE_ACTOR_SPAWN[2] ) {
                                if( code.at( (code.end() - code.begin()) - 3 ) == FORCE_ACTOR_SPAWN[1] && code.at( (code.end() - code.begin()) - 4 ) == FORCE_ACTOR_SPAWN[0] ) {
                                    std::cout << "Found Base Turret!" << std::endl;
                                    found_item = true;
                                }
                            }
                        }
                        
                        if( found_item ) {
                            *settings.output_ref << "i = " << std::dec << i << std::endl;
                            *settings.output_ref << "faction = " << std::dec << functions.at( i ).faction << std::endl;
                            *settings.output_ref << "identifier = " << std::dec << functions.at( i ).identifier << std::endl;
                            *settings.output_ref << "start = " << std::dec << functions.at( i ).start_parameter_offset << "\n" << std::endl;
                            *settings.output_ref << std::hex << "Code = ";
                            for( auto f = code.begin(); f < code.end(); f++ ) {
                                *settings.output_ref<< "0x" << static_cast<unsigned>( (*f) ) << ", ";
                            }
                            std::cout << std::endl;
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
