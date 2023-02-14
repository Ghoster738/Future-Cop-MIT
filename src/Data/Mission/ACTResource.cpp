#include "ACTResource.h"

#include "../../Utilities/DataHandler.h"
#include "ACT/Internal/Hash.h"
#include "ACT/Unknown.h"

#include <fstream>
#include <iostream>
#include <cassert>

#include <json/json.h>

const std::string Data::Mission::ACTResource::FILE_EXTENSION = "act";
const uint32_t Data::Mission::ACTResource::IDENTIFIER_TAG = 0x43616374; // which is { 0x43, 0x61, 0x63, 0x74 } or { 'C', 'a', 'c', 't' } or "Cact"
const uint32_t Data::Mission::ACTResource::SAC_IDENTI_TAG = 0x43736163; // which is { 0x43, 0x73, 0x61, 0x63 } or { 'C', s, 'a', 'c' } or "Csac"

const double Data::Mission::ACTResource::SECONDS_PER_GAME_TICK = 1.0 / 60.0;

const uint32_t Data::Mission::ACTResource::ACT_CHUNK_ID = 0x74414354; // which is { 0x74, 0x41, 0x43, 0x54 } or { 't', 'A', 'C', 'T' } or "tACT"
const uint32_t Data::Mission::ACTResource::RSL_CHUNK_ID = 0x6152534c; // which is { 0x61, 0x52, 0x53, 0x4c } or { 'a', 'R', 'S', 'L' } or "aRSL"
const uint32_t Data::Mission::ACTResource::SAC_CHUNK_ID = 0x74534143; // which is { 0x74, 0x53, 0x41, 0x43 } or { 't', 'S', 'A', 'C' } or "tSAC"

Data::Mission::ACTResource::ACTResource() : tSAC( {false, 0, 1, 2, 3} ), matching_number( 0xFFFFF ), rsl_data() {
}

Data::Mission::ACTResource::ACTResource( const ACTResource *const obj ) : Resource( *obj ), matching_number( obj->matching_number ), rsl_data( obj->rsl_data ), tSAC( obj->tSAC ) {
}

Data::Mission::ACTResource::~ACTResource() {
}

std::string Data::Mission::ACTResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::ACTResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

std::string Data::Mission::ACTResource::getFullName( unsigned int index ) const {
    if( tSAC.exists )
        return Resource::getFullName( index ) + "_sac";
    else
        return Resource::getFullName( index );

}

