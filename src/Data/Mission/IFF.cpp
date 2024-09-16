#include "IFF.h"

#include "../Accessor.h"

#include "AIFFResource.h"
#include "ANMResource.h"
#include "ACTResource.h"
#include "BMPResource.h"
#include "DCSResource.h"
#include "FontResource.h"
#include "FUNResource.h"
#include "MSICResource.h"
#include "NetResource.h"
#include "PTCResource.h"
#include "PYRResource.h"
#include "TilResource.h"
#include "TOSResource.h"
#include "SNDSResource.h"
#include "SHDResource.h"
#include "WAVResource.h"
#include "ObjResource.h"
#include "UnkResource.h"
#include "RPNSResource.h"

#include "ACT/Unknown.h"
#include "ACT/SkyCaptain.h"

#include "../../Utilities/Buffer.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <unordered_set>

namespace {
    // These are the first things that appears in the header.
    const uint32_t IN_ENDIAN_CTRL_TAG = 0x4354524C; // which is { 0x43, 0x54, 0x52, 0x4C } or { 'C', 'T', 'R', 'L' } or "CTRL"
    const uint32_t OP_ENDIAN_CTRL_TAG = 0x4C525443; // which is { 0x4C, 0x52, 0x54, 0x43 } or { 'L', 'R', 'T', 'C' } or "LRTC"
    const char  MAC_CTRL_TAG[ 4 ] = { 'C', 'T', 'R', 'L' };
    const char  WIN_CTRL_TAG[ 4 ] = { 'L', 'R', 'T', 'C' }; // This tag is also found on Playstation files.

    // These are the mission file tags
    // which is { 0x53, 0x59, 0x4E, 0x43 } or { 'S', 'Y', 'N', 'C' } or "SYNC"
    const uint32_t SYNC_TAG = 0x53594E43; // This might not be a tag. +4 bytes.
    // which is { 0x53, 0x48, 0x4F, 0x43 } or { 'S', 'H', 'O', 'C' } or "SHOC"
    const uint32_t SHOC_TAG = 0x53484F43; // +12 bytes
    // which is { 0x53, 0x48, 0x44, 0x52 } or { 'S', 'H', 'D', 'R' } or "SHDR"
    const uint32_t SHDR_TAG = 0x53484452; // +4 bytes
    // which is { 0x46, 0x49, 0x4C, 0x4C } or { 'F', 'I', 'L', 'L' } or "FILL"
    const uint32_t FILL_TAG = 0x46494C4C; // Dynamic size
    // which is { 0x53, 0x44, 0x41, 0x54 } or { 'S', 'D', 'A', 'T' } or "SDAT"
    const uint32_t SDAT_TAG = 0x53444154; // Dynamic size
    // which is { 0x4D, 0x53, 0x49, 0x43 } or { 'M', 'S', 'I', 'C' } or "MSIC"
    const uint32_t MSIC_TAG = 0x4D534943; // Dynamic size
    // which is { 0x53, 0x57, 0x56, 0x52 } or { 'S', 'W', 'V', 'R' } or "SWVR"
    const uint32_t SWVR_TAG = 0x53575652; // +32 bytes
    // which is { 0x43, 0x41, 0x4E, 0x4D } or { 'C', 'A', 'N', 'M' } or "CANM"
    const uint32_t PS1_CANM_TAG = 0x43414E4D; // Dynamic size
    // which is { 0x56, 0x41, 0x47, 0x42 } or { 'V', 'A', 'G', 'B' } or "VAGB"
    const uint32_t PS1_VAGB_TAG = 0x56414742; // Dynamic size
    // which is { 0x56, 0x41, 0x47, 0x4D } or { 'V', 'A', 'G', 'M' } or "VAGM"
    const uint32_t PS1_VAGM_TAG = 0x5641474D; // Dynamic size

    const std::map<uint32_t, Data::Mission::Resource*> file_type_list {
        { Data::Mission::ACTResource::IDENTIFIER_TAG, new Data::Mission::ACT::Unknown() },
        { Data::Mission::ANMResource::IDENTIFIER_TAG, new Data::Mission::ANMResource() },
        { Data::Mission::BMPResource::IDENTIFIER_TAG, new Data::Mission::BMPResource() },
        // which is { 0x43, 0x63, 0x74, 0x72 } or { 'C', 'c', 't', 'r' } or "Cctr"
        { 0x43637472, new Data::Mission::UnkResource( 0x43637472, "ctr" ) },
        { Data::Mission::DCSResource::IDENTIFIER_TAG,  new Data::Mission::DCSResource() },
        { Data::Mission::FontResource::IDENTIFIER_TAG, new Data::Mission::FontResource() },
        { Data::Mission::NetResource::IDENTIFIER_TAG,  new Data::Mission::NetResource() },
        { Data::Mission::ObjResource::IDENTIFIER_TAG,  new Data::Mission::ObjResource() },
        { Data::Mission::PTCResource::IDENTIFIER_TAG,  new Data::Mission::PTCResource() },
        { Data::Mission::PYRResource::IDENTIFIER_TAG,  new Data::Mission::PYRResource() },
        { Data::Mission::ACTResource::SAC_IDENTI_TAG,  new Data::Mission::ACT::Unknown() },
        // which is { 0x43, 0x73, 0x66, 0x78 } or { 'C', 's', 'f', 'x' } or "Csfx"
        { 0x43736678, new Data::Mission::UnkResource( 0x43736678, "sfx", true) },  // Resource ID missing. Holds Act based structures?
        { Data::Mission::SHDResource::IDENTIFIER_TAG, new Data::Mission::SHDResource() }, // Resource ID missing.
        { Data::Mission::TilResource::IDENTIFIER_TAG, new Data::Mission::TilResource() },
        { Data::Mission::TOSResource::IDENTIFIER_TAG, new Data::Mission::TOSResource() },
        { Data::Mission::WAVResource::IDENTIFIER_TAG, new Data::Mission::WAVResource() },
        { Data::Mission::AIFFResource::IDENTIFIER_TAG, new Data::Mission::AIFFResource() },
        { Data::Mission::RPNSResource::IDENTIFIER_TAG, new Data::Mission::RPNSResource() },
        { Data::Mission::SNDSResource::IDENTIFIER_TAG, new Data::Mission::SNDSResource() },
        { Data::Mission::FUNResource::IDENTIFIER_TAG, new Data::Mission::FUNResource() },
        { 0x43766b68, new Data::Mission::UnkResource( 0x43766b68, "vkh" ) }, // PS1 Missions.
        { 0x43766b62, new Data::Mission::UnkResource( 0x43766b68, "vkb" ) },
        { 0x43747273, new Data::Mission::UnkResource( 0x43747273, "trs" ) }, // PS1 Global.
        { 0x436d6463, new Data::Mission::UnkResource( 0x436d6463, "mdc" ) },
        { 0x4374696e, new Data::Mission::UnkResource( 0x4374696e, "tin" ) },
        { 0x43746474, new Data::Mission::UnkResource( 0x43746474, "tdt" ) },
        { 0x436d6963, new Data::Mission::UnkResource( 0x436d6963, "mic" ) }
    };

