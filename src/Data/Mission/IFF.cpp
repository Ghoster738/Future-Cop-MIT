#include "IFF.h"

#include "ANMResource.h"
#include "ACTResource.h"
#include "BMPResource.h"
#include "DCSResource.h"
#include "FontResource.h"
#include "MSICResource.h"
#include "NetResource.h"
#include "PTCResource.h"
#include "PYRResource.h"
#include "TilResource.h"
#include "SNDSResource.h"
#include "WAVResource.h"
#include "ObjResource.h"
#include "UnkResource.h"

#include "ACT/Unknown.h"

#include "../../Utilities/DataHandler.h"
#include "../../Utilities/Buffer.h"

#include <fstream>
#include <algorithm>
#include <cstring> // std::strerror
#include <cassert> // std::strerror
#include <iostream>

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
        { 0x43736678, new Data::Mission::UnkResource( 0x43736678, "sfx" ) },
        // which is { 0x43, 0x73, 0x68, 0x64 } or { 'C', 's', 'h', 'd' } or "Cshd"
        { 0x43736864, new Data::Mission::UnkResource( 0x43736864, "shd" ) }, // Holds programmer settings?
        { Data::Mission::TilResource::IDENTIFIER_TAG, new Data::Mission::TilResource() },
        // which is { 0x43, 0x74, 0x6F, 0x73 } or { 'C', 't', 'o', 's' } or "Ctos"
        { 0x43746F73, new Data::Mission::UnkResource( 0x43746F73, "tos" ) },  // time of sounds?
        { Data::Mission::WAVResource::IDENTIFIER_TAG, new Data::Mission::WAVResource() },
        // which is { 0x52, 0x50, 0x4E, 0x53 } or { 'R', 'P', 'N', 'S' } or "RPNS"
        { 0x52504E53, new Data::Mission::UnkResource( 0x52504E53, "rpns" ) },
        { Data::Mission::SNDSResource::IDENTIFIER_TAG, new Data::Mission::SNDSResource() },
        // which is { 0x53, 0x57, 0x56, 0x52 } or { 'S', 'W', 'V', 'R' } or "SWVR"
        { 0x53575652, new Data::Mission::UnkResource( 0x53575652, "swvr" ) }
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
    type = UNKNOWN;
    resource_amount = 0;
}

Data::Mission::IFF::IFF( const char *const  file_path ) {
    type = UNKNOWN;
    resource_amount = 0;
    open( file_path );
}

Data::Mission::IFF::IFF( const std::string &file_path ) {
    type = UNKNOWN;
    resource_amount = 0;
    open( file_path.c_str() );
}


Data::Mission::IFF::~IFF() {
    // Delete every allocated resource in std::map
    for( auto map_it = resource_map.begin(); map_it != resource_map.end(); map_it++ ) {
        for( auto it = map_it->second.begin(); it < map_it->second.end(); it++ )
            delete ( *it );
    }
}

