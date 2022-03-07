#include "DataHandler.h"

#include "DataTypes.h"

namespace {
    bool swapLittleEndian;
    bool swapBigEndian;
}

void Utilities::DataHandler::init() {
    // Thanks David Cournapeau!
    union {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};

    swapBigEndian = !(bint.c[0] == 1);
    swapLittleEndian = !swapBigEndian;
}

bool Utilities::DataHandler::is_little_endian() {
    // If little endian numbers are not to be swapped then this machine is little endian.
    // Or the cpu was set at big endian at the time of init.
    return !swapLittleEndian;
}

void Utilities::DataHandler::swapBytes( uint8_t *word, unsigned int size ) {
    uint8_t store;
    for( unsigned int i = 0; i < size / 2; i++ )
    {
        store = word[i];
        word[i] = word[ size - 1 - i ];
        word[ size - 1 - i ] = store;
    }
}


uint64_t Utilities::DataHandler::read_u64_little( const uint8_t *const word ) {
    return read_u64( word, swapLittleEndian);
}

uint64_t Utilities::DataHandler::read_u64_big( const uint8_t *const word ) {
    return read_u64( word, swapBigEndian);
}

uint64_t Utilities::DataHandler::read_u64( const uint8_t *const word, bool swap ) {
    uint64_t word_read = *reinterpret_cast<const uint64_t *const>(word);

    if( swap )
        swapBytes( reinterpret_cast<uint8_t*>( &word_read ), sizeof( uint64_t ) );

    return word_read;
}

int64_t Utilities::DataHandler::read_64_little( const uint8_t *const word ) {
    return static_cast<int64_t>( read_u64_little( word ) );
}

int64_t Utilities::DataHandler::read_64_big( const uint8_t *const word ) {
    return static_cast<int64_t>( read_u64_big( word ) );
}

int64_t Utilities::DataHandler::read_64( const uint8_t *const word, bool swap ) {
    return static_cast<int64_t>( read_u64( word, swap ) );
}


uint32_t Utilities::DataHandler::read_u32_little( const uint8_t *const word ) {
    return read_u32( word, swapLittleEndian );
}

uint32_t Utilities::DataHandler::read_u32_big( const uint8_t *const word ) {
    return read_u32( word, swapBigEndian );
}

uint32_t Utilities::DataHandler::read_u32( const uint8_t *const word, bool swap ) {
    uint32_t word_read = *reinterpret_cast<const uint32_t *const>(word);

    if( swap )
        swapBytes( reinterpret_cast<uint8_t*>( &word_read ), sizeof( uint32_t ) );

    return word_read;
}

int32_t Utilities::DataHandler::read_32_little( const uint8_t *const word ) {
    return static_cast<int32_t>( read_u32_little( word ) );
}

int32_t Utilities::DataHandler::read_32_big( const uint8_t *const word ) {
    return static_cast<int32_t>( read_u32_big( word ) );
}

int32_t Utilities::DataHandler::read_32( const uint8_t *const word, bool swap ) {
    return static_cast<int32_t>( read_u32( word, swap ) );
}


uint16_t Utilities::DataHandler::read_u16_little( const uint8_t *const word ) {
    return read_u16( word, swapLittleEndian );
}

uint16_t Utilities::DataHandler::read_u16_big( const uint8_t *const word ) {
    return read_u16( word, swapBigEndian );
}

uint16_t Utilities::DataHandler::read_u16( const uint8_t *const word, bool swap ) {
    uint16_t word_read = *reinterpret_cast<const uint16_t *const>(word);

    if( swap )
        swapBytes( reinterpret_cast<uint8_t*>( &word_read ), sizeof( uint16_t ) );

    return word_read;
}

int16_t Utilities::DataHandler::read_16_little( const uint8_t *const word ) {
    return static_cast<int16_t>( read_u16_little( word ) );
}

int16_t Utilities::DataHandler::read_16_big( const uint8_t *const word ) {
    return static_cast<int16_t>( read_u16_big( word ) );
}

int16_t Utilities::DataHandler::read_16( const uint8_t *const word, bool swap ) {
    return static_cast<int16_t>( read_u16( word, swap ) );
}


uint8_t Utilities::DataHandler::read_u8( const uint8_t *const word ) {
    uint8_t word_read = *reinterpret_cast<const uint8_t *const>(word);

    return word_read;
}

int8_t Utilities::DataHandler::read_8( const uint8_t *const word) {
    return static_cast<int8_t>( read_u8( word ) );
}