    std::map<uint32_t, uint32_t> pc_header_enum_numbers = {
        {0x43616374, 3}, // Cact
        {0x43616966, 0}, // Caif
        {0x43626d70, 2}, // Cbmp
        {0x43637472, 1}, // Cctr
        {0x43646373, 2}, // Cdcs
        {0x43666e74, 2}, // Cfnt
        {0x4366756e, 2}, // Cfun
        {0x436e6574, 2}, // Cnet
        {0x436f626a, 2}, // Cobj
        {0x43707463, 2}, // Cptc
        {0x43707972, 0}, // Cpyr
        {0x43736163, 2}, // Csac
        {0x43736678, 0}, // Csfx
        {0x43736864, 2}, // Cshd
        {0x4374696c, 2}, // Ctil
        {0x43746f73, 2}, // Ctos
        {0x43776176, 1}, // Cwav
        {0x52504e53, 2}, // RPNS
        {0x63616e6d, 6}, // canm
        {0x736e6473, 5}, // snds
    };

    std::map<uint32_t, uint32_t> ps_header_enum_numbers = {
        {0x43616374, 3}, // Cact
        {0x43626d70, 2}, // Cbmp
        {0x43637472, 1}, // Cctr
        {0x43646373, 2}, // Cdcs
        {0x43666e74, 2}, // Cfnt
        {0x4366756e, 2}, // Cfun
        {0x436d6463, 2}, // Cmdc
        {0x436d6963, 1}, // Cmic
        {0x436e6574, 2}, // Cnet
        {0x436f626a, 2}, // Cobj
        {0x43707463, 2}, // Cptc
        {0x43707972, 1}, // Cpyr
        {0x43736163, 2}, // Csac
        {0x43736678, 1}, // Csfx
        {0x43736864, 2}, // Cshd
        {0x43746474, 1}, // Ctdt
        {0x4374696c, 2}, // Ctil
        {0x4374696e, 2}, // Ctin
        {0x43746f73, 2}, // Ctos
        {0x43747273, 2}, // Ctrs
        {0x43766b62, 1}, // Cvkb
        {0x43766b68, 2}, // Cvkh
        {0x52504e53, 2}  // RPNS
    };
}

bool Data::Mission::IFF::compareFunction( const Data::Mission::Resource *const l_operand, const Data::Mission::Resource *const r_operand ) {
    return ( *l_operand < *r_operand );
}


Data::Mission::Resource* Data::Mission::IFF::getResourceFrom( std::map<uint32_t, std::vector<Data::Mission::Resource*>> &id_to_resource, uint32_t type, unsigned int index ) {
    // O( log n ) for n = the amount of types.
    auto map_it = id_to_resource.find( type );

    if( map_it != id_to_resource.end() && (*map_it).second.size() > index )
        return (*map_it).second[ index ];
    else
        return nullptr;
}

void Data::Mission::IFF::addResourceTo( std::map<uint32_t, std::vector<Data::Mission::Resource*>> &id_to_resource, Data::Mission::Resource* resource_p ) {
    // Get a vector according to the key, the resource tag id.
    // Put this resource into the resources_allocated dynamic array
    id_to_resource[ resource_p->getResourceTagID() ].push_back( resource_p );
    resource_amount++;
}

std::vector<Data::Mission::Resource*> Data::Mission::IFF::getResourcesFrom( std::map<uint32_t, std::vector<Data::Mission::Resource*>> &id_to_resource, uint32_t type ) {
    auto vector_it = id_to_resource.find( type );

    if( vector_it != id_to_resource.end() )
        return std::vector<Data::Mission::Resource*>( (*vector_it).second );
    else // Return all the elements in the array.
        return std::vector<Data::Mission::Resource*>();
}

