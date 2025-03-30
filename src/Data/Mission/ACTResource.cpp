#include "ACTResource.h"

#include "ACT/Internal/Hash.h"
#include "ACT/Unknown.h"

#include <fstream>

#include <json/json.h>

const std::filesystem::path Data::Mission::ACTResource::FILE_EXTENSION = "act";
const uint32_t Data::Mission::ACTResource::IDENTIFIER_TAG = 0x43616374; // which is { 0x43, 0x61, 0x63, 0x74 } or { 'C', 'a', 'c', 't' } or "Cact"
const uint32_t Data::Mission::ACTResource::SAC_IDENTI_TAG = 0x43736163; // which is { 0x43, 0x73, 0x61, 0x63 } or { 'C', s, 'a', 'c' } or "Csac"

const double Data::Mission::ACTResource::SECONDS_PER_GAME_TICK = 1.0 / 60.0;

const uint32_t Data::Mission::ACTResource::ACT_CHUNK_ID = 0x74414354; // which is { 0x74, 0x41, 0x43, 0x54 } or { 't', 'A', 'C', 'T' } or "tACT"
const uint32_t Data::Mission::ACTResource::RSL_CHUNK_ID = 0x6152534c; // which is { 0x61, 0x52, 0x53, 0x4c } or { 'a', 'R', 'S', 'L' } or "aRSL"
const uint32_t Data::Mission::ACTResource::SAC_CHUNK_ID = 0x74534143; // which is { 0x74, 0x53, 0x41, 0x43 } or { 't', 'S', 'A', 'C' } or "tSAC"


std::string Data::Mission::ACTResource::tSAC_chunk::getString() const {
    std::stringstream sac;

    if( !this->exists )
        sac << "DOES NOT EXIST\n";
    else {
        sac << "\n";
        sac << "game_ticks  = " << this->game_ticks  << "\n";
        sac << "spawn_limit = " << this->spawn_limit << "\n";
        sac << "unk_2       = " << this->unk_2       << "\n";
        sac << "unk_3       = " << this->unk_3       << "\n";
    }

    return sac.str();
}

Data::Mission::ACTResource::ACTResource() : matching_number( 0xFFFFF ), rsl_data(), tSAC( {false, 0, 1, 2, 3} ) {
}

Data::Mission::ACTResource::ACTResource( const ACTResource *const obj ) : Resource( *obj ), matching_number( obj->matching_number ), rsl_data( obj->rsl_data ), tSAC( obj->tSAC ) {
}

Data::Mission::ACTResource::~ACTResource() {
}

std::filesystem::path Data::Mission::ACTResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::ACTResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

std::filesystem::path Data::Mission::ACTResource::getFullName( unsigned int index ) const {
    if( tSAC.exists ) {
        std::filesystem::path full_name = Resource::getFullName( index );
        full_name += "_sac";
        return full_name;
    }
    return Resource::getFullName( index );

}

Json::Value Data::Mission::ACTResource::makeJson() const {
    const uint32_t NULL_INT = 0x4E554C4C; // This spells out NULL.
    uint8_t type[5] = {'\0'};
    Json::Value root;

    root["FutureCopAsset"]["type"] = "ACT Resource";
    root["FutureCopAsset"]["major"] = 2;
    root["FutureCopAsset"]["minor"] = 0;

    for( unsigned i = 0; i < rsl_data.size(); i++ )
    {
        // Convert from uint32_t to ascii string.
        type[ 0 ] = (rsl_data[ i ].type >> 24) & 0xFF;
        type[ 1 ] = (rsl_data[ i ].type >> 16) & 0xFF;
        type[ 2 ] = (rsl_data[ i ].type >>  8) & 0xFF;
        type[ 3 ] = (rsl_data[ i ].type >>  0) & 0xFF;

        root["RSL"][ i ]["type"]  = reinterpret_cast<char*>( type );

        if( rsl_data[ i ].type != NULL_INT && rsl_data[ i ].resource_id != 0x0 )
            root["RSL"][ i ]["resource_id"] = rsl_data[ i ].resource_id;
    }

    root["resource"]["id"] = static_cast<unsigned int>( this->matching_number );

    if( tSAC.exists ) {
        root["SAC"]["game_ticks"]  = tSAC.game_ticks;
        root["SAC"]["spawn_limit"] = tSAC.spawn_limit;
        root["SAC"]["unk_2"] = tSAC.unk_2;
        root["SAC"]["unk_3"] = tSAC.unk_3;
    }

    return root;
}

