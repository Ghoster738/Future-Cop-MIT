#include "IFF.h"

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
#include "SNDSResource.h"
#include "WAVResource.h"
#include "ObjResource.h"
#include "UnkResource.h"
#include "RPNSResource.h"

#include "ACT/Unknown.h"
#include "ACT/SkyCaptain.h"

#include "../../Utilities/Buffer.h"

#include <fstream>
#include <algorithm>
#include <cstring>
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
        { Data::Mission::ANMResource::IDENTIFIER_TAG, new Data::Mission::ANMResource() }, // Resource ID missing
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
        { 0x43736678, new Data::Mission::UnkResource( 0x43736678, "sfx", true ) },  // Resource ID missing
        // which is { 0x43, 0x73, 0x68, 0x64 } or { 'C', 's', 'h', 'd' } or "Cshd"
        { 0x43736864, new Data::Mission::UnkResource( 0x43736864, "shd", true ) }, // Holds programmer settings?  // Resource ID missing
        { Data::Mission::TilResource::IDENTIFIER_TAG, new Data::Mission::TilResource() },
        // which is { 0x43, 0x74, 0x6F, 0x73 } or { 'C', 't', 'o', 's' } or "Ctos"
        { 0x43746F73, new Data::Mission::UnkResource( 0x43746F73, "tos" ) },  // time of sounds?
        { Data::Mission::WAVResource::IDENTIFIER_TAG, new Data::Mission::WAVResource() },
        // which is { 0x43, 0x61, 0x69, 0x66 } or { 'C', 'a', 'i', 'f' } or "Caif"
        { 0x43616966, new Data::Mission::UnkResource( 0x43616966, "aif" ) },
        { Data::Mission::RPNSResource::IDENTIFIER_TAG, new Data::Mission::RPNSResource() },
        { Data::Mission::SNDSResource::IDENTIFIER_TAG, new Data::Mission::SNDSResource() }, // Resource ID missing
        // which is { 0x53, 0x57, 0x56, 0x52 } or { 'S', 'W', 'V', 'R' } or "SWVR"
        { 0x53575652, new Data::Mission::UnkResource( 0x53575652, "swvr" ) },
        { Data::Mission::FUNResource::IDENTIFIER_TAG, new Data::Mission::FUNResource() },
        { 0x43766b68, new Data::Mission::UnkResource( 0x43766b68, "vkh" ) }, // PS1 Missions.
        { 0x43766b62, new Data::Mission::UnkResource( 0x43766b68, "vkb" ) },
        { 0x43747273, new Data::Mission::UnkResource( 0x43747273, "trs" ) }, // PS1 Global.
        { 0x436d6463, new Data::Mission::UnkResource( 0x436d6463, "mdc" ) },
        { 0x4374696e, new Data::Mission::UnkResource( 0x4374696e, "tin" ) },
        { 0x43746474, new Data::Mission::UnkResource( 0x43746474, "tdt" ) },
        { 0x436d6963, new Data::Mission::UnkResource( 0x436d6963, "mic" ) }
    };
    class AutoDelete {
    public:
        ~AutoDelete() {
            for( auto &i : file_type_list ) {
                delete i.second;
            }
        }
    } auto_delete_file_type_list;
}

bool Data::Mission::IFF::compareFunction( const Data::Mission::Resource *const l_operand, const Data::Mission::Resource *const r_operand ) {
    return ( *l_operand < *r_operand );
}

Data::Mission::IFF::IFF() {
    resource_amount = 0;
}

Data::Mission::IFF::IFF( const std::string &file_path ) {
    resource_amount = 0;
    open( file_path );
}


Data::Mission::IFF::~IFF() {
    // Delete every allocated resource in std::map
    for( auto map_it = resource_map.begin(); map_it != resource_map.end(); map_it++ ) {
        for( auto it = map_it->second.begin(); it < map_it->second.end(); it++ )
            delete ( *it );
    }
}

void Data::Mission::IFF::setName( const std::string &name ) {
    this->name = name;
}

std::string Data::Mission::IFF::getName() const {
    return name;
}

namespace {
    struct ResourceType {
        uint32_t type_enum;
        uint32_t offset;
        uint32_t iff_index;
        uint32_t resource_id;
        std::string name_swvr;
        Utilities::Buffer *header_p;
        Utilities::Buffer *data_p;
    };

