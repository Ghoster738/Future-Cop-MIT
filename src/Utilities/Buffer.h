#ifndef UTILITIES_BUFFER_938223_H
#define UTILITIES_BUFFER_938223_H

#include <exception>
#include <stdint.h>
#include <vector>
#include <string>

namespace Utilities {

class Buffer {
public:
    class Reader;
    
    enum Endian {
        NO_SWAP,
        SWAP,
        LITTLE,
        BIG
    };

protected:
    std::vector<uint8_t> data;

public:
    Buffer();
    Buffer( const uint8_t *const buffer_r, size_t byte_amount );
    virtual ~Buffer();

    void reserve( size_t byte_amount );
    bool add( const uint8_t *const buffer, size_t byte_amount );
    bool set( const uint8_t *const buffer, size_t byte_amount );

    Reader getReader( size_t offset = 0, size_t byte_amount = 0 ) const;
    
    bool addU8(   uint8_t value );
    bool addI8(    int8_t value );
    bool addU16( uint16_t value, Endian endianess = NO_SWAP );
    bool addI16(  int16_t value, Endian endianess = NO_SWAP );
    bool addU32( uint32_t value, Endian endianess = NO_SWAP );
    bool addI32(  int32_t value, Endian endianess = NO_SWAP );
    bool addU64( uint64_t value, Endian endianess = NO_SWAP );
    bool addI64(  int64_t value, Endian endianess = NO_SWAP );
    
    bool write( const std::string& file_path ) const;

public:
    class ReaderOutOfBounds: public std::exception {
    private:
        std::string what_is_wrong;
    public:
        ReaderOutOfBounds( const char *const method_name_r, const uint8_t *const data_r, size_t byte_amount, size_t current_index );

        virtual const char* what() const throw();
    };

    class Reader {
    protected:
        const uint8_t *const data_r;
        size_t size;

        size_t current_index;

    public:
        enum Direction {
            BEGINING,
            CURRENT,
            ENDING
        };

        static bool getSwap( Endian endianess );
    public:
        Reader( const uint8_t *const buffer_r, size_t byte_amount );
        virtual ~Reader();

        bool empty() const;
        bool ended() const;
        size_t totalSize() const;

        void setPosition( int offset, Direction way );

        std::vector<uint8_t> getBytes( size_t byte_amount = 0, Endian endianess = NO_SWAP );

        Reader getReader( size_t reader_size );

        uint64_t readU64( Endian endianess = NO_SWAP );
        int64_t  readI64( Endian endianess = NO_SWAP );

        uint32_t readU32( Endian endianess = NO_SWAP );
        int32_t  readI32( Endian endianess = NO_SWAP );

        uint16_t readU16( Endian endianess = NO_SWAP );
        int16_t  readI16( Endian endianess = NO_SWAP );

        uint8_t readU8();
        int8_t  readI8();
    };
};

}

#endif // UTILITIES_BUFFER_938223_H