uint32_t Data::Mission::ACTResource::readACTChunk( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian, const ParseSettings &settings ) {
    auto debug_log = settings.logger_r->getLog( Utilities::Logger::DEBUG );
    debug_log.info << FILE_EXTENSION << ": " << getResourceID() << " ACT Chunk\n";
    auto error_log = settings.logger_r->getLog( Utilities::Logger::ERROR );
    error_log.info << FILE_EXTENSION << ": " << getResourceID() << " ACT Chunk\n";

    debug_log.output << "ACT_CHUNK_ID = 0x" << ACT_CHUNK_ID << std::endl;

    if( !data_reader.empty() && ACT_CHUNK_ID == data_reader.readU32( endian ) ) // 0
    {
        uint32_t chunk_size = data_reader.readU32( endian ); // 4

        debug_log.output << "chunk_size = " << chunk_size << std::endl;

        this->matching_number = data_reader.readU32( endian );  // 8

        debug_log.output << "matching_number = " << matching_number << std::endl;

        const uint32_t ACT_SIZE = chunk_size - 7 * sizeof( uint32_t );
        const uint_fast8_t act_type = data_reader.readU8(); // 12

        //data_reader.setPosition( 3, Utilities::Buffer::Reader::CURRENT );
        data_reader.readU8(); // 13
        data_reader.readU8(); // 14
        data_reader.readU8(); // 15
        
        // position_x and position_y is though to fixed point numbers.
        // The fixed point numbers are basically divided by 2^13 or 8192.
        // Since a Ctil is 131072x131072 and it has 16x16 tiles,
        // we can divide 131072/16 which gives 8192.
        // Since 8192 is equal to 2^13. We can treat these numbers as
        // fixed points. My engine for now will simply use floating
        // points, but position_y and position_x will be treated like this.
        position_y      = data_reader.readI32( endian ); // 16
        position_height = data_reader.readI32( endian ); // 20
        position_x      = data_reader.readI32( endian ); // 24
        
        auto reader_act = data_reader.getReader( ACT_SIZE );
        
        if( dynamic_cast<ACT::Unknown*>(this) == nullptr ) {
            debug_log.output << getTypeIDName() << "; Size: " << ACT_SIZE << ".\n";
        }
        
        if( !readACTType( act_type, reader_act, endian ) )
            error_log.output << getTypeIDName() << " ACT Type failed to parse. Size:" << ACT_SIZE << "\n";
        
        if( dynamic_cast<ACT::Unknown*>(this) != nullptr ) {
            debug_log.output << getTypeIDName() << "; Size: " << ACT_SIZE << ".\n";
        }
        
        return chunk_size;
    }
    else
        return 0;
}
uint32_t Data::Mission::ACTResource::readRSLChunk( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian, const ParseSettings &settings ) {
    auto warning_log = settings.logger_r->getLog( Utilities::Logger::WARNING );
    warning_log.info << FILE_EXTENSION << ": " << getResourceID() << " RSL Chunk\n";

    if( RSL_CHUNK_ID == data_reader.readU32( endian ) )
    {
        uint32_t chunk_size = data_reader.readU32( endian );

        uint32_t matching_number = data_reader.readU32( endian );

        if( this->matching_number != matching_number )
            warning_log.output << "this->matching number (" << std::dec << this->matching_number << ") is not equal to matching number (" << matching_number << ").\n";

        uint32_t rsl_data_size = chunk_size - sizeof( uint32_t ) * 3;
        uint32_t rsl_entry_amount = rsl_data_size / (sizeof( uint32_t ) * 2);

        rsl_data.reserve( rsl_entry_amount );

        for( uint32_t i = 0; i < rsl_entry_amount; i++ )
        {
            uint32_t type  = data_reader.readU32( endian );
            uint32_t resource_id = data_reader.readU32( endian );

            rsl_data.push_back( { type, resource_id } );
        }

        return chunk_size;
    }
    else
        return 0;
}

uint32_t Data::Mission::ACTResource::readSACChunk( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian, const ParseSettings &settings ) {
    auto warning_log = settings.logger_r->getLog( Utilities::Logger::WARNING );
    warning_log.info << FILE_EXTENSION << ": " << getResourceID() << " SAC Chunk\n";

    if( !data_reader.ended() && SAC_CHUNK_ID == data_reader.readU32( endian ) )
    {
        uint32_t chunk_size = data_reader.readU32( endian );

        uint32_t matching_number = data_reader.readU32( endian );

        if( this->matching_number != matching_number )
            warning_log.output << "this->matching number (" << std::dec << this->matching_number << ") is not equal to matching number (" << matching_number << ").\n";

        uint32_t sac_size  = chunk_size - sizeof( uint32_t ) * 3;

        if( sac_size != 0x24 )
            warning_log.output << "sac_size is (" << std::dec << sac_size << ") is not equal to " << 0x24 << ".\n";

        auto sac_reader = data_reader.getReader( sac_size );

        if( sac_size >= sizeof( uint16_t ) * 2 )
        {
            tSAC.exists = true;

            tSAC.game_ticks  = sac_reader.readI16( endian );
            tSAC.spawn_limit = sac_reader.readU16( endian );
            // TODO Figure out the rest from the raw data from the tSAC chunk.
            tSAC.unk_2 = sac_reader.readU16( endian );
            tSAC.unk_3 = sac_reader.readU16( endian );
        }

        // Somehow these bytes are NEVER used!
        // Do not ask me why this is because I do not know too.
        // Fun fact: since tSAC is always 0x30 bytes, and the 4 uint16's are useful that means that the overhead is 83.3 percent.
        // 0x08-0x24
        for( unsigned i = 0; i < 7; i++ ) {
            auto zero = sac_reader.readU32();

            if( zero != 0 ) {
                warning_log.output << "zero is actually 0x" << std::hex << zero << "\n.";

                i = 7;
            }
        }

        return chunk_size;
    }
    else
        return 0;
}


