#include "Buffer.h"
#include "DataHandler.h"

#include <sstream>
#include <fstream>

Utilities::Buffer::Buffer() {}

Utilities::Buffer::Buffer( const uint8_t *const buffer_r, size_t byte_amount ) {
    set( buffer_r, byte_amount );
}

Utilities::Buffer::Buffer( const Buffer &buffer ) {
    set( buffer.data.data(), buffer.data.size() );
}

Utilities::Buffer::~Buffer() {}

void Utilities::Buffer::reserve( size_t byte_amount ) {
    data.reserve( byte_amount );
}

bool Utilities::Buffer::allocate( size_t byte_amount ) {
    reserve( byte_amount );
    for( size_t i = 0; i < byte_amount; i++ )
        addU8( 0 );
    return true;
}

bool Utilities::Buffer::add( const uint8_t *const buffer, size_t byte_amount ) {
    reserve( byte_amount + data.size() );

    for( size_t i = 0; i < byte_amount; i++ )
        data.push_back( buffer[ i ] );

    return true;
}

bool Utilities::Buffer::set( const uint8_t *const buffer, size_t byte_amount ) {
    data.clear();

    return add( buffer, byte_amount );
}

bool Utilities::Buffer::addU8( uint8_t value ) {
    data.push_back( value );
    return true;
}

bool Utilities::Buffer::addI8( int8_t value ) {
    data.push_back( *reinterpret_cast<uint8_t*>(&value) );
    return true;
}

bool Utilities::Buffer::addU16( uint16_t value, Endian endianess ) {
    uint16_t store = value;
    
    if( Reader::getSwap( endianess ) )
        DataHandler::swapBytes( reinterpret_cast<uint8_t*>(&store), sizeof(uint16_t) );
    
    auto array = reinterpret_cast<uint8_t*>( &store );
    
    data.push_back( array[0] );
    data.push_back( array[1] );
    return true;
}

bool Utilities::Buffer::addI16( int16_t value, Endian endianess ) {
    int16_t store = value;
    
    if( Reader::getSwap( endianess ) )
        DataHandler::swapBytes( reinterpret_cast<uint8_t*>(&store), sizeof(uint16_t) );
    
    auto array = reinterpret_cast<uint8_t*>( &store );
    
    data.push_back( array[0] );
    data.push_back( array[1] );
    return true;
}

bool Utilities::Buffer::addU32( uint32_t value, Endian endianess ) {
    uint32_t store = value;
    
    if( Reader::getSwap( endianess ) )
        DataHandler::swapBytes( reinterpret_cast<uint8_t*>(&store), sizeof(uint32_t) );
    
    auto array = reinterpret_cast<uint8_t*>( &store );
    
    data.push_back( array[0] );
    data.push_back( array[1] );
    data.push_back( array[2] );
    data.push_back( array[3] );
    return true;
}

bool Utilities::Buffer::addI32( int32_t value, Endian endianess ) {
    int32_t store = value;
    
    if( Reader::getSwap( endianess ) )
        DataHandler::swapBytes( reinterpret_cast<uint8_t*>(&store), sizeof(uint32_t) );
    
    auto array = reinterpret_cast<uint8_t*>( &store );
    
    data.push_back( array[0] );
    data.push_back( array[1] );
    data.push_back( array[2] );
    data.push_back( array[3] );
    return true;
}

bool Utilities::Buffer::addU64( uint64_t value, Endian endianess ) {
    uint64_t store = value;
    
    if( Reader::getSwap( endianess ) )
        DataHandler::swapBytes( reinterpret_cast<uint8_t*>(&store), sizeof(uint64_t) );
    
    auto array = reinterpret_cast<uint8_t*>( &store );
    
    data.push_back( array[0] );
    data.push_back( array[1] );
    data.push_back( array[2] );
    data.push_back( array[3] );
    data.push_back( array[4] );
    data.push_back( array[5] );
    data.push_back( array[6] );
    data.push_back( array[7] );
    return true;
}

bool Utilities::Buffer::addI64(  int64_t value, Endian endianess ) {
    int64_t store = value;
    
    if( Reader::getSwap( endianess ) )
        DataHandler::swapBytes( reinterpret_cast<uint8_t*>(&store), sizeof(uint64_t) );
    
    auto array = reinterpret_cast<uint8_t*>( &store );
    
    data.push_back( array[0] );
    data.push_back( array[1] );
    data.push_back( array[2] );
    data.push_back( array[3] );
    data.push_back( array[4] );
    data.push_back( array[5] );
    data.push_back( array[6] );
    data.push_back( array[7] );
    return true;
}

