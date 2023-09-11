#include "FUNResource.h"
#include <limits>

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
    // const size_t NUM_ENTRIES_SIZE = sizeof(uint32_t);
    // const size_t       ENTRY_SIZE = 8 * sizeof(uint8_t);
    const uint32_t TAG_tFUN = 0x7446554e;
    // which is { 0x74, 0x46, 0x55, 0x4e } or { 't', 'F', 'U', 'N' } or "tFUN"
    const uint32_t TAG_tEXT = 0x74455854;
    // which is { 0x74, 0x45, 0x58, 0x54 } or { 't', 'E', 'X', 'T' } or "tEXT"

    auto debug_log = settings.logger_r->getLog( Utilities::Logger::DEBUG );
    debug_log.output << FILE_EXTENSION << ": " << getResourceID() << "\n";
    
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
                if( data_id != 1 )
                    debug_log.output << "Difference: data_id is " << data_id << ".\n";
                
                while( !reader_tfun.ended() ) {
                    fun_struct.faction    = reader_tfun.readI32( settings.endian );
                    fun_struct.identifier = reader_tfun.readI32( settings.endian );

                    fun_struct.zero       = reader_tfun.readI32( settings.endian );
                    fun_struct.start_parameter_offset = reader_tfun.readU32( settings.endian );
                    fun_struct.start_code_offset      = reader_tfun.readU32( settings.endian );
                    
                    if( !(( fun_struct.faction == 1 ) | ( fun_struct.faction == -1 ) | (fun_struct.faction == 0 ) | ( fun_struct.faction == 25 ) | ( fun_struct.faction == 9999 )) ) {
                        debug_log.output << "Difference: fun_struct.fraction is " << fun_struct.faction << ".\n";
                    }
                    if( fun_struct.zero != 0 ) {
                        debug_log.output << "Difference: fun_struct.zero is " << fun_struct.zero << " not zero.\n";
                    }
                    if( fun_struct.start_parameter_offset >= fun_struct.start_code_offset ) {
                        debug_log.output << "Difference: fun_struct.start_parameter_offset (" << fun_struct.start_parameter_offset << ") < fun_struct.start_code_offset (" << fun_struct.start_code_offset << ").\n";
                    }
                    
                    functions.push_back( fun_struct );
                }
                
                if( functions.size() == 0 ) {
                    debug_log.output << "Difference: functions.size() is " << functions.size() << ".\n";
                }
                
                auto header    = reader.readU32( settings.endian );
                auto size      = reader.readU32( settings.endian );
                
                if( header == TAG_tEXT ) {
                    auto reader_ext = reader.getReader( size - TAG_HEADER_SIZE );
                    
                    data_id = reader_ext.readU32( settings.endian );
                    if( data_id != 1 )
                        debug_log.output << "Difference: data_id is " << data_id << ".\n";
                    
                    ext_bytes = reader_ext.getBytes();
                    
                    if( ext_bytes.size() == 0 )
                        debug_log.output << "Difference: ext_bytes.size() is " << ext_bytes.size() << ".\n";
                    if( ext_bytes.size() <= functions.back().start_code_offset )
                        debug_log.output << "Difference: ext_bytes.size() (" << ext_bytes.size() << ") <= functions.back().start_code_offset (" << functions.back().start_code_offset << ").\n";
                    if( !reader.ended() )
                        debug_log.output << "Difference: Reader is not done.\n";
                    
                    while( ext_bytes.back() != 0 ){
                        last_ext.insert( last_ext.begin(), ext_bytes.back() );
                        ext_bytes.pop_back(  );
                    }
                    
                    for( size_t i = 0; i < functions.size(); i++ ) {
                        auto parameters = getFunctionParameters( i );
                        auto code = getFunctionCode( i );
                        
                        debug_log.output << "i[" << std::dec << i  << "], ";
                        debug_log.output << "f[" << functions.at( i ).faction << "], ";
                        debug_log.output << "id[" << functions.at( i ).identifier << "], ";
                        debug_log.output << "offset = " << functions.at( i ).start_parameter_offset << "\n" << std::endl;

                        debug_log.output << std::hex << "Parameters = ";
                        for( auto f = parameters.begin(); f < parameters.end(); f++ ) {
                            debug_log.output << "0x" << static_cast<unsigned>( (*f) ) << ", ";
                        }
                        if( !parameters.empty() ) {
                            for( auto f = parameters.begin(); f < parameters.end() - 1; f++ ) {
                                if( (*f) == 0 ) {
                                    debug_log.output << "Difference: (*f) == 0.\n";
                                }
                            }
                        }
                        debug_log.output << std::endl;
                        debug_log.output << std::hex << "Code = ";
                        for( auto f = code.begin(); f < code.end(); f++ ) {
                            debug_log.output << "0x" << static_cast<unsigned>( (*f) ) << ", ";
                        }
                        debug_log.output << std::dec << "\n";
                        
                        // faction = 1, identifier = 5 Probably means initialization!
                        // FORCE_ACTOR_SPAWN = NUMBER, { 0xC7, 0x80, 0x3C }
                        // NEUTRAL_TURRET_INIT = NUMBER, { 0xC7, 0x80, 0x3D }
                        
                        // JOKE/SLIM has faction 1 and identifier 5, and it appears to be something else. I can deduce no pattern in this sequence.
                        // However, I have enough knowedge to write an inaccurate parser.
                        
                        if( parameters.size() <= 1 ) {
                            debug_log.output << "Difference: parameters.size() > 1 false. parameters.size() = " << parameters.size() << "\n";
                        }
                        if( code.size() <= 1 ) {
                            debug_log.output << "Difference: code.size() > 1 false. code.size() = " << code.size() << "\n";
                        }
                        if( !parameters.empty() ) {
                        if( parameters.back() != 0 ) {
                            debug_log.output << "Difference: parameters.back() = " << parameters.back() << ". It is not zero.\n";
                        }
                        }
                        if( !code.empty() ) {
                        if( code.back() != 0 ) {
                            debug_log.output << "Difference: code.back() = " << code.back() << ". It is not zero.\n";
                        }
                        }
                    }
                    debug_log.output << std::hex << "Last tEXT = ";
                    for( auto f = last_ext.begin(); f < last_ext.end(); f++ ){
                        debug_log.output << "0x" << static_cast<unsigned>( (*f) ) << ", ";
                    }
                    debug_log.output << "\n";
                    
                    return true;
                }
            }
        }
    }
    
    debug_log.output << "parsing had failed.\n";

    return false;
}

Data::Mission::Resource * Data::Mission::FUNResource::duplicate() const {
    return new FUNResource( *this );
}

int Data::Mission::FUNResource::write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options ) const {
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
    
    for( size_t i = THE_FUNCTION.start_code_offset; i < THE_FUNCTION.start_code_offset + code_size && i < ext_bytes.size(); i++ ) {
        code.push_back( ext_bytes.at( i ) );
    }
    
    return code;
}

bool Data::Mission::IFFOptions::FUNOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {
    return IFFOptions::ResourceOption::readParams( arguments, output_r );
}

std::string Data::Mission::IFFOptions::FUNOption::getOptions() const {
    std::string information_text = getBuiltInOptions();

    return information_text;
}
