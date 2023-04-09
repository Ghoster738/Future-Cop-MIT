#include "DataHandler.h"

#include "DataTypes.h"

bool Utilities::DataHandler::isLittleEndian() {
    bool is_little_endian;

    // Thanks David Cournapeau!
    union {
        uint32_t i;
        uint8_t  c[4];
    } bint = {0x01020304};

    is_little_endian = !(bint.c[0] == 1);

    return is_little_endian;
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

uint64_t Utilities::DataHandler::read_u64( const uint8_t *const word, bool swap ) {
    uint64_t word_read = *reinterpret_cast<const uint64_t *const>(word);

    if( swap )
        swapBytes( reinterpret_cast<uint8_t*>( &word_read ), sizeof( uint64_t ) );

    return word_read;
}
int64_t Utilities::DataHandler::read_64( const uint8_t *const word, bool swap ) {
    return static_cast<int64_t>( read_u64( word, swap ) );
}

uint32_t Utilities::DataHandler::read_u32( const uint8_t *const word, bool swap ) {
    uint32_t word_read = *reinterpret_cast<const uint32_t *const>(word);

    if( swap )
        swapBytes( reinterpret_cast<uint8_t*>( &word_read ), sizeof( uint32_t ) );

    return word_read;
}
int32_t Utilities::DataHandler::read_32( const uint8_t *const word, bool swap ) {
    return static_cast<int32_t>( read_u32( word, swap ) );
}

uint16_t Utilities::DataHandler::read_u16( const uint8_t *const word, bool swap ) {
    uint16_t word_read = *reinterpret_cast<const uint16_t *const>(word);

    if( swap )
        swapBytes( reinterpret_cast<uint8_t*>( &word_read ), sizeof( uint16_t ) );

    return word_read;
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