Json::Value Data::Mission::ACTResource::makeJson() const {
    const uint32_t NULL_INT = 0x4E554C4C; // This spells out NULL.
    uint8_t type[5] = {'\0'};
    Json::Value root;

    root["FutureCopAsset"]["type"] = "ACT Resource";
    root["FutureCopAsset"]["major"] = 1;
    root["FutureCopAsset"]["minor"] = 0;

    for( unsigned i = 0; i < rsl_data.size(); i++ )
    {
        // Convert from uint32_t to ascii string.
        type[ 0 ] = (rsl_data[ i ].type >> 24) & 0xFF;
        type[ 1 ] = (rsl_data[ i ].type >> 16) & 0xFF;
        type[ 2 ] = (rsl_data[ i ].type >>  8) & 0xFF;
        type[ 3 ] = (rsl_data[ i ].type >>  0) & 0xFF;

        root["RSL"][ i ]["type"]  = reinterpret_cast<char*>( type );

        if( rsl_data[ i ].type != NULL_INT && rsl_data[ i ].index != 0x0 )
            root["RSL"][ i ]["index"] = rsl_data[ i ].index;
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

uint32_t Data::Mission::ACTResource::readACTChunk( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    // std::cout << std::hex;

    // std::cout << "ACT_CHUNK_ID = " << ACT_CHUNK_ID << std::endl;

    if( !data_reader.empty() && ACT_CHUNK_ID == data_reader.readU32( endian ) )
    {
        uint32_t chunk_size = data_reader.readU32( endian );

        // std::cout << "chunk_size = " << chunk_size << std::endl;

        this->matching_number = data_reader.readU32( endian );

        // std::cout << "matching_number = " << matching_number << std::endl;

        const uint32_t ACT_SIZE = chunk_size - sizeof( uint32_t ) * 7;
        const uint_fast8_t act_type = data_reader.readU8();
        
        // std::cout << std::dec;

        //data_reader.setPosition( 3, Utilities::Buffer::Reader::CURRENT );
        data_reader.readU8();
        data_reader.readU8();
        data_reader.readU8();
        
        // position_x and position_y is though to fixed point numbers.
        // The fixed point numbers are basically divided by 2^13 or 8192.
        // Since a Ctil is 131072x131072 and it has 16x16 tiles,
        // we can divide 131072/16 which gives 8192.
        // Since 8192 is equal to 2^13. We can treat these numbers as
        // fixed points. My engine for now will simply use floating
        // points, but position_y and position_x will be treated like this.
        position_y      = data_reader.readI32();
        position_height = data_reader.readI32();
        position_x      = data_reader.readI32();
        
        if( getTypeID() == 8 )
            std::cout << getTypeIDName() << " Resource ID: " << getResourceID() << std::endl;
        
        auto reader_act = data_reader.getReader( ACT_SIZE );
        bool processed = readACTType( act_type, reader_act, endian );
        
        /*if( this->getTypeID() == 1 ) {
            std::cout << "position_x = " << static_cast<float>( position_x ) / 8192.0f << std::endl;
            std::cout << "position_y = " << static_cast<float>( position_y ) / 8192.0f << std::endl;
        }*/
        
        return chunk_size;
    }
    else
        return 0;
}
uint32_t Data::Mission::ACTResource::readRSLChunk( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    // const uint32_t Cobj_INT = 0x436F626A; // This spells out Cobj.

    if( RSL_CHUNK_ID == data_reader.readU32( endian ) )
    {
        uint32_t chunk_size = data_reader.readU32( endian );

        uint32_t matching_number = data_reader.readU32( endian );

        assert( this->matching_number == matching_number );

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

uint32_t Data::Mission::ACTResource::readSACChunk( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    if( !data_reader.ended() && SAC_CHUNK_ID == data_reader.readU32( endian ) )
    {
        uint32_t chunk_size = data_reader.readU32( endian );

        uint32_t matching_number = data_reader.readU32( endian );

        assert( this->matching_number == matching_number );

        // TODO Find out how to read the raw data from the tSAC chunk.
        uint32_t sac_size  = chunk_size - sizeof( uint32_t ) * 3;

        assert( sac_size == 0x24 );

        auto sac_reader = data_reader.getReader( sac_size );

        if( sac_size >= sizeof( uint16_t ) * 2 )
        {
            tSAC.exists = true;

            tSAC.game_ticks  = sac_reader.readI16( endian );
            tSAC.spawn_limit = sac_reader.readU16( endian );
            tSAC.unk_2 = sac_reader.readU16( endian );
            tSAC.unk_3 = sac_reader.readU16( endian );
        }

        // Somehow these bytes are NEVER used!
        // Do not ask me why this is because I do not know too.
        // Fun fact: since tSAC is always 0x30 bytes, and the 4 uint16's are useful that means that the overhead is 83.3 percent.
        assert( sac_reader.readU64() == 0 ); // 0x08-0x10
        assert( sac_reader.readU64() == 0 ); // 0x10-0x18
        assert( sac_reader.readU64() == 0 ); // 0x18-0x20
        assert( sac_reader.readU32() == 0 ); // 0x20-0x24

        return chunk_size;
    }
    else
        return 0;
}

bool Data::Mission::ACTResource::parse( const ParseSettings &settings ) {
    if( this->data_p != nullptr )
    {
        auto data_reader = this->data_p->getReader();

        assert( data_reader.totalSize() != 0 );

        if( readACTChunk( data_reader, settings.endian ) ) {
            if( readRSLChunk( data_reader, settings.endian ) ) {
                if( readSACChunk( data_reader, settings.endian ) ) {
                    assert( checkRSL() );
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

int Data::Mission::ACTResource::write(  const std::string& file_path, const std::vector<std::string> & arguments ) const {
    std::ofstream resource;
    bool enable_export = true;

    for( auto arg = arguments.begin(); arg != arguments.end(); arg++ ) {
        if( (*arg).compare("--dry") == 0 )
            enable_export = false;
    }

    if( enable_export )
    {
        resource.open( file_path + ".json", std::ios::out );

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

Data::Mission::Resource* Data::Mission::ACTResource::genResourceByType( const Utilities::Buffer &header, const Utilities::Buffer &data ) const {
    auto read_data = data.getReader( 3 * sizeof( uint32_t ), 1 );
    uint_fast8_t type_id;

    // Check to see if the read_data is valid.
    if( !read_data.empty() ) {
        type_id = read_data.readU8();

        if( Data::Mission::ACT::Hash::isActValid( type_id ) )
            return Data::Mission::ACT::Hash::generateAct( this, type_id );
    }
    return new Data::Mission::ACT::Unknown( *this );
}

std::vector<Data::Mission::ACTResource*> Data::Mission::ACTResource::getVector( Data::Mission::IFF &mission_file ) {
    std::vector<Resource*> to_copy = mission_file.getResources( Data::Mission::ACTResource::IDENTIFIER_TAG );

    std::vector<ACTResource*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<ACTResource*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::ACTResource*> Data::Mission::ACTResource::getVector( const Data::Mission::IFF &mission_file ) {
    return Data::Mission::ACTResource::getVector( const_cast< Data::Mission::IFF& >( mission_file ) );
}
