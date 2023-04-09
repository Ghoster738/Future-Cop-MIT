#ifndef DATA_HANDLER_HEADER
#define DATA_HANDLER_HEADER

#include <stdint.h>

namespace Utilities {

/**
 * This time there should not be a class, because this just involves in reading bytes.
 * This should be thread safe for the most part, because there is only one local varrible within it.
 */
namespace DataHandler {
    void swapBytes( uint8_t *word, unsigned int size );

    /**
     * Test to see if the cpu is little endian.
     */
    bool isLittleEndian();

    uint64_t read_u64( const uint8_t *const word, bool swap = false );
    int64_t read_64( const uint8_t *const word, bool swap = false );
    uint32_t read_u32( const uint8_t *const word, bool swap = false );
    int32_t read_32( const uint8_t *const word, bool swap = false );
    uint16_t read_u16( const uint8_t *const word, bool swap = false );
    int16_t read_16( const uint8_t *const word, bool swap = false );
    uint8_t read_u8( const uint8_t *const word );
    int8_t read_8( const uint8_t *const word);
};

};

#endif // DATA_HANDLER_HEADER