std::vector<const Data::Mission::Resource*> Data::Mission::IFF::getResourcesFrom( const std::map<uint32_t, std::vector<Data::Mission::Resource*>> &id_to_resource, uint32_t type ) const {
    auto vector_it = id_to_resource.find( type );

    if( vector_it != id_to_resource.end() ) {
        auto res_vec = std::vector<const Data::Mission::Resource*>();

        for( auto i: (*vector_it).second) {
            res_vec.push_back( i );
        }

        return res_vec;
    }
    else // Return all the elements in the array.
        return std::vector<const Data::Mission::Resource*>();
}

std::vector<Data::Mission::Resource*> Data::Mission::IFF::getAllResourcesFrom( std::map<uint32_t, std::vector<Data::Mission::Resource*>> &id_to_resource ) {
    std::vector<Data::Mission::Resource*> entire_resource;

    entire_resource.reserve( resource_amount );

    for( auto map_it = id_to_resource.begin(); map_it != id_to_resource.end(); map_it++ ) {
        for( auto it = map_it->second.begin(); it != map_it->second.end(); it++ )
            entire_resource.push_back( (*it) );
    }

    return entire_resource;
}

std::vector<const Data::Mission::Resource*> Data::Mission::IFF::getAllResourcesFrom( const std::map<uint32_t, std::vector<Resource*>> &id_to_resource ) const {
    std::vector<const Data::Mission::Resource*> entire_resource;

    entire_resource.reserve( resource_amount );

    for( auto map_it = id_to_resource.begin(); map_it != id_to_resource.end(); map_it++ ) {
        for( auto it = map_it->second.begin(); it != map_it->second.end(); it++ )
            entire_resource.push_back( (*it) );
    }

    return entire_resource;
}

Data::Mission::IFF::IFF() {
    this->resource_amount = 0;
    this->music_p = nullptr;
}

Data::Mission::IFF::IFF( const std::string &file_path ) {
    this->resource_amount = 0;
    this->music_p = nullptr;
    open( file_path );
}


Data::Mission::IFF::~IFF() {
    // Delete every allocated resource
    for( auto map_it = id_to_resource_p.begin(); map_it != id_to_resource_p.end(); map_it++ ) {
        for( auto it = map_it->second.begin(); it < map_it->second.end(); it++ )
            delete ( *it );
    }
    for( auto tos_it = tos_to_map_p.begin(); tos_it != tos_to_map_p.end(); tos_it++ ) {
        for( auto map_it = (*tos_it).second.begin(); map_it != (*tos_it).second.end(); map_it++ ) {
            for( auto it = map_it->second.begin(); it < map_it->second.end(); it++ )
                delete ( *it );
        }
    }
}

void Data::Mission::IFF::setName( const std::string &name ) {
    this->name = name;
}

std::string Data::Mission::IFF::getName() const {
    return name;
}

namespace {
    class UnidentifiedResource : public Data::Mission::UnkResource {
    protected:
        static const std::string ext;

    public:
        UnidentifiedResource() : Data::Mission::UnkResource( 0, ext ) {}

        std::unique_ptr<Utilities::Buffer>& getDataReference() { return data; }
    };

    const std::string UnidentifiedResource::ext = "unidentified";

    inline uint32_t chunkToDataSize( uint32_t chunk_size ) {
        auto TYPE_CHUNK_SIZE = sizeof( int32_t ) * 2;

        if( chunk_size >= TYPE_CHUNK_SIZE )
            return chunk_size - TYPE_CHUNK_SIZE;
        else
            return 0;
    }
}