    inline uint32_t chunkToDataSize( uint32_t chunk_size ) {
        auto TYPE_CHUNK_SIZE = sizeof( int32_t ) * 2;

        if( chunk_size >= TYPE_CHUNK_SIZE )
            return chunk_size - TYPE_CHUNK_SIZE;
        else
            return 0;
    }
}

int Data::Mission::IFF::open( const std::string &file_path ) {
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

        // it is set like this in default because it has not found the header yet.
        // since this is a do while loop the loop would only run once if it did not find the header.
        bool error_in_read = true;

        // There are two loading buffers for the loader
        Utilities::Buffer type_buffer;
        type_buffer.allocate( sizeof( uint32_t ) + sizeof( int32_t ) );
        Utilities::Buffer::Writer type_writer = type_buffer.getWriter();
        Utilities::Buffer::Reader type_reader = type_buffer.getReader();

        Utilities::Buffer data_buffer;
        data_buffer.allocate( 0x6000 ); // This is a little higher than the biggest chunk of ConFt.
        Utilities::Buffer::Writer data_writer = data_buffer.getWriter();
        Utilities::Buffer::Reader data_reader = data_buffer.getReader();

        std::vector<ResourceType> resource_pool;
        MSICResource *msic_p = nullptr;
        Utilities::Buffer *msic_data_p;

        {
            auto info_log = logger.getLog( Utilities::Logger::INFO );
            info_log.info << "IFF: " << file_path << "\n";

            type_writer.write( file, type_reader.totalSize() );

            const uint32_t TYPE_ID = type_reader.readU32( Utilities::Buffer::Endian::NO_SWAP );

            // First Read the header.
            if( TYPE_ID == IN_ENDIAN_CTRL_TAG || TYPE_ID == OP_ENDIAN_CTRL_TAG ) {
                error_in_read = false;

                // This determines if the file is big endian or little endian.
                if( WIN_CTRL_TAG[ 0 ] == reinterpret_cast<const char*>(&TYPE_ID)[ 0 ] ) {
                    info_log.output << "This IFF file is little endian (Windows/Playstation) formated.\n";
                    default_settings.type = Resource::ParseSettings::Windows; // Might be Playstation file as well.
                    default_settings.endian = Utilities::Buffer::Endian::LITTLE;
                }
                else
                if( MAC_CTRL_TAG[ 0 ] == reinterpret_cast<const char*>(&TYPE_ID)[ 0 ] ) {
                    info_log.output << "This IFF file is big endian (Macintosh) formated.\n";
                    default_settings.type = Resource::ParseSettings::Macintosh;
                    default_settings.endian = Utilities::Buffer::Endian::BIG;
                }

                const int32_t CHUNK_SIZE = type_reader.readI32( default_settings.endian );
                file.seekg( chunkToDataSize( CHUNK_SIZE ), std::ios_base::cur );

                // TODO Add playstation detection
            }
        }

        int file_offset = 0;
        std::string name_swvr = "";

        while( file && !error_in_read ) {
            file_offset = file.tellg();

            type_writer.setPosition(0);
            type_reader.setPosition(0);

            const auto READ_AMOUNT = type_writer.write( file, type_reader.totalSize() );

            if( READ_AMOUNT != type_reader.totalSize() ) {
                error_in_read = true;
            }
            else {

                const uint32_t TYPE_ID = type_reader.readU32( default_settings.endian );
                const uint32_t CHUNK_SIZE = type_reader.readI32( default_settings.endian );
                const uint32_t DATA_SIZE = chunkToDataSize( CHUNK_SIZE );

                if( TYPE_ID == FILL_TAG ) {
                    debug_log.output << "TYPE_ID: " << "FILL" << " CHUNK_SIZE: " << CHUNK_SIZE << std::endl;

                    // This tag does not have any useable information. This might have been used to demiout certain files.

                    if( static_cast<uint32_t>(CHUNK_SIZE) == SHOC_TAG )
                        file.seekg( file_offset + sizeof( TYPE_ID ), std::ios_base::beg ); // Only skip the TYPE_ID.
                    else
                        file.seekg( DATA_SIZE, std::ios_base::cur ); // Skip the FILL buffer.
                }
                else
                if( DATA_SIZE + file.tellg() < iff_file_size )
                {
                    // Check if the buffer is too high.
                    if( DATA_SIZE > data_reader.totalSize() ) {
                        // data_buffer.allocate( DATA_SIZE - data_reader.totalSize() );
                        error_log.output << std::hex << " The limit of 0x" << data_reader.totalSize() << " for this reader has been reached" << std::endl;
                        error_in_read = true;
                    }

                    data_writer.setPosition(0);
                    data_reader.setPosition(0);

                    // Finally read the buffer.
                    auto amount_written = data_writer.write( file, DATA_SIZE );
                    if( amount_written != DATA_SIZE )
                        warning_log.output << "amount_written is 0x" << std::hex << amount_written << " while \nDATA_SIZE is 0x" << DATA_SIZE << ".\n";

                    data_reader.setPosition( 8 );

                    const auto CURRENT_TAG = data_reader.readU32( default_settings.endian );

                    if( TYPE_ID == SHOC_TAG ) {
                        // this checks if the chunk holds a file header!
                        if( CURRENT_TAG == SHDR_TAG ) {

                            if( DATA_SIZE >= 20 ) {
                                resource_pool.push_back( ResourceType() );

                                // These are the tag sizes to be expected.
                                // Subtract them by 20 and we would get the header size.
                                // The smallest DATA_SIZE is 52, and the biggest data size is 120.
                                if( !((DATA_SIZE == 52) || (DATA_SIZE ==  56) || (DATA_SIZE ==  60) || (DATA_SIZE ==  64) ||
                                      (DATA_SIZE == 72) || (DATA_SIZE ==  76) || (DATA_SIZE ==  80) || (DATA_SIZE ==  84) ||
                                      (DATA_SIZE == 96) || (DATA_SIZE == 100) || (DATA_SIZE == 116) || (DATA_SIZE == 120)) )
                                    warning_log.output << "DATA_SIZE has an unexpected size of " << DATA_SIZE << ".\n";

                                data_reader.setPosition( 16 );

                                resource_pool.back().type_enum = data_reader.readU32( default_settings.endian );
                                resource_pool.back().offset    = file_offset;
                                resource_pool.back().iff_index = resource_pool.size() - 1;
                                resource_pool.back().resource_id = data_reader.readU32( default_settings.endian );
                                resource_pool.back().name_swvr = name_swvr;

                                resource_pool.back().data_p = new Utilities::Buffer();
                                if( resource_pool.back().data_p != nullptr )
                                    resource_pool.back().data_p->reserve( data_reader.readU32( default_settings.endian ) );

                                resource_pool.back().header_p = new Utilities::Buffer( reinterpret_cast<uint8_t*>(data_buffer.dangerousPointer() + 28), DATA_SIZE - 28 );
                            }
                            else
                                error_in_read = true;
                        }
                        else
                        if( DATA_SIZE >= 12 && !resource_pool.empty() && CURRENT_TAG == SDAT_TAG ) {
                            resource_pool.back().data_p->add( reinterpret_cast<uint8_t*>(data_buffer.dangerousPointer() + 12), DATA_SIZE - 12 );
                        }
                        else
                            error_log.output << "This SHOC chunk is either too small or has an invalid tag." << std::endl;

                        debug_log.output << "TYPE_ID: " << "SHOC" << " CHUNK_SIZE: " << CHUNK_SIZE << std::endl;
                    }
                    else
                    if( TYPE_ID == MSIC_TAG ) {
                        debug_log.output << "TYPE_ID: " << "MISC" << " CHUNK_SIZE: " << CHUNK_SIZE << std::endl;
                        if( msic_p == nullptr ) {
                            msic_p = new MSICResource();
                            msic_p->setIndexNumber( 0 );
                            msic_p->setResourceID( 1 );
                            msic_p->setSWVRName( name_swvr );
                            msic_p->setOffset( file_offset );

                            msic_data_p = new Utilities::Buffer();
                        }

                        if( msic_p != nullptr )
                            msic_data_p->add( reinterpret_cast<uint8_t*>(data_buffer.dangerousPointer()), DATA_SIZE );
                    }
                    else
                    if( TYPE_ID == SWVR_TAG ) {
                        if( DATA_SIZE != 28 || DATA_SIZE >= data_reader.totalSize() || amount_written != DATA_SIZE )
                            error_log.output << "SWVR chunk: DATA_SIZE = " << std::dec << DATA_SIZE << "amount_written is " << amount_written << "\n";
                        else {
                            name_swvr = "";

                            data_reader.setPosition( 8 );

                            const auto file_identifier = data_reader.readU32( default_settings.endian );

                            if( file_identifier != 0x46494c45 )
                                warning_log.output << "SWVR chunk: file_identifier is not FILE!\n";

                            int8_t some_char = '1';

                            size_t dot_position = DATA_SIZE;
                            const size_t STRING_LIMIT = DATA_SIZE - 12;

                            for( uint32_t i = 0; i < STRING_LIMIT && some_char != '\0'; i++ )
                            {
                                some_char = data_reader.readI8();

                                if(some_char == '.')
                                    dot_position = i;

                                if(some_char != '\0')
                                    name_swvr += some_char;
                            }

                            // Now, the swvr name must be cleaned up.

                            // If dot_position is beyond name_swvr then, there is not stream ending to cut out.
                            if( name_swvr.length() > dot_position )
                            {
                                // Get the ".stream" ending cut out of the swvr name.

                                const std::string ending = name_swvr.substr( dot_position );
                                const std::string expecting = std::string(".stream").substr( 0, ending.length() );

                                if( ending.compare(expecting) != 0 ) {
                                    warning_log.output << "Offset = 0x" << std::hex << file_offset << ".\n"
                                        << "  \"" << name_swvr << "\" is the name of the SWVR chunk.\n"
                                        << "  Invalid line ending at IFF! This could mean that this IFF file might not work with Future Cop.\n";
                                }
                                else {
                                    name_swvr = name_swvr.substr( 0, dot_position );
                                }
                            }
                            else
                            {
                                if( name_swvr.length() != STRING_LIMIT - 1 ) {
                                    warning_log.output << "Offset = 0x" << std::hex << file_offset << ".\n"
                                        << "  SWVR name \"" << name_swvr << "\" probably invalid.\n";
                                }
                            }
                        }
                    }
                    else
                    if( TYPE_ID == PS1_CANM_TAG ) {
                        // TODO Read in this case.
                    }
                    else
                    if( TYPE_ID == PS1_VAGB_TAG ) {
                        // TODO Read in this case.
                    }
                    else
                    if( TYPE_ID == PS1_VAGM_TAG ) {
                        // TODO Read in this case.
                    }
                    else
                        warning_log.output << "TYPE_ID: " << static_cast<char>((TYPE_ID >> 24) & 0xFF) << static_cast<char>((TYPE_ID >> 16) & 0xFF) << static_cast<char>((TYPE_ID >> 8) & 0xFF) << static_cast<char>(TYPE_ID & 0xFF) << " CHUNK_SIZE: " << CHUNK_SIZE << ".\n";
                }
                else
                    error_in_read = true;
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
            auto file_type_it = file_type_list.find( i.type_enum );

            // If an element is found.
            if( file_type_it != file_type_list.end() )
                new_resource_p = (*file_type_it).second->genResourceByType( *i.header_p, *i.data_p );
            else // Default to generic resource.
                new_resource_p = new UnkResource( i.type_enum, "unk" );

            new_resource_p->setOffset( i.offset );
            new_resource_p->setMisIndexNumber( i.iff_index );
            new_resource_p->setIndexNumber( resource_map[ i.type_enum ].size() );
            new_resource_p->setResourceID( i.resource_id );
            new_resource_p->setSWVRName( i.name_swvr );

            new_resource_p->setMemory( i.header_p, i.data_p );
            new_resource_p->processHeader( default_settings );
            new_resource_p->parse( default_settings );
            
            // TODO Add option to discard memory once loaded.
            // new_resource_p->setMemory( nullptr, nullptr );
            
            i.header_p = nullptr;
            i.data_p   = nullptr;
            
            // Check for naming conflicts
            const std::string file_name = new_resource_p->getFullName( new_resource_p->getResourceID() );
            
            debug_log.output << "Resource Name = \"" << file_name << "\".\n";

            if( filenames.find( file_name ) != filenames.end() )
                error_log.output << "Duplicate file name detected for resource name \"" << file_name << "\".\n";

            filenames.emplace( file_name );

            addResource( new_resource_p );
        }

        // Then write the MISC file.
        if( msic_p != nullptr )
        {
            // This gives msic_data_p to msic so there is no need to delete it.
            msic_p->setMemory( nullptr, msic_data_p );
            msic_p->parse( default_settings );
            
            // msic_p->setMemory( nullptr, nullptr );
            addResource( msic_p );
        }

        std::vector<PTCResource*> pyr_pointer_r = Data::Mission::PTCResource::getVector( *this );

        if( pyr_pointer_r.size() != 0 ) {
            if( !pyr_pointer_r[0]->makeTiles( Data::Mission::TilResource::getVector( *this ) ) )
            {
                error_log.output << "PYR resource is found, but there are no Til resources.\n";
            }
        }
        else
        if( getResource( Data::Mission::TilResource::IDENTIFIER_TAG ) != nullptr )
            error_log.output << "PYR resource is not found, but the Til resources are in the file.\n";

        if( getResource( Data::Mission::ObjResource::IDENTIFIER_TAG ) != nullptr ) {
            std::vector<Data::Mission::BMPResource*> textures_from_prime = Data::Mission::BMPResource::getVector( *this );

            // TODO add optional global file.
            // textures.insert( textures.end(), textures.begin(), textures.end() );

            std::vector<Data::Mission::ObjResource*> objects = Data::Mission::ObjResource::getVector( *this );

            for( auto it = objects.begin(); it != objects.end(); it++ ) {
                (*it)->loadTextures( textures_from_prime );
            }
        }

        if( getResource( Data::Mission::TilResource::IDENTIFIER_TAG ) != nullptr ) {
            std::vector<Data::Mission::BMPResource*> textures_from_prime = Data::Mission::BMPResource::getVector( *this );

            // TODO add optional global file.
            // textures.insert( textures.end(), textures.begin(), textures.end() );

            std::vector<Data::Mission::TilResource*> sections = Data::Mission::TilResource::getVector( *this );

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

void Data::Mission::IFF::addResource( Data::Mission::Resource* resource ) {
    // Get a vector according to the key, the resource tag id.
    // Put this resource into the resources_allocated dynamic array
    resource_map[ resource->getResourceTagID() ].push_back( resource );
    resource_amount++;
}

Data::Mission::Resource* Data::Mission::IFF::getResource( uint32_t type, unsigned int index ) {
    // O( log n ) for n = the amount of types.
    auto map_it = resource_map.find( type );

    if( map_it != resource_map.end() && (*map_it).second.size() > index )
        return (*map_it).second[ index ];
    else
        return nullptr;
}
const Data::Mission::Resource* Data::Mission::IFF::getResource( uint32_t type, unsigned int index ) const {
    return const_cast<Data::Mission::IFF*>( this )->getResource( type, index );
}

std::vector<Data::Mission::Resource*> Data::Mission::IFF::getResources( uint32_t type ) {
    auto vector_it = resource_map.find( type );

    if( vector_it != resource_map.end() )
        return std::vector<Data::Mission::Resource*>( (*vector_it).second );
    else // Return all the elements in the array.
        return std::vector<Data::Mission::Resource*>();
}
const std::vector<Data::Mission::Resource*> Data::Mission::IFF::getResources( uint32_t type ) const {
    return const_cast<Data::Mission::IFF*>( this )->getResources( type );
}

std::vector<Data::Mission::Resource*> Data::Mission::IFF::getAllResources() {
    std::vector<Data::Mission::Resource*> entire_resource;

    entire_resource.reserve( resource_amount );

    for( auto map_it = resource_map.begin(); map_it != resource_map.end(); map_it++ ) {
        for( auto it = map_it->second.begin(); it != map_it->second.end(); it++ )
            entire_resource.push_back( (*it) );
    }

    return entire_resource;
}
const std::vector<Data::Mission::Resource*> Data::Mission::IFF::getAllResources() const {
    return const_cast<Data::Mission::IFF*>( this )->getAllResources();
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
            for( auto map_it = resource_map.begin(); map_it != resource_map.end(); map_it++ )
            {
                for( auto it = map_it->second.begin(); it != map_it->second.end(); it++ )
                {
                    std::string full_path = path + (*it)->getFullName( (*it)->getResourceID() );

                    if( raw_file_mode )
                        (*it)->write( full_path, iff_options );
                    else
                    if( !iff_options.enable_global_dry_default )
                        (*it)->writeRaw( full_path, iff_options );
                }
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
    if(resource_map.find(PTCResource::IDENTIFIER_TAG) == resource_map.end())
        return DataType::GLOBALS;

    std::vector<ACTResource*> act_resources = ACTResource::getVector( *this );

    // Find if Sky Captain exists. If he does then this IFF file is deemed to be a Precinct Assualt Map.
    for( auto act = act_resources.begin(); act != act_resources.end(); act++ ) {
        if((*act)->getTypeID() == ACT::SkyCaptain::TYPE_ID)
            return DataType::PRECINCT_ASSUALT;
    }

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