bool Utilities::Buffer::write( const std::string& file_path ) const {
    std::ofstream output;
    
    output.open( file_path, std::ios::binary | std::ios::out );
    
    if( output.is_open() )
    {
        for( const auto &byte : data )
            output.put( byte );
        
        output.close();
        
        return true;
    }
    else
        return false;
}

uint8_t* Utilities::Buffer::dangerousPointer() {
    return data.data();
}

const uint8_t *const Utilities::Buffer::dangerousPointer() const {
    return data.data();
}

Utilities::Buffer::Reader Utilities::Buffer::getReader( size_t offset, size_t byte_amount ) const {
    if( byte_amount == 0 )
        byte_amount = data.size() - offset;

    size_t offset_sum = offset + byte_amount;

    if( byte_amount <= 0 )
        return Reader( nullptr, 0 ); // If the byte_amount is 0 or lesser then return nullptr.
    else
    {
        offset_sum = offset + byte_amount;

        if( offset_sum < byte_amount )
            return Reader( nullptr, 0 ); // If offset_sum has been overflowed return nullptr.
        else
        if( offset_sum > data.size() )
            return Reader( nullptr, 0 ); // If the requested data is too much then return nullptr;
        else
            return Reader( data.data() + offset, byte_amount );
    }
}

Utilities::Buffer::ReaderOutOfBounds::ReaderOutOfBounds( const char *const method_name_r, const uint8_t *const data_r, size_t byte_amount, size_t current_index ) {
    std::stringstream error_out;

    error_out << "Utilities::Buffer overflowed using \"";
    error_out << method_name_r;
    error_out << "\" reader using pointer ";
    error_out << data_r;
    error_out << " with ";
    error_out << byte_amount;
    error_out << " byte(s) at index ";
    error_out << current_index << "\n";

    what_is_wrong = error_out.str();
}

const char* Utilities::Buffer::ReaderOutOfBounds::what() const throw() {
    return what_is_wrong.c_str();
}

bool Utilities::Buffer::Reader::getSwap( Endian endianess ) {
    bool swap_value;

    switch( endianess ) {
        case SWAP:
            swap_value = true;
            break;
        case LITTLE:
            swap_value = !Utilities::DataHandler::is_little_endian();
            break;
        case BIG:
            swap_value = Utilities::DataHandler::is_little_endian();
            break;
        case NO_SWAP:
        default:
            swap_value = false;
    }

    return swap_value;
}

Utilities::Buffer::Reader::Reader( const uint8_t *const buffer_r, size_t byte_amount ) : data_r( buffer_r ), size( byte_amount ), current_index( 0 ) {}

Utilities::Buffer::Reader::~Reader() {
}

bool Utilities::Buffer::Reader::ended() const {
    return size == current_index;
}

bool Utilities::Buffer::Reader::empty() const {
    return (data_r == nullptr) | (size == 0);
}

size_t Utilities::Buffer::Reader::totalSize() const {
    return size;
}

size_t Utilities::Buffer::Reader::getPosition( Direction way ) const {
    if( way == END )
        return size - current_index;
    else
        return current_index;
}

void Utilities::Buffer::Reader::setPosition( int offset, Direction way ) {
    int new_offset;

    switch( way ) {
        case BEGIN:
            new_offset = 0 + offset;
            break;
        case CURRENT:
            new_offset = static_cast<int>( current_index ) + offset;
            break;
        case END:
            new_offset = static_cast<int>( size ) - offset;
            break;
        default:
            new_offset = -1;
    }

    current_index = new_offset;

    if( new_offset < 0 || static_cast<size_t>( new_offset ) > size )
        throw ReaderOutOfBounds( "setPosition", data_r, size, current_index );

}

std::vector<uint8_t> Utilities::Buffer::Reader::getBytes( size_t byte_amount, Endian endianess ) {
    if( byte_amount == 0 )
        byte_amount = size - current_index;

    std::vector<uint8_t> buffer;

    if( byte_amount <= 0 || current_index == size )
        return buffer;
    else {
        bool swap_value = getSwap( endianess );
        size_t new_offset;

        new_offset = current_index + byte_amount;

        if( new_offset < current_index )
            throw ReaderOutOfBounds( "getBytes", data_r, size, current_index );
        else
        if( new_offset > size )
            throw ReaderOutOfBounds( "getBytes", data_r, size, current_index );
        else {
            buffer.reserve( byte_amount );

            if( swap_value ) {
                for( auto i = new_offset; i != current_index; i-- ) {
                    buffer.push_back( data_r[ i - 1 ] );
                }
            }
            else {
                for( auto i = current_index; i < new_offset; i++ ) {
                    buffer.push_back( data_r[ i ] );
                }
            }

            current_index = new_offset;

            return buffer;
        }
    }
}