int Data::Mission::IFF::open( const std::string &file_path ) {
    const size_t BLOCK_SIZE = 0x6000;

    std::unordered_set<std::string> filenames; // Check for potential conflicts.
    Utilities::Logger &logger = Utilities::logger;
    std::fstream file;

    auto info_log = logger.getLog( Utilities::Logger::INFO );
    info_log.info << "IFF: " << file_path << "\n";
    auto debug_log = logger.getLog( Utilities::Logger::DEBUG );
    debug_log.info << "IFF: " << file_path << "\n";
    auto warning_log = logger.getLog( Utilities::Logger::WARNING );
    warning_log.info << "IFF: " << file_path << "\n";
    auto error_log = logger.getLog( Utilities::Logger::ERROR );
    error_log.info << "IFF: " << file_path << "\n";

    this->setName( file_path );

    file.open( file_path, std::ios::binary | std::ios::in );

    if( file.is_open() )
    {
        file.seekg(0, std::ios::end);
        const int iff_file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        Data::Mission::Resource::ParseSettings default_settings = Data::Mission::Resource::ParseSettings();

        bool error_in_read = false;

        Utilities::Buffer data_buffer;
        data_buffer.allocate( BLOCK_SIZE );
        Utilities::Buffer::Writer data_writer = data_buffer.getWriter();
        Utilities::Buffer::Reader data_reader = data_buffer.getReader();

        std::vector<UnidentifiedResource> resource_pool;
        MSICResource *msic_p = nullptr;
        Utilities::Buffer *msic_data_p;

        default_settings.endian = Utilities::Buffer::Endian::NO_SWAP;

        uint32_t first_swvr_offset;
        Resource::SWVREntry swvr_entry;

        swvr_entry.tos_offset = 0xfff;
        swvr_entry.offset = 0;
        swvr_entry.name = "NOT SUPPOSED";

        std::map<uint32_t, uint32_t> *header_enum_numbers_r = &pc_header_enum_numbers;
        uint32_t rpns_size = 0xFFFFFFFF;

        uint32_t block_index = 0;

        data_writer.setPosition(0);
        data_writer.write( file, BLOCK_SIZE );
        data_reader.setPosition(0);

        {
            const uint32_t TYPE_ID = data_reader.readU32( default_settings.endian );

            if( TYPE_ID == IN_ENDIAN_CTRL_TAG || TYPE_ID == OP_ENDIAN_CTRL_TAG ) {
                auto info_log = logger.getLog( Utilities::Logger::INFO );
                info_log.info << "IFF: " << file_path << "\n";

                // This determines if the file is big endian or little endian.
                if( WIN_CTRL_TAG[ 0 ] == reinterpret_cast<const char*>(&TYPE_ID)[ 0 ] ) {
                    info_log.output << "This IFF file is little endian (Windows/Playstation) formated.\n";
                    default_settings.endian = Utilities::Buffer::Endian::LITTLE;
                }
                else
                if( MAC_CTRL_TAG[ 0 ] == reinterpret_cast<const char*>(&TYPE_ID)[ 0 ] ) {
                    info_log.output << "This IFF file is big endian (Macintosh) formated.\n";
                    default_settings.endian = Utilities::Buffer::Endian::BIG;
                }

                const int32_t CHUNK_SIZE = data_reader.readI32( default_settings.endian );

                data_reader.setPosition( CHUNK_SIZE );

                // TODO Add playstation detection
            }
            else {
                // TODO Test this case!
                error_log.output << "Expected CTRL chunk at the beginning! Most likely, it is not an Future Cop IFF file.\n";
                error_in_read = true;
            }
        }

        while( !file.eof() && !error_in_read ) {
            while( data_reader.getPosition(Utilities::Buffer::Direction::END) > 2 * sizeof(uint32_t) ) {
                const auto file_offset = BLOCK_SIZE * block_index + data_reader.getPosition();

                assert( iff_file_size > file_offset );

                const uint32_t   TYPE_ID = data_reader.readU32( default_settings.endian );
                const int32_t CHUNK_SIZE = data_reader.readI32( default_settings.endian );
                const uint32_t DATA_SIZE = chunkToDataSize( CHUNK_SIZE );

                if(DATA_SIZE > data_reader.getPosition(Utilities::Buffer::Direction::END)) {
                    error_in_read = true;

                    error_log.output << "Chunk on offset 0x" << std::hex << file_offset << " is too big for the remaining block size.\n";
                    error_log.output << "  chunk_size = 0x" << CHUNK_SIZE << "\n";
                    error_log.output << "  data_size = 0x" << DATA_SIZE << "\n";
                    error_log.output << "  remaining_block_size = 0x" << data_reader.getPosition(Utilities::Buffer::Direction::END) << "\n";

                    break;
                }

                auto block_chunk_reader = data_reader.getReader( DATA_SIZE );

                // Now that the data chunk can be read it is time for this switch statement.
                switch( TYPE_ID ) {
                case FILL_TAG:
                    // Ignore the fill chunk. It is padding
                    break;
                case MSIC_TAG:
                    debug_log.output << "TYPE_ID: MISC CHUNK_SIZE: 0x" << std::hex << CHUNK_SIZE << std::endl;
                    if( msic_p == nullptr ) {
                        msic_p = new MSICResource();
                        msic_p->setIndexNumber( 0 );
                        msic_p->setResourceID( 1 );
                        msic_p->getSWVREntry() = swvr_entry;
                        msic_p->setOffset( file_offset );

                        msic_data_p = new Utilities::Buffer();
                    }

                    assert(msic_p != nullptr);
                    assert(msic_data_p != nullptr);

                    block_chunk_reader.addToBuffer(*msic_data_p, DATA_SIZE);
                    break;
                case PS1_CANM_TAG:
                case PS1_VAGB_TAG:
                case PS1_VAGM_TAG:
                    header_enum_numbers_r = &ps_header_enum_numbers;
                    break;
                case SHOC_TAG:
                    {
                        const auto METADATA = block_chunk_reader.readU32( default_settings.endian );
                        const auto ZERO = block_chunk_reader.readU32( default_settings.endian );
                        const auto CURRENT_TAG = block_chunk_reader.readU32( default_settings.endian );

                        if( ZERO != 0 )
                            warning_log.output << "ZERO for SHOC_TAG is " << std::dec << ZERO << ".\n";

                        // this checks if the chunk holds a file header!
                        if( CURRENT_TAG == SHDR_TAG ) {

                            if( DATA_SIZE >= 0x14 ) {
                                resource_pool.push_back( UnidentifiedResource() );

                                const auto ENUM_NUMBER = block_chunk_reader.readU32( default_settings.endian );

                                const auto TYPE_ENUM = block_chunk_reader.readU32( default_settings.endian );

                                resource_pool.back().setResourceTagID( TYPE_ENUM );
                                resource_pool.back().setOffset( file_offset );
                                resource_pool.back().setMisIndexNumber( resource_pool.size() - 1 );
                                resource_pool.back().setResourceID( block_chunk_reader.readU32( default_settings.endian ) );
                                resource_pool.back().getSWVREntry() = swvr_entry;

                                auto test_enum_number = header_enum_numbers_r->find(TYPE_ENUM);

                                if(test_enum_number == header_enum_numbers_r->end() && ps_header_enum_numbers.find(TYPE_ENUM) != ps_header_enum_numbers.end()) {
                                    test_enum_number = ps_header_enum_numbers.find(TYPE_ENUM);
                                    header_enum_numbers_r = &ps_header_enum_numbers;
                                }

                                if(test_enum_number == header_enum_numbers_r->end()) {
                                    warning_log.output << "SHDR_TAG "
                                        << static_cast<char>((TYPE_ENUM >> 24) & 0xFF) << static_cast<char>((TYPE_ENUM >> 16) & 0xFF)
                                        << static_cast<char>((TYPE_ENUM >>  8) & 0xFF) << static_cast<char>(TYPE_ENUM & 0xFF)
                                        << ": Enum Number Not Present in Table. ENUM_NUMBER = " << std::dec << ENUM_NUMBER << "\n";
                                }
                                else if((*test_enum_number).second != ENUM_NUMBER) {
                                    warning_log.output << "SHDR_TAG "
                                        << static_cast<char>((TYPE_ENUM >> 24) & 0xFF) << static_cast<char>((TYPE_ENUM >> 16) & 0xFF)
                                        << static_cast<char>((TYPE_ENUM >>  8) & 0xFF) << static_cast<char>(TYPE_ENUM & 0xFF)
                                        << ": Expected ENUM_NUMBER " << std::dec << (*test_enum_number).second << ", but got " << ENUM_NUMBER << "\n";
                                }

                                if( METADATA != 0 )
                                    debug_log.output << "SHDR_TAG "
                                        << static_cast<char>((TYPE_ENUM >> 24) & 0xFF) << static_cast<char>((TYPE_ENUM >> 16) & 0xFF)
                                        << static_cast<char>((TYPE_ENUM >>  8) & 0xFF) << static_cast<char>(TYPE_ENUM & 0xFF)
                                        << ": METADATA is " << std::dec << METADATA << " for 0x" << std::hex << file_offset << ".\n";

                                const auto RESOURCE_SIZE = block_chunk_reader.readU32( default_settings.endian );

                                // 0x1c

                                for(size_t i = 0; i < Data::Mission::Resource::RPNS_OFFSET_AMOUNT; i++) {
                                    resource_pool.back().setRPNSOffset( i, block_chunk_reader.readU32( default_settings.endian ) );
                                } // 0x28


                                for(size_t i = 0; i < Data::Mission::Resource::CODE_AMOUNT; i++) {
                                    resource_pool.back().setCodeAmount( i, block_chunk_reader.readU32( default_settings.endian ) );
                                } // 0x30

                                resource_pool.back().setHeaderSize( block_chunk_reader.getPosition( Utilities::Buffer::Direction::END ) );

                                resource_pool.back().getDataReference() = std::make_unique<Utilities::Buffer>();

                                assert( resource_pool.back().getDataReference() != nullptr );

                                resource_pool.back().getDataReference()->reserve( RESOURCE_SIZE + resource_pool.back().getHeaderSize() );
                                block_chunk_reader.addToBuffer(*resource_pool.back().getDataReference(), resource_pool.back().getHeaderSize() );
                            }
                            else {
                                error_in_read = true;
                                break;
                            }
                        }
                        else
                        if( DATA_SIZE >= 0xc && !resource_pool.empty() && CURRENT_TAG == SDAT_TAG ) {
                            if( METADATA != 0 ) {
                                const auto TYPE_ENUM = resource_pool.back().getResourceTagID();

                                debug_log.output << "SHDR_TAG "
                                    << static_cast<char>((TYPE_ENUM >> 24) & 0xFF) << static_cast<char>((TYPE_ENUM >> 16) & 0xFF)
                                    << static_cast<char>((TYPE_ENUM >>  8) & 0xFF) << static_cast<char>(TYPE_ENUM & 0xFF)
                                    << ": METADATA is " << std::dec << METADATA << " for 0x" << std::hex << file_offset << ".\n";
                            }

                            block_chunk_reader.addToBuffer(*resource_pool.back().getDataReference(), DATA_SIZE - 0xc );
                        }
                        else
                            error_log.output << "This SHOC chunk is either too small or has an invalid tag." << std::endl;
                    }

                    debug_log.output << "TYPE_ID: SHOC CHUNK_SIZE: " << CHUNK_SIZE << std::endl;
                    break;
                case SWVR_TAG:
                    if( DATA_SIZE != 0x1c )
                        error_log.output << "SWVR chunk: DATA_SIZE = 0x" << std::hex << DATA_SIZE << "\n";
                    else {
                        if(!swvr_entry.isPresent())
                            first_swvr_offset = file_offset;

                        swvr_entry.offset = file_offset;
                        swvr_entry.tos_offset = file_offset - first_swvr_offset;
                        swvr_entry.name = "";

                        block_chunk_reader.setPosition( 8 );

                        const auto file_identifier = block_chunk_reader.readU32( default_settings.endian );

                        if( file_identifier != 0x46494c45 )
                            warning_log.output << "SWVR chunk: file_identifier is not FILE!\n";

                        int8_t some_char = '1';

                        size_t dot_position = DATA_SIZE;
                        const size_t STRING_LIMIT = DATA_SIZE - 0xc;

                        for( uint32_t i = 0; i < STRING_LIMIT && some_char != '\0'; i++ )
                        {
                            some_char = block_chunk_reader.readI8();

                            if(some_char == '.')
                                dot_position = i;

                            if(some_char != '\0')
                                swvr_entry.name += some_char;
                        }

                        // Now, the swvr name must be cleaned up.

                        // If dot_position is beyond swvr_entry.name then, there is not stream ending to cut out.
                        if( swvr_entry.name.length() > dot_position )
                        {
                            // Get the ".stream" ending cut out of the swvr name.

                            const std::string ending = swvr_entry.name.substr( dot_position );
                            const std::string expecting = std::string(".stream").substr( 0, ending.length() );

                            if( ending.compare(expecting) != 0 ) {
                                warning_log.output << "Offset = 0x" << std::hex << file_offset << ".\n"
                                    << "  \"" << swvr_entry.name << "\" is the name of the SWVR chunk.\n"
                                    << "  Invalid line ending at IFF! This could mean that this IFF file might not work with Future Cop.\n";
                            }
                            else {
                                swvr_entry.name = swvr_entry.name.substr( 0, dot_position );
                            }
                        }
                        else
                        {
                            if( swvr_entry.name.length() != STRING_LIMIT - 1 ) {
                                warning_log.output << "Offset = 0x" << std::hex << file_offset << ".\n"
                                    << "  SWVR name \"" << swvr_entry.name << "\" probably invalid.\n";
                            }
                        }
                    }
                    break;
                default:
                    warning_log.output << "Unknown TYPE_ID: " << static_cast<char>((TYPE_ID >> 24) & 0xFF) << static_cast<char>((TYPE_ID >> 16) & 0xFF) << static_cast<char>((TYPE_ID >> 8) & 0xFF) << static_cast<char>(TYPE_ID & 0xFF) << " CHUNK_SIZE: " << CHUNK_SIZE << ".\n";
                };
            }

            // Advance the block index.
            block_index++;

            if(!file.eof()) {
                data_writer.setPosition(0);
                data_writer.write( file, BLOCK_SIZE );
                data_reader.setPosition(0);
            }
        }

        // Find a potential error.
        auto readstate = file.rdstate();

        if( errno != 0 )
        {
            if( ( readstate & std::ifstream::failbit ) != 0 ) {
                error_log.output << "There is a logical error detected with the reading of the IFF or Mission File. Please report this to the developer!\n"
                    << "Error: " << std::strerror(errno) << "\n";
            }
            if( ( readstate & std::ifstream::badbit ) != 0 ) {
                error_log.output << "There is a input error detected with the reading of the IFF or Mission File.\n";
            }
        }

        // We are done reading the IFF file.
        file.close();

        // Now, every resource can be parsed.
        for( auto &i : resource_pool ) {
            Resource *new_resource_p;

            // Find a resource
            auto file_type_it = file_type_list.find( i.getResourceTagID() );

            // If an element is found.
            if( file_type_it != file_type_list.end() )
                new_resource_p = (*file_type_it).second->genResourceByType( i.getDataReference()->getReader( i.getHeaderSize() ) );
            else // Default to generic resource.
                new_resource_p = new UnkResource( i.getResourceTagID(), "unk" );

            new_resource_p->setHeaderSize( i.getHeaderSize() );
            new_resource_p->setOffset( i.getOffset() );
            new_resource_p->setMisIndexNumber( i.getMisIndexNumber() );
            new_resource_p->setIndexNumber( id_to_resource_p[ i.getResourceTagID() ].size() );
            new_resource_p->setResourceID( i.getResourceID() );
            for(size_t a = 0; a < Data::Mission::Resource::RPNS_OFFSET_AMOUNT; a++) {
                new_resource_p->setRPNSOffset(a, i.getRPNSOffset(a));
            }
            for(size_t a = 0; a < Data::Mission::Resource::CODE_AMOUNT; a++) {
                new_resource_p->setCodeAmount(a, i.getCodeAmount(a));
            }
            new_resource_p->getSWVREntry() = i.getSWVREntry();

            new_resource_p->setMemory( i.getDataReference().release() );

            new_resource_p->parse( default_settings );
            
            // TODO Add option to discard memory once loaded.
            // new_resource_p->setMemory( nullptr );
            
            // Check for naming conflicts
            const std::string file_name = new_resource_p->getFullName( new_resource_p->getResourceID() );
            
            debug_log.output << "Resource Name = \"" << file_name << "\".\n";

            if( filenames.find( file_name ) != filenames.end() ) {
                error_log.output << "Duplicate file name detected for resource name \"" << file_name << "\".\n";
                error_log.output << "Index \"" << new_resource_p->getIndexNumber() << "\".\n";
            }

            filenames.emplace( file_name );

            addResource( new_resource_p );
        }

        // Then write the MISC file.
        if( msic_p != nullptr )
        {
            // This gives msic_data_p to msic so there is no need to delete it.
            msic_p->setMemory( msic_data_p );
            msic_p->parse( default_settings );
            
            // msic_p->setMemory( nullptr );
            addResource( msic_p );
        }

        Data::Accessor accessor;
        accessor.load( *this );

        auto ptc_pointers_r = accessor.getAllPTC();

        if( ptc_pointers_r.size() != 0 ) {
            if( !ptc_pointers_r[0]->makeTiles( accessor.getAllTIL() ) )
            {
                error_log.output << "PTC resource is found, but there are no Til resources.\n";
            }
        }
        else
        if( getResource( Data::Mission::TilResource::IDENTIFIER_TAG ) != nullptr )
            error_log.output << "PTC resource is not found, but the Til resources are in the file.\n";

        if( getResource( Data::Mission::ObjResource::IDENTIFIER_TAG ) != nullptr ) {
            auto textures_from_prime = accessor.getAllBMP();

            // TODO add optional global file.
            // textures.insert( textures.end(), textures.begin(), textures.end() );

            auto objects = accessor.getAllOBJ();

            for( auto it = objects.begin(); it != objects.end(); it++ ) {
                (*it)->loadTextures( textures_from_prime );
            }
        }

        if( getResource( Data::Mission::TilResource::IDENTIFIER_TAG ) != nullptr ) {
            auto textures_from_prime = accessor.getAllBMP();

            // TODO add optional global file.
            // textures.insert( textures.end(), textures.begin(), textures.end() );

            auto sections = accessor.getAllTIL();

            for( auto section = sections.begin(); section != sections.end(); section++ ) {
                if( !(*section)->loadTextures( textures_from_prime ) )
                    error_log.output << "Section/CTil ID: " << std::dec << (*section)->getResourceID() << " had failed to load the textures!.\n";
            }
        }

        return 1;
    }
    else
        return -1;
}

