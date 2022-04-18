#include "Resource.h"

#include <fstream>
#include <iostream>

const Data::Mission::Resource::ParseSettings Data::Mission::Resource::DEFAULT_PARSE_SETTINGS = Data::Mission::Resource::ParseSettings();

Data::Mission::Resource::ParseSettings::ParseSettings() {
    type = Unidentified; // We do not know what is being loaded.
    is_opposite_endian = false; // Do not switch endianess mode by default.
    output_level = 0;
    output_ref = &std::cout; // The console is the default output.
}

Data::Mission::Resource::Resource() : header_p( nullptr ), data_p( nullptr ), mis_index_number( -1 ), index_number( -1 ), offset( 0 ) {

}

Data::Mission::Resource::Resource( const Data::Mission::Resource &obj ) : header_p( nullptr ), data_p( nullptr ), mis_index_number( obj.mis_index_number ), index_number( obj.index_number ), offset( obj.offset ) {
    if( obj.header_p != nullptr )
        header_p = new Utilities::Buffer( *obj.header_p );
    
    if( obj.data_p != nullptr )
        data_p = new Utilities::Buffer( *obj.data_p );
}

Data::Mission::Resource::~Resource() {
    if( header_p != nullptr )
        delete header_p;
    
    if( data_p != nullptr )
        delete data_p;
}

void Data::Mission::Resource::setIndexNumber( int index_number ) {
    this->index_number = index_number;
}

int Data::Mission::Resource::getIndexNumber() const {
    return index_number;
}

void Data::Mission::Resource::setMisIndexNumber( int mis_index_number ) {
    this->mis_index_number = mis_index_number;
}

int Data::Mission::Resource::getMisIndexNumber() const {
    return this->mis_index_number;

}

void Data::Mission::Resource::setOffset( size_t offset ) {
    this->offset = offset;
}

size_t Data::Mission::Resource::getOffset() const {
    return offset;
}

std::string Data::Mission::Resource::getFullName( unsigned int index ) const {
    std::string full_name;

    // full_name = "dat_60"
    full_name  = getFileExtension();
    full_name += "_";
    full_name += std::to_string( index );

    return full_name;
}

Data::Mission::Resource* Data::Mission::Resource::genResourceByType( const Utilities::Buffer &header, const Utilities::Buffer &data ) const {
    return duplicate();
}

void Data::Mission::Resource::setMemory( Utilities::Buffer *header_p, Utilities::Buffer *data_p ) {
    // Do not do anything if the pointers are the same.
    if( this->header_p != header_p )
    {
        // Delete the class's header and set it to null
        if( this->header_p != nullptr )
            delete this->header_p;
        this->header_p = nullptr;
        
        // Set header with a new pointer with a copy of the parameter's header_p contents.
        if( header_p != nullptr )
            this->header_p = new Utilities::Buffer( *header_p );
    }
    
    if( this->data_p != data_p )
    {
        if( this->data_p != nullptr )
            delete this->data_p;
        this->data_p = nullptr;
        
        if( data_p != nullptr )
            this->data_p = new Utilities::Buffer( *data_p );
    }
}

int Data::Mission::Resource::read( const char *const file_path ) {
    std::ifstream resource;

    resource.open(file_path, std::ios::binary | std::ios::in | std::ios::ate );

    if( resource.is_open() ) // Make sure that the file is opened.
    {
        size_t size = resource.tellg();

        // Return to the beginning.
        resource.seekg( 0, std::ios::beg );
        
        if( size > 0 )
            setMemory( nullptr, nullptr );
        
        data_p = new Utilities::Buffer();

        // To make these operations faster expand the raw_data, so it does not have to reallocate data.
        data_p->reserve( size );

        // This will store the byte.
        char byte = 0;

        for( size_t i = 0; i < size; i++ )
        {
            resource.get( byte );

            data_p->addI8( byte );
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

int Data::Mission::Resource::read( const std::string &file_path ) {
    return read( file_path.c_str() );
}

int Data::Mission::Resource::write( const char *const file_path, const std::vector<std::string> & arguments  ) const {
    return -1;
}

int Data::Mission::Resource::writeRaw( const char *const file_path, const std::vector<std::string> & arguments ) const {
    std::ofstream resource;

    resource.open( std::string(file_path) + "." + getFileExtension(), std::ios::binary | std::ios::out );

    if( resource.is_open() && data_p != nullptr )
    {
        auto reader = data_p->getReader();
        
        while( !reader.empty() )
            resource.put( reader.readI8() );

        resource.close();

        return 1;
    }
    else
    {
        return 0;
    }
}

bool Data::Mission::Resource::operator() ( const Data::Mission::Resource& l_operand, const Data::Mission::Resource& r_operand ) {
    return (l_operand < r_operand);
}

bool Data::Mission::operator == ( const Data::Mission::Resource& l_operand, const Data::Mission::Resource& r_operand ) {
    if( l_operand.getResourceTagID() != r_operand.getResourceTagID() )
        return false;
    else // Same buffer means the same file.
    if( l_operand.data_p == r_operand.data_p  )
        return true;
    else // If one of the buffers are null then they are not equal.
    if( l_operand.data_p == nullptr || r_operand.data_p == nullptr )
        return false;
    else // Check the size.
    if( l_operand.data_p->getReader().totalSize() != r_operand.data_p->getReader().totalSize() ) // Then check the size
        return false;
    else
    {
        // Since they are both pointers that are not null they are
        // used as readers.
        auto l_reader = l_operand.data_p->getReader();
        auto r_reader = r_operand.data_p->getReader();
        
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

bool Data::Mission::operator != ( const Data::Mission::Resource& l_operand, const Data::Mission::Resource& r_operand ) {
    return !( l_operand == r_operand );
}

bool Data::Mission::operator < ( const Data::Mission::Resource& l_operand, const Data::Mission::Resource& r_operand ) {
    if( l_operand.getResourceTagID() < r_operand.getResourceTagID() )
        return true;
    else
    if( l_operand.getResourceTagID() > r_operand.getResourceTagID() )
        return false;
    else // If l_operand's and r_operand's buffer are the same buffer then they are not greater than each other.
    if( l_operand.data_p == r_operand.data_p )
        return false;
    else // If l_operand has null it gets treated as 0, and r_operand gets treated as a one.
    if( l_operand.data_p == nullptr )
        return true; // 0 < 1
    else // If l_operand has null it gets treated as 0, and r_operand gets treated as a one.
    if( r_operand.data_p == nullptr )
        return false; // 0 !< 1
    else
    if( l_operand.data_p->getReader().totalSize() < r_operand.data_p->getReader().totalSize() ) // First check the size
        return true;
    else
    if( l_operand.data_p->getReader().totalSize() > r_operand.data_p->getReader().totalSize() )
        return false;
    else { // l_operand.buffer_p->totalSize() == r_operand.buffer_p->totalSize()
        auto l_reader = l_operand.data_p->getReader();
        auto r_reader = r_operand.data_p->getReader();
        
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

bool Data::Mission::operator > ( const Data::Mission::Resource& l_operand, const Data::Mission::Resource& r_operand ) {
    return ( r_operand < l_operand );
}

bool Data::Mission::operator <= ( const Data::Mission::Resource& l_operand, const Data::Mission::Resource& r_operand ) {
    return !( l_operand > r_operand );
}

bool Data::Mission::operator >= ( const Data::Mission::Resource& l_operand, const Data::Mission::Resource& r_operand ) {
    return !( l_operand < r_operand );
}