Utilities::Buffer::Reader Utilities::Buffer::Reader::getReader( size_t reader_size ) {
    if( current_index + reader_size > size )
        throw ReaderOutOfBounds( "getReader", data_r, size, current_index );
    else
    {
        auto reader = Reader( this->data_r + current_index, reader_size );

        current_index += reader_size;

        return reader;
    }
}

uint64_t Utilities::Buffer::Reader::readU64( Endian endianess ) {
    size_t new_offset = current_index + sizeof( uint64_t );

    if( new_offset < current_index )
        throw ReaderOutOfBounds( "readU64", data_r, size, current_index );
    else
    if( new_offset > size )
        throw ReaderOutOfBounds( "readU64", data_r, size, current_index );
    else
    {
        current_index = new_offset;
        return Utilities::DataHandler::read_u64( data_r + current_index - sizeof( uint64_t ), getSwap( endianess ) );
    }
}

int64_t Utilities::Buffer::Reader::readI64( Endian endianess ) {
    size_t new_offset = current_index + sizeof( int64_t );

    if( new_offset < current_index )
        throw ReaderOutOfBounds( "readI64", data_r, size, current_index );
    else
    if( new_offset > size )
        throw ReaderOutOfBounds( "readI64", data_r, size, current_index );
    else
    {
        current_index = new_offset;
        return Utilities::DataHandler::read_64( data_r + current_index - sizeof( uint64_t ), getSwap( endianess ) );
    }
}

uint32_t Utilities::Buffer::Reader::readU32( Endian endianess ) {
    size_t new_offset = current_index + sizeof( uint32_t );

    if( new_offset < current_index )
        throw ReaderOutOfBounds( "readU32", data_r, size, current_index );
    else
    if( new_offset > size )
        throw ReaderOutOfBounds( "readU32", data_r, size, current_index );
    else
    {
        current_index = new_offset;
        return Utilities::DataHandler::read_u32( data_r + current_index - sizeof( uint32_t ), getSwap( endianess ) );
    }
}

int32_t Utilities::Buffer::Reader::readI32( Endian endianess ) {
    size_t new_offset = current_index + sizeof( int32_t );

    if( new_offset < current_index )
        throw ReaderOutOfBounds( "readI32", data_r, size, current_index );
    else
    if( new_offset > size )
        throw ReaderOutOfBounds( "readI32", data_r, size, current_index );
    else
    {
        current_index = new_offset;
        return Utilities::DataHandler::read_32( data_r + current_index - sizeof( uint32_t ), getSwap( endianess ) );
    }
}

uint16_t Utilities::Buffer::Reader::readU16( Endian endianess ) {
    size_t new_offset = current_index + sizeof( uint16_t );

    if( new_offset < current_index )
        throw ReaderOutOfBounds( "readU16", data_r, size, current_index );
    else
    if( new_offset > size )
        throw ReaderOutOfBounds( "readU16", data_r, size, current_index );
    else
    {
        current_index = new_offset;
        return Utilities::DataHandler::read_u16( data_r + current_index - sizeof( uint16_t ), getSwap( endianess ) );
    }
}

int16_t Utilities::Buffer::Reader::readI16( Endian endianess ) {
    size_t new_offset = current_index + sizeof( int16_t );

    if( new_offset < current_index )
        throw ReaderOutOfBounds( "readI16", data_r, size, current_index );
    else
    if( new_offset > size )
        throw ReaderOutOfBounds( "readI16", data_r, size, current_index );
    else
    {
        current_index = new_offset;
        return Utilities::DataHandler::read_16( data_r + current_index - sizeof( uint16_t ), getSwap( endianess ) );
    }
}

uint8_t Utilities::Buffer::Reader::readU8() {
    size_t new_offset = current_index + sizeof( uint8_t );

    if( new_offset < current_index )
        throw ReaderOutOfBounds( "readU8", data_r, size, current_index );
    else
    if( new_offset > size )
        throw ReaderOutOfBounds( "readU8", data_r, size, current_index );
    else
    {
        current_index = new_offset;
        return Utilities::DataHandler::read_u8( data_r + current_index - sizeof( uint8_t ) );
    }
}

int8_t Utilities::Buffer::Reader::readI8() {
    size_t new_offset = current_index + sizeof( int8_t );

    if( new_offset < current_index )
        throw ReaderOutOfBounds( "readI8", data_r, size, current_index );
    else
    if( new_offset > size )
        throw ReaderOutOfBounds( "readI8", data_r, size, current_index );
    else
    {
        current_index = new_offset;
        return Utilities::DataHandler::read_8( data_r + current_index - sizeof( uint8_t ) );
    }
}