void Data::Mission::IFF::addResource( Data::Mission::Resource* resource_p ) {
    assert(resource_p != nullptr);

    if(!resource_p->getSWVREntry().isPresent())
        addResourceTo(id_to_resource_p, resource_p);
    else if(dynamic_cast<Data::Mission::MSICResource*>(resource_p) != nullptr) {
        if(this->music_p != nullptr)
            delete this->music_p;
        this->music_p = dynamic_cast<Data::Mission::MSICResource*>(resource_p);
    }
    else {
        const auto tos_offset = resource_p->getSWVREntry().tos_offset;
        auto id_to_resource_it = tos_to_map_p.find(tos_offset);

        if(id_to_resource_it == tos_to_map_p.end())
            tos_to_map_p[tos_offset] = std::map<uint32_t, std::vector<Resource*>>();

        addResourceTo(tos_to_map_p[tos_offset], resource_p);
    }
}

Data::Mission::Resource* Data::Mission::IFF::getResource( uint32_t type, unsigned int index ) {
    return getResourceFrom(id_to_resource_p, type, index);
}
const Data::Mission::Resource* Data::Mission::IFF::getResource( uint32_t type, unsigned int index ) const {
    return const_cast<Data::Mission::IFF*>( this )->getResource( type, index );
}

std::vector<Data::Mission::Resource*> Data::Mission::IFF::getResources( uint32_t type ) {
    return getResourcesFrom(id_to_resource_p, type);
}
std::vector<const Data::Mission::Resource*> Data::Mission::IFF::getResources( uint32_t type ) const {
    return const_cast<Data::Mission::IFF*>( this )->getResourcesFrom( id_to_resource_p, type );
}

