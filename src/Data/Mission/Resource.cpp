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

Data::Mission::Resource::Resource() {

}

Data::Mission::Resource::Resource( const Data::Mission::Resource &obj ) : raw_header( obj.raw_header ), raw_data( obj.raw_data ), mis_index_number( obj.mis_index_number ), index_number( obj.index_number ), offset( obj.offset ) {
}

Data::Mission::Resource::~Resource() {

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

void Data::Mission::Resource::pruneRaws() {
    // Completely, get rid of the data that is not being used.
    raw_header.clear();
    raw_header.shrink_to_fit();
    raw_data.clear();
    raw_data.shrink_to_fit();
}

int Data::Mission::Resource::read( const char *const file_path ) {
    std::ifstream resource;

    resource.open(file_path, std::ios::binary | std::ios::in | std::ios::ate );

    if( resource.is_open() ) // Make sure that the file is opened.
    {
        size_t size = resource.tellg();

        // Return to the beginning.
        resource.seekg( 0, std::ios::beg );

        // To make these operations faster expand the raw_data, so it does not have to reallocate data.
        raw_data.reserve( size );

        // This will store the byte.
        char byte = 0;

        for( size_t i = 0; i < size; i++ )
        {
            resource.get( byte );

            raw_data.push_back( byte );
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

    if( resource.is_open() )
    {
        for( unsigned int i = 0; i < raw_data.size(); i++ )
        {
            resource.put( raw_data.at(i) );
        }

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
    else
    if( l_operand.raw_data.size() != r_operand.raw_data.size() ) // Then check the size
        return false;
    else
    {
        // Check byte by byte to see if these files in fact match.
        for( size_t i = 0; i < l_operand.raw_data.size(); i++ ) {
            if( l_operand.raw_data[i] != r_operand.raw_data[i] ) {
                return false;
            }
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
    else
    if( l_operand.raw_data.size() < r_operand.raw_data.size() ) // First check the size
        return true;
    else
    if( l_operand.raw_data.size() > r_operand.raw_data.size() )
        return false;
    else { // l_operand.raw_data.size() == r_operand.raw_data.size()
        // Check byte by byte to see if l_operand is in fact less than r_operand.
        for( size_t i = 0; i < l_operand.raw_data.size(); i++ ) {
            if( l_operand.raw_data[i] < r_operand.raw_data[i] ) {
                return true;
            }
            else
            if( l_operand.raw_data[i] > r_operand.raw_data[i] )
                return false;
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

