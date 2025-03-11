#include "Resource.h"

#include <cassert>
#include <fstream>

namespace Data {
namespace Mission {

const Resource::ParseSettings Resource::DEFAULT_PARSE_SETTINGS = Resource::ParseSettings();

Resource::ParseSettings::ParseSettings() :
    endian( Utilities::Buffer::Endian::NO_SWAP ),
    logger_r( &Utilities::logger ) {
}

Utilities::Buffer::Reader Resource::getDataReader() const {
    return this->data->getReader( this->header_size );
}

Resource::Resource() : mis_index_number( -1 ), index_number( -1 ), offset( 0 ), resource_id( 0 ), rpns_offsets{0, 0, 0}, code_sizes{0, 0}, header_size(0), data(nullptr), swvr_entry() {
}

Resource::Resource( const Resource &obj ) :
    mis_index_number( obj.mis_index_number ), index_number( obj.index_number ), offset( obj.offset ), resource_id( obj.resource_id ), rpns_offsets{obj.rpns_offsets[0],
    obj.rpns_offsets[1], obj.rpns_offsets[2]}, code_sizes{obj.code_sizes[0], obj.code_sizes[1]}, swvr_entry( obj.swvr_entry ),
    header_size(obj.header_size), data(nullptr) {

    if(obj.data != nullptr) {
        data = std::make_unique<Utilities::Buffer>(*obj.data);
    }
}

Resource::~Resource() {}

uint32_t Resource::getResourceID() const {
    if( !noResourceID() )
        return resource_id;
    
    return resource_id + getIndexNumber();
}

bool Resource::noResourceID() const {
    return false;
}

uint32_t Resource::getRPNSOffset( unsigned index ) const {
    assert( index < RPNS_OFFSET_AMOUNT );

    return this->rpns_offsets[index];
}

void Resource::setRPNSOffset( unsigned index, uint32_t value ) {
    assert( index < RPNS_OFFSET_AMOUNT );

    this->rpns_offsets[index] = value;
}

uint32_t Resource::getCodeAmount( unsigned index ) const {
    assert( index < CODE_AMOUNT );

    return this->code_sizes[index];
}

void Resource::setCodeAmount( unsigned index, uint32_t amount ) {
    assert( index < CODE_AMOUNT );

    this->code_sizes[index] = amount;
}

std::string Resource::getFullName( unsigned int index ) const {
    std::string full_name = getFileExtension();
    full_name += "_";
    
    if( !swvr_entry.isPresent() )
        full_name += std::to_string( index );
    else
        full_name += swvr_entry.name;
    
    // full_name = "dat_60"

    return full_name;
}

Resource* Resource::genResourceByType( const Utilities::Buffer::Reader &data ) const {
    return duplicate();
}

void Resource::setMemory( Utilities::Buffer *data_p ) {
    this->data = std::unique_ptr<Utilities::Buffer>(data_p);
}

int Resource::read( const std::string &file_path ) {
    std::ifstream resource;

    resource.open(file_path, std::ios::binary | std::ios::in | std::ios::ate );

    if( resource.is_open() ) // Make sure that the file is opened.
    {
        size_t size = resource.tellg();

        // Return to the beginning.
        resource.seekg( 0, std::ios::beg );
        
        this->data = std::make_unique<Utilities::Buffer>();

        // TODO make these operations faster expand the raw_data, so it does not have to reallocate data.
        this->data->reserve( size );

        // This will store the byte.
        char byte = 0;

        for( size_t i = 0; i < size; i++ )
        {
            resource.get( byte );

            this->data->addI8( byte );
        }

        // The resource is complety read into memory.
        resource.close();

        return 1;
    }
    else
    {
        return 0;
    }
}

int Resource::read( Utilities::Buffer::Reader& reader ) {
    if( reader.empty() ) // Do nothing if the reader is empty.
        return 0;
    else
    if( reader.ended() ) // Do nothing if the reader has ended.
        return -1;
    else {
        // Make sure that there is a buffer to write to.
        this->data = std::make_unique<Utilities::Buffer>();
        
        // Add all the information that the reader has to this resource.
        while( !reader.ended() ) {
            this->data->addU8( reader.readU8() );
        }

        return 1; // Successfully read the resource.
    }
}

int Resource::write( const std::string& file_path, const IFFOptions &iff_options  ) const {
    return -1;
}

int Resource::writeRaw( const std::string& file_path, const IFFOptions &iff_options ) const {
    std::ofstream resource;

    resource.open( file_path + "." + getFileExtension(), std::ios::binary | std::ios::out );

    if( resource.is_open() && this->data != nullptr )
    {
        auto reader = getDataReader();
        
        while( !reader.ended() )
            resource.put( reader.readI8() );

        resource.close();

        return 1;
    }
    else
    {
        return 0;
    }
}

bool Resource::operator() ( const Resource& l_operand, const Resource& r_operand ) {
    return (l_operand < r_operand);
}

bool operator == ( const Resource& l_operand, const Resource& r_operand ) {
    if( l_operand.getResourceTagID() != r_operand.getResourceTagID() )
        return false;
    else // Same buffer means the same file.
    if( l_operand.data == r_operand.data  )
        return true;
    else // If one of the buffers are null then they are not equal.
    if( l_operand.data == nullptr || r_operand.data == nullptr )
        return false;
    else // Check the size.
    if( l_operand.data->getReader().totalSize() != r_operand.data->getReader().totalSize() ) // Then check the size
        return false;
    else
    {
        // Since they are both pointers that are not null they are
        // used as readers.
        auto l_reader = l_operand.data->getReader();
        auto r_reader = r_operand.data->getReader();
        
        // Check byte by byte to see if these files in fact match.
        // Since they are both equal in size only one can be checked for emptyness.
        while( !l_reader.empty() ) {
            if( l_reader.readU8() != r_reader.readU8() )
                return false;
        }
        // Thus the l_operand and r_operand are the same.
        return true;
    }
}

bool operator != ( const Resource& l_operand, const Resource& r_operand ) {
    return !( l_operand == r_operand );
}

bool operator < ( const Resource& l_operand, const Resource& r_operand ) {
    if( l_operand.getResourceTagID() < r_operand.getResourceTagID() )
        return true;
    else
    if( l_operand.getResourceTagID() > r_operand.getResourceTagID() )
        return false;
    else // If l_operand's and r_operand's buffer are the same buffer then they are not greater than each other.
    if( l_operand.data == r_operand.data )
        return false;
    else // If l_operand has null it gets treated as 0, and r_operand gets treated as a one.
    if( l_operand.data == nullptr )
        return true; // 0 < 1
    else // If l_operand has null it gets treated as 0, and r_operand gets treated as a one.
    if( r_operand.data == nullptr )
        return false; // 0 !< 1
    else
    if( l_operand.data->getReader().totalSize() < r_operand.data->getReader().totalSize() ) // First check the size
        return true;
    else
    if( l_operand.data->getReader().totalSize() > r_operand.data->getReader().totalSize() )
        return false;
    else { // l_operand.buffer_p->totalSize() == r_operand.buffer_p->totalSize()
        auto l_reader = l_operand.data->getReader();
        auto r_reader = r_operand.data->getReader();
        
        // Check byte by byte to see if l_operand is in fact less than r_operand.
        while( !l_reader.empty() && !r_reader.empty() ) {
            auto l_byte = l_reader.readU8();
            auto r_byte = l_reader.readU8();
            
            if( l_byte < r_byte ) {
                return true;
            }
            else
            if( l_byte > r_byte )
                return false;
            // else continue.
        }

        // l_operand therefore must be equal to r_operand, but not less than.
        return false;
    }
}

bool operator > ( const Resource& l_operand, const Resource& r_operand ) {
    return ( r_operand < l_operand );
}

bool operator <= ( const Resource& l_operand, const Resource& r_operand ) {
    return !( l_operand > r_operand );
}

bool operator >= ( const Resource& l_operand, const Resource& r_operand ) {
    return !( l_operand < r_operand );
}

}
}