std::vector<Data::Mission::Resource*> Data::Mission::IFF::getAllResources() {
    return getAllResourcesFrom( id_to_resource_p );
}
std::vector<const Data::Mission::Resource*> Data::Mission::IFF::getAllResources() const {
    return getAllResourcesFrom( id_to_resource_p );
}

Data::Mission::Resource* Data::Mission::IFF::getSWVRResource( uint32_t tos_offset, uint32_t type, unsigned int index ) {
    return getResourceFrom(tos_to_map_p.at(tos_offset), type, index);
}
const Data::Mission::Resource* Data::Mission::IFF::getSWVRResource( uint32_t tos_offset, uint32_t type, unsigned int index ) const {
    return const_cast<Data::Mission::IFF*>( this )->getSWVRResource( tos_offset, type, index );
}

std::vector<Data::Mission::Resource*> Data::Mission::IFF::getSWVRResources( uint32_t tos_offset, uint32_t type ) {
    return getResourcesFrom(tos_to_map_p.at(tos_offset), type);
}
std::vector<const Data::Mission::Resource*> Data::Mission::IFF::getSWVRResources( uint32_t tos_offset, uint32_t type ) const {
    return getResourcesFrom(tos_to_map_p.at(tos_offset), type);
}

std::vector<Data::Mission::Resource*> Data::Mission::IFF::getAllSWVRResources( uint32_t tos_offset ) {
    if(tos_to_map_p.find(tos_offset) == tos_to_map_p.end())
        return std::vector<Data::Mission::Resource*>(); // Empty vector.

    return getAllResourcesFrom( tos_to_map_p.at(tos_offset) );
}