float Data::Mission::ACTResource::getRotation( int16_t rotation ) {
    return -glm::pi<float>() / 2048.0f * rotation;
}

glm::quat Data::Mission::ACTResource::getRotationQuaternion( float rotation ) {
    return glm::angleAxis( rotation, glm::vec3( 0.0f, 1.0f, 0.0f ) );
}

bool Data::Mission::ACTResource::parse( const ParseSettings &settings ) {
    auto error_log = settings.logger_r->getLog( Utilities::Logger::ERROR );
    error_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";
    auto warning_log = settings.logger_r->getLog( Utilities::Logger::WARNING );
    warning_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

    if( this->data != nullptr )
    {
        auto data_reader = this->getDataReader();

        if( data_reader.totalSize() == 0 ) {
            error_log.output << "data_reader.totalSize() should not be 0.\n";
            return false;
        }

        if( readACTChunk( data_reader, settings.endian, settings ) ) {
            if( readRSLChunk( data_reader, settings.endian, settings ) ) {

                if( !checkRSL() ) {
                    warning_log.output << "Display RSL:\n" << displayRSL() << "\n";
                }

                if( readSACChunk( data_reader, settings.endian, settings ) ) {
                    return true;
                }
                else
                    return false;
            }
            else
                return false;
        }
        else
            return false;
    }
    else
        return false;
}

int Data::Mission::ACTResource::write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    std::ofstream resource;

    if( iff_options.act.shouldWrite( iff_options.enable_global_dry_default ) ) {
        std::filesystem::path full_file_path = file_path;
        full_file_path += ".json";

        resource.open( full_file_path, std::ios::out );

        if( resource.is_open() )
        {
            resource << makeJson();

            resource.close();

            return 0;
        }
        else
            return 1;
    }
    else
        return 1;
}

Data::Mission::Resource* Data::Mission::ACTResource::genResourceByType( const Utilities::Buffer::Reader &data ) const {
    auto read_data = Utilities::Buffer::Reader(data);

    read_data.setPosition( 3 * sizeof( uint32_t ) );

    uint_fast8_t type_id;

    // Check to see if the read_data is valid.
    if( !read_data.empty() ) {
        type_id = read_data.readU8();

        if( Data::Mission::ACT::Hash::isActValid( type_id ) )
            return Data::Mission::ACT::Hash::generateAct( this, type_id );
    }
    return new Data::Mission::ACT::Unknown( *this );
}

bool Data::Mission::ACTResource::hasRSL( uint32_t type_id, uint32_t resource_id ) const {
    for( auto i = rsl_data.begin(); i != rsl_data.end(); i++ ) {
        if( (*i).type == type_id && (*i).resource_id == resource_id )
            return true;
    }
    return false;
}

std::string Data::Mission::ACTResource::displayRSL() const {
    std::string rsl_text = "";
    
    for( auto i = rsl_data.begin(); i != rsl_data.end(); i++ ) {
        rsl_text += "RSL[" + std::to_string( i - rsl_data.begin() ) + "] = {Type:";
        
        rsl_text += static_cast<char>( ((*i).type >> 24) & 0xFF );
        rsl_text += static_cast<char>( ((*i).type >> 16) & 0xFF );
        rsl_text += static_cast<char>( ((*i).type >>  8) & 0xFF );
        rsl_text += static_cast<char>( ((*i).type >>  0) & 0xFF );
        
        rsl_text += ", Resource ID: " + std::to_string((*i).resource_id);
        
        rsl_text += "};\n";
    }
    
    return rsl_text;
}

glm::vec2 Data::Mission::ACTResource::getPosition() const {
    return (1.f / 8192.f) * glm::vec2( position_x, position_y );
}

glm::vec3 Data::Mission::ACTResource::getPosition( const PTCResource &ptc, GroundCast ground_cast ) const {
    const auto v = this->getPosition();
    return glm::vec3( v.x, ptc.getRayCast2D( v.x, v.y, getGroundCastLevels(ground_cast) ), v.y );
}

bool Data::Mission::IFFOptions::ACTOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {
    return IFFOptions::ResourceOption::readParams( arguments, output_r );
}

std::string Data::Mission::IFFOptions::ACTOption::getOptions() const {
    std::string information_text = getBuiltInOptions();

    return information_text;

}
