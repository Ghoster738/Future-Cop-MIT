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

                while( !reader_tfun.ended() ) {
                    fun_struct.faction    = reader_tfun.readI32( settings.endian );
                    fun_struct.identifier = reader_tfun.readI32( settings.endian );

                    fun_struct.zero       = reader_tfun.readI32( settings.endian );
                    fun_struct.start_parameter_offset = reader_tfun.readU32( settings.endian );
                    fun_struct.start_code_offset      = reader_tfun.readU32( settings.endian );
                    
                    functions.push_back( fun_struct );
                }
                
                auto header    = reader.readU32( settings.endian );
                auto size      = reader.readU32( settings.endian );
                
                if( header == TAG_tEXT ) {
                    auto reader_ext = reader.getReader( size - TAG_HEADER_SIZE );
                    
                    data_id = reader_ext.readU32( settings.endian );
                    
                    ext_bytes = reader_ext.getBytes();

                    auto error_log = settings.logger_r->getLog( Utilities::Logger::ERROR );
                    error_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";
                    
                    for( size_t i = 0; i < functions.size(); i++ ) {
                        auto parameters = getFunctionParameters( i );
                        auto code       = getFunctionCode( i );
                        
                        // Detect neutral turret spawn.
                        {
                            uint8_t p[] = { 0x86, 0x12, 0x80, 0x21, 0x00 };
                            uint8_t param_2 = 0x88;
                            uint8_t c[] = { 0xb2, 0xc7, 0x80, 0x3d, 0x00 };

                            if( parameters.size() == 5 && (parameters[0] == p[0] || parameters[0] == param_2) && parameters[1] == p[1] && parameters[2] == p[2] && parameters[3] == p[3] ) {
                                error_log.output << "Function Index = " << i << "\n";
                                error_log.output << "  Faction = " << functions[ i ].faction << "\n";
                                error_log.output << "  Identifier = " << functions[ i ].identifier << "\n";
                                error_log.output << "  Parameter Offset = 0x" << std::hex << functions[ i ].start_parameter_offset << "\n";
                                error_log.output << "  Code Offset = 0x" << functions[ i ].start_code_offset << std::dec << "\n";

                                error_log.output << "Neutral Parameters!" << std::hex;
                                for( auto param = parameters.begin(); param != parameters.end(); param++ ) {
                                    error_log.output << " 0x" << (unsigned)(*param);
                                }
                                error_log.output << "\n" << std::dec;

                                if( code.size() >= 5 && code[0] == c[0] && code[1] == c[1] && code[2] == c[2] && code[3] == c[3] ) {
                                    error_log.output << "Command: NeutralInitAll()";
                                }

                                error_log.output << std::hex;
                                for( auto param = code.begin(); param != code.end(); param++ ) {
                                    error_log.output << " 0x" << (unsigned)(*param);
                                }
                                error_log.output << "\n\n" << std::dec;
                            }
                        }

                        {
                            uint8_t p[] = { 0x86, 0x12, 0x80, 0x21, 0x00 };
                            uint8_t c[] = { 0xC7, 0x80, 0x3C };

                            if( parameters.size() == 5 && parameters[1] == p[1] && parameters[2] == p[2] && parameters[3] == p[3] &&
                                code.size() > 4 && code[1] == c[0] && code[2] == c[1] && code[3] == c[2] ) {
                                error_log.output << "Function Index = " << i << "\n";
                                error_log.output << "  Faction = " << functions[ i ].faction << "\n";
                                error_log.output << "  Identifier = " << functions[ i ].identifier << "\n";
                                error_log.output << "  Parameter Offset = 0x" << std::hex << functions[ i ].start_parameter_offset << "\n";
                                error_log.output << "  Code Offset = 0x" << functions[ i ].start_code_offset << std::dec << "\n";

                                error_log.output << "Neutral Parameters!" << std::hex;
                                for( auto param = parameters.begin(); param != parameters.end(); param++ ) {
                                    error_log.output << " 0x" << (unsigned)(*param);
                                }
                                error_log.output << "\n" << std::dec;

                                error_log.output << "Command: Init an Actor( id )";
                                error_log.output << std::hex;
                                for( auto param = code.begin(); param != code.end(); param++ ) {
                                    error_log.output << " 0x" << (unsigned)(*param);
                                }
                                error_log.output << "\n\n" << std::dec;

                            }
                        }

                        // faction = 1, identifier = 5 Probably means initialization!
                        // FORCE_ACTOR_SPAWN = NUMBER, { 0xC7, 0x80, 0x3C }
                        // NEUTRAL_TURRET_INIT = NUMBER, { 0xC7, 0x80, 0x3D }

                        // JOKE/SLIM has faction 1 and identifier 5, and it appears to be something else. I can deduce no pattern in this sequence.
                        // However, I have enough knowedge to write an inaccurate parser.
                    }

                    
                    return true;
                }
            }
        }
    }

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