std::vector<const Data::Mission::Resource*> Data::Mission::IFF::getAllSWVRResources( uint32_t tos_offset ) const {
    if(tos_to_map_p.find(tos_offset) == tos_to_map_p.end())
        return std::vector<const Data::Mission::Resource*>(); // Empty vector.

    return getAllResourcesFrom( tos_to_map_p.at(tos_offset) );
}


int Data::Mission::IFF::exportAllResources( const std::string &folder_path, bool raw_file_mode, const std::vector<std::string>& arguments ) const {
    // This algorithm is an O(n) algorithm and it is as good as it is going to get in terms of data complexity. :)
    if( resource_amount != 0 )
    {
        std::string path = folder_path;

        if( path.back() != '/' )
            path += '/';

        Data::Mission::IFFOptions iff_options;

        if( iff_options.readParams( arguments, &std::cout ) ) {
            // For every resource type categories in the Mission file.
            for( auto map_it = id_to_resource_p.begin(); map_it != id_to_resource_p.end(); map_it++ ) {
                for( auto it = map_it->second.begin(); it != map_it->second.end(); it++ ) {
                    std::string full_path = path + (*it)->getFullName( (*it)->getResourceID() );

                    if( raw_file_mode )
                        (*it)->write( full_path, iff_options );
                    else
                    if( !iff_options.enable_global_dry_default )
                        (*it)->writeRaw( full_path, iff_options );
                }
            }
            for( auto tos_it : tos_to_map_p ) {
                for( auto map_it : tos_it.second ) {
                    for( auto it : map_it.second ) {
                        std::string full_path = path + it->getFullName( it->getResourceID() );

                        if( raw_file_mode )
                            it->write( full_path, iff_options );
                        else
                        if( !iff_options.enable_global_dry_default )
                            it->writeRaw( full_path, iff_options );
                    }
                }
            }
            if( this->music_p != nullptr ) {
                std::string full_path = path + this->music_p->getFullName( this->music_p->getResourceID() );

                if( raw_file_mode )
                    this->music_p->write( full_path, iff_options );
                else
                if( !iff_options.enable_global_dry_default )
                    this->music_p->writeRaw( full_path, iff_options );
            }
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

Data::Mission::IFF::DataType Data::Mission::IFF::getDataType() const {
    // Check if PTC is present.
    if(id_to_resource_p.find(PTCResource::IDENTIFIER_TAG) == id_to_resource_p.end())
        return DataType::GLOBALS;

    Data::Accessor accessor;
    accessor.loadConstant(*this);

    // If Sky Captain exists does then this IFF file is deemed to be a Precinct Assualt Map.
    if(!accessor.getActorAccessor().getAllConstSkyCaptain().empty())
        return DataType::PRECINCT_ASSUALT;

    return DataType::CRIME_WAR;
}

int Data::Mission::IFF::compare( const IFF &operand, std::ostream &out ) const {
    size_t index_of_this = 0;
    size_t index_of_operand = 0;
    int successful_finds = 0;
    int previous_count = 0;
    std::string currentType = "@@@@"; // This value simply states which

    // This is for comparision between IFFs.
    std::vector<Resource*> operand1( const_cast< Data::Mission::IFF* >(  this  )->getAllResources() );
    std::vector<Resource*> operand2( const_cast< Data::Mission::IFF& >( operand ).getAllResources() );

    // This makes sure that the operation is not a O-log(n^2) operation when the comparing happens.
    std::sort( operand1.begin(), operand1.end(), compareFunction );
    std::sort( operand2.begin(), operand2.end(), compareFunction );

    while( index_of_this < operand1.size() && index_of_operand < operand2.size() ) {
        if( currentType.compare( operand1.at( index_of_this )->getFileExtension() ) != 0 ) {
            if( previous_count != 0 )
                out << previous_count << " pairs of " << currentType << " found!" << std::endl;

            currentType = operand1.at( index_of_this )->getFileExtension();
            out << std::endl << "For format " << currentType << std::endl;

            previous_count = 0;
        }

        if( *operand1.at( index_of_this ) > *operand2.at( index_of_operand ) )
        {
            index_of_operand++;
        }
        else
        if( *operand1.at( index_of_this ) < *operand2.at( index_of_operand ) )
        {
            index_of_this++;
        }
        else
        {
            out << "\t" << name << " Index:" << operand1.at( index_of_this )->getIndexNumber() << std::endl
                << "\t" << operand.name <<" Index:" << operand2.at( index_of_operand )->getIndexNumber()
                << std::endl << std::endl;
            index_of_this++;
            index_of_operand++;

            successful_finds++;
            previous_count++;
       }
    }
    if( previous_count != 0 )
        out << previous_count << " pairs of " << currentType << " found!" << std::endl;

    out << "Overall there where " << successful_finds << " matches" << std::endl;

    out << "index_of_this is " << index_of_this << " while " << operand1.size() << " is the limit" << std::endl;
    out << "index_of_operand is " << index_of_operand << " while " << operand2.size() << " is the limit" << std::endl;

    return successful_finds;
}

void Data::Mission::IFF::generatePlaceholders( Data::Mission::IFF &iff, bool map_spawn ) {
    auto endian = Utilities::Buffer::LITTLE;
    auto logger_r = &Utilities::logger;

    iff.name = "EMBEDDED";

    // Generate the fonts just in case.
    iff.addResource( FontResource::getPlaystation( 1, logger_r ) );
    iff.addResource( FontResource::getWindows( 2, logger_r ) );

    if(!map_spawn)
        return; // Skip map generation.

    // Generate Placeholder Map
    std::vector<BMPResource*> bmp_r;

    // Generate this texture.
    {
        BMPResource *resource_r = BMPResource::getTest( 2, logger_r );

        iff.addResource( resource_r );
        bmp_r.push_back( resource_r );
    }

    std::vector<TilResource*> tils_r;

    // Generate two flat Sections.
    for(int i = 0; i < 2; i++) {
        TilResource *resource_r = TilResource::getTest( tils_r.size() + 1, 110 + i, true, false, endian, logger_r );

        iff.addResource( resource_r );
        tils_r.push_back( resource_r );
    }

    // Generate 6 Sections.
    for(int i = 0; i < 7; i++) {
        TilResource *resource_r = TilResource::getTest( tils_r.size() + 1, 16 * i, true, false, endian, logger_r );

        iff.addResource( resource_r );
        tils_r.push_back( resource_r );
    }

    // Generate 6 Sections.
    for(int i = 0; i < 7; i++) {
        TilResource *resource_r = TilResource::getTest( tils_r.size() + 1, 16 * i, true, true, endian, logger_r );

        iff.addResource( resource_r );
        tils_r.push_back( resource_r );
    }

    for( auto section = tils_r.begin(); section != tils_r.end(); section++ ) {
        (*section)->loadTextures( bmp_r );
    }

    {
        PTCResource *resource_r = PTCResource::getTest( 1, endian, logger_r );
        resource_r->makeTiles( tils_r );
        iff.addResource( resource_r );
    }
}