void Data::Mission::IFF::setName( std::string name ) {
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

int Data::Mission::IFF::open( const char *const file_path ) {
    std::fstream file;

    this->setName( file_path );

    file.open( file_path, std::ios::binary | std::ios::in );

    if( file.is_open() )
    {
        file.seekg(0, std::ios::end);
        const int iff_file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        Data::Mission::Resource::ParseSettings default_settings = Data::Mission::Resource::ParseSettings();
        default_settings.is_opposite_endian = false; // Does the cpu have the opposite endian type as the file that is being read.

        // it is set like this in default because it has not found the header yet.
        // since this is a do while loop the loop would only run once if it did not find the header.
        bool error_in_read = true;
        bool is_confirmend_iff_file = false;

        uint32_t typeID;
        int32_t chunk_size; // Yes, it is a signed integer.
        int32_t data_size; // This is the chunk size only 8 bytes less.

        // There are two loading buffers for the loader
        char  type_buffer[ sizeof( typeID ) + sizeof( chunk_size ) ];
        int   data_buffer_size = 0x6000; // This is a little higher than the biggest chunk of ConFt.
        char *data_buffer = new char [data_buffer_size];

        int file_offset = 0;
        std::vector<ResourceType> resource_pool;
        MSICResource *msic_p = nullptr;
        Utilities::Buffer *msic_data_p;

        do
        {
            file_offset = file.tellg();

            file.read( type_buffer, sizeof( type_buffer ) );
            typeID = Utilities::DataHandler::read_u32( reinterpret_cast<uint8_t*>(type_buffer), default_settings.is_opposite_endian );
            chunk_size = Utilities::DataHandler::read_32( reinterpret_cast<uint8_t*>(type_buffer + sizeof( typeID )), default_settings.is_opposite_endian );

            data_size = chunkToDataSize( chunk_size );

            if( !is_confirmend_iff_file ) {
                if( typeID == IN_ENDIAN_CTRL_TAG || typeID == OP_ENDIAN_CTRL_TAG )
                {
                    error_in_read = false;
                    is_confirmend_iff_file = true;

                    // This determines if the file is big endian or little endian.
                    if( WIN_CTRL_TAG[ 0 ] == type_buffer[ 0 ] ) {
                        this->type = FILE_IS_LITTLE_ENDIAN;
                        std::cout << "\"" << file_path << "\" is a valid little endian mission file" << std::endl;
                        default_settings.type = Resource::ParseSettings::Windows; // Might be Playstation file as well.
                        default_settings.is_opposite_endian = !Utilities::DataHandler::is_little_endian(); // TODO Remove this
                        default_settings.endian = Utilities::Buffer::Endian::LITTLE;
                    }
                    else
                    if( MAC_CTRL_TAG[ 0 ] == type_buffer[ 0 ] ) {
                        this->type = FILE_IS_BIG_ENDIAN;
                        std::cout << "\"" << file_path << "\" is a valid big endian mission file" << std::endl;
                        default_settings.type = Resource::ParseSettings::Macintosh;
                        default_settings.is_opposite_endian = Utilities::DataHandler::is_little_endian();
                        default_settings.endian = Utilities::Buffer::Endian::BIG;
                    }
                    else
                        this->type = UNKNOWN;

                    // Skip the data chunk
                    chunk_size = Utilities::DataHandler::read_32( reinterpret_cast<uint8_t*>(type_buffer + sizeof( typeID )), default_settings.is_opposite_endian );

                    data_size = chunkToDataSize( chunk_size );
                    file.seekg( data_size, std::ios_base::cur );

                    // TODO Add playstation detection
                }
            }
            else
            if( typeID == FILL_TAG ) {
                //std::cout << "typeID: " << "FILL" << " chunk_size: " << chunk_size << std::endl;

                // This tag does not have any useable information. This might have been used to demiout certain files.

                if( static_cast<uint32_t>(chunk_size) == SHOC_TAG )
                    file.seekg( file_offset + sizeof( typeID ), std::ios_base::beg ); // Only skip the typeID.
                else
                    file.seekg( data_size, std::ios_base::cur ); // Skip the FILL buffer.
            }
            else
            if( data_size + file.tellg() < iff_file_size )
            {
                // Extend the data buffer if it is too small.
                if( data_size > data_buffer_size ) {
                    delete [] data_buffer;
                    data_buffer_size = data_size;
                    data_buffer = new char [data_buffer_size];
                    // std::cout << "data_buffer resized to " << data_buffer_size << std::endl;
                }

                // Finally read the buffer.
                file.read( data_buffer, data_size );

                if( typeID == SHOC_TAG ) {
                    // this checks if the chunk holds a file header!
                    if( Utilities::DataHandler::read_u32( reinterpret_cast<uint8_t*>(data_buffer + 8), default_settings.is_opposite_endian ) == SHDR_TAG ) {
                        if( data_size >= 20 ) {
                            resource_pool.push_back( ResourceType() );
                            
                            // These are the tag sizes to be expected.
                            // Subtract them by 20 and we would get the header size.
                            // The smallest data_size is 52, and the biggest data size is 120.
                            assert( (data_size == 52) || (data_size ==  56) || (data_size ==  60) || (data_size ==  64) ||
                                    (data_size == 72) || (data_size ==  76) || (data_size ==  80) || (data_size ==  84) ||
                                    (data_size == 96) || (data_size == 100) || (data_size == 116) || (data_size == 120) );

                            resource_pool.back().type_enum = Utilities::DataHandler::read_u32( reinterpret_cast<uint8_t*>(data_buffer + 16), default_settings.is_opposite_endian );
                            resource_pool.back().offset    = file_offset;
                            resource_pool.back().iff_index = resource_pool.size() - 1;
                            resource_pool.back().resource_id = Utilities::DataHandler::read_u32( reinterpret_cast<uint8_t*>(data_buffer + 20), default_settings.is_opposite_endian );
                            
                            resource_pool.back().data_p = new Utilities::Buffer();
                            if( resource_pool.back().data_p != nullptr )
                                resource_pool.back().data_p->reserve( Utilities::DataHandler::read_u32( reinterpret_cast<uint8_t*>(data_buffer + 24),
                                                                      default_settings.is_opposite_endian ) );

                            resource_pool.back().header_p = new Utilities::Buffer( reinterpret_cast<uint8_t*>(data_buffer + 28), data_size - 28 );
                        }
                        else
                            assert( 0 );
                    }
                    else
                    if( data_size >= 12 && !resource_pool.empty() && Utilities::DataHandler::read_u32( reinterpret_cast<uint8_t*>(data_buffer + 8), default_settings.is_opposite_endian ) == SDAT_TAG ) {
                        resource_pool.back().data_p->add( reinterpret_cast<uint8_t*>(data_buffer + 12), data_size - 12 );
                    }
                    else
                        std::cout << "This SHOC chunk is either too small or has an invalid tag." << std::endl;

                    // std::cout << "typeID: " << "SHOC" << " chunk_size: " << chunk_size << std::endl;
                }
                else
                if( typeID == MSIC_TAG ) {
                    //std::cout << "typeID: " << "MISC" << " chunk_size: " << chunk_size << std::endl;
                    if( msic_p == nullptr ) {
                        msic_p = new MSICResource();
                        msic_p->setIndexNumber( 0 );
                        msic_p->setOffset( file_offset );
                        
                        msic_data_p = new Utilities::Buffer();
                    }

                    if( msic_p != nullptr )
                        msic_data_p->add( reinterpret_cast<uint8_t*>(data_buffer), data_size );
                }
                else
                if( typeID == SWVR_TAG ) {
                    //std::cout << "typeID: " << "SWVR" << " chunk_size: " << chunk_size << std::endl;
                }
                else
                    std::cout << "typeID: 0x" << std::hex << typeID << std::dec << " chunk_size: " << chunk_size << std::endl;
            }
            else
                error_in_read = true;
        }
        while( file && !error_in_read );

        // Find a potential error.
        auto readstate = file.rdstate();

        if( errno != 0 )
        {
            if( ( readstate & std::ifstream::failbit ) != 0 ) {
                std::cout << "There is a logical error detected with the reading of the IFF or Mission File. Please report this to the developer!" << std::endl;
                std::cout << "Error: " << std::strerror(errno) << std::endl;
            }
            if( ( readstate & std::ifstream::badbit ) != 0 ) {
                std::cout << "There is a input error detected with the reading of the IFF or Mission File." << std::endl;
            }
        }

        // We are done reading the IFF file.
        file.close();

        if( !is_confirmend_iff_file ) {
            std::cout << "This file is not a little endian iff file or big endian iff file." << std::endl;
        }

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

            new_resource_p->setMemory( i.header_p, i.data_p );
            new_resource_p->processHeader( default_settings );
            new_resource_p->parse( default_settings );
            
            // TODO Add option to discard memory once loaded.
            // new_resource_p->setMemory( nullptr, nullptr );
            
            i.header_p = nullptr;
            i.data_p   = nullptr;

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
                std::cerr << "Error: PYR resource is found, but there are no Til resources." << std::endl;
            }
        }
        else
        if( getResource( Data::Mission::TilResource::IDENTIFIER_TAG ) != nullptr )
            std::cerr << "Error: PYR resource is not found, but the Til resources are in the file." << std::endl;

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

int Data::Mission::IFF::exportAllResources( const char *const folder_path, bool raw_file_mode, const std::vector<std::string>& arguments ) const {
    // This algorithm is an O(n) algorithm and it is as good as it is going to get in terms of data complexity. :)
    if( resource_amount != 0 )
    {
        std::string path = folder_path;

        if( path.back() != '/' )
            path += '/';

        // For every resource type categories in the Mission file.
        for( auto map_it = resource_map.begin(); map_it != resource_map.end(); map_it++ )
        {
            for( auto it = map_it->second.begin(); it != map_it->second.end(); it++ )
            {
                std::string full_path = path + (*it)->getFullName( (*it)->getIndexNumber() );

                if( raw_file_mode )
                    (*it)->write( full_path.c_str(), arguments );
                else
                    (*it)->writeRaw( full_path.c_str(), arguments );
            }
        }
        return true;
    }
    else
        return false;
}

int Data::Mission::IFF::exportSingleResource( uint32_t type, unsigned int index, const char *const folder_path, bool raw_file_mode, const std::vector<std::string> & arguments ) const {
    const Resource *resource_r = getResource( type, index );

    if( resource_r != nullptr )
    {
        std::string path = folder_path;

        if( path.back() != '/' )
            path += '/';

        path += resource_r->getFullName( index );

        if( raw_file_mode )
            return resource_r->write( path.c_str(), arguments );
        else
            return resource_r->writeRaw( path.c_str(), arguments );
    }
    else
        return -5; // Tell the user that there is either an invalid type or the index is out of bounds.
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
