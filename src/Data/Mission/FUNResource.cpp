#include "FUNResource.h"
#include <limits>

const std::filesystem::path Data::Mission::FUNResource::FILE_EXTENSION = "fun";
// which is { 0x43, 0x66, 0x75, 0x6E } or { 'C', 'f', 'u', 'n' } or "Cfun"
const uint32_t Data::Mission::FUNResource::IDENTIFIER_TAG = 0x4366756e;

// TODO Do a further study on how that works.
const float Data::Mission::FUNResource::FUNCTION_TIME_UNITS_TO_SECONDS = 0.016424851;

uint32_t Data::Mission::FUNResource::getNumber(uint8_t *bytes_r, size_t &position) {
    uint32_t number = 0;

    while(*bytes_r != 0) {
        position++;

        if((*bytes_r & 0x80) == 0) {
            number |= *bytes_r;

            number = number << 7;
        }
        else {
            number |= (*bytes_r & 0x7F);
            return number;
        }

        bytes_r++;
    }

    return number;
}

Data::Mission::FUNResource::FUNResource() {
    spawn_all_neutral_turrets = false;
}

Data::Mission::FUNResource::FUNResource( const FUNResource &obj ) {
    spawn_all_neutral_turrets = false;
}

std::filesystem::path Data::Mission::FUNResource::getFileExtension() const {
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
    
    uint32_t un_data_id;
    Function fun_struct;
    
    if( this->data != nullptr )
    {
        auto reader = this->getDataReader();

        if( reader.totalSize() >= TAG_HEADER_SIZE ) {
            auto header    = reader.readU32( settings.endian );
            auto size      = reader.readU32( settings.endian );

            if( header == TAG_tFUN ) {
                auto reader_tfun = reader.getReader( size - TAG_HEADER_SIZE );
                
                un_data_id = reader_tfun.readU32( settings.endian );

                while( !reader_tfun.ended() ) {
                    fun_struct.how_many_times = reader_tfun.readI32( settings.endian );
                    fun_struct.time_units     = reader_tfun.readI32( settings.endian );

                    fun_struct.zero       = reader_tfun.readI32( settings.endian );
                    fun_struct.start_parameter_offset = reader_tfun.readU32( settings.endian );
                    fun_struct.start_code_offset      = reader_tfun.readU32( settings.endian );
                    
                    functions.push_back( fun_struct );
                }
                
                auto header    = reader.readU32( settings.endian );
                auto size      = reader.readU32( settings.endian );
                
                if( header == TAG_tEXT ) {
                    auto reader_ext = reader.getReader( size - TAG_HEADER_SIZE );
                    
                    un_data_id = reader_ext.readU32( settings.endian );
                    
                    ext_bytes = reader_ext.getBytes();
                    
                    for( size_t i = 0; i < functions.size(); i++ ) {
                        auto parameters = getFunctionParameters( i );
                        auto code       = getFunctionCode( i );

                        const static uint8_t p[] = {0x12, 0x80, 0x21};

                        if(parameters.size() == 5 && parameters[1] == p[0] && parameters[2] == p[1] && parameters[3] == p[2]) {
                            const uint8_t spawn_opcode[2] = {0xc7, 0x80};
                            const uint8_t spawn_actor = 0x3c;
                            const uint8_t spawn_neutral = 0x3d;
                            size_t position = 0;
                            uint8_t opcodes[3];
                            uint32_t number = 0;

                            while(code.at(position) != 0 && code.size() > position) {
                                number = getNumber(code.data() + position, position);
                                opcodes[0] = code.at(position);
                                if(opcodes[0] == 0)
                                    continue;
                                position++;
                                opcodes[1] = code.at(position);
                                if(opcodes[1] == 0)
                                    continue;
                                position++;
                                opcodes[2] = code.at(position);
                                if(opcodes[2] == 0)
                                    continue;
                                position++;

                                if(opcodes[0] == spawn_opcode[0] && opcodes[1] == spawn_opcode[1]) {
                                    if(opcodes[2] == spawn_neutral) {
                                        spawn_all_neutral_turrets = true;
                                    }
                                    else if(opcodes[2] == spawn_actor) {
                                        spawn_actors_now.push_back( number );
                                    }
                                }
                            }
                        }
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

int Data::Mission::FUNResource::write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options ) const {
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
