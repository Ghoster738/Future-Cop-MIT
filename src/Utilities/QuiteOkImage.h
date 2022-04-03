#ifndef QUITE_OK_IMAGE_HEADER
#define QUITE_OK_IMAGE_HEADER

#include <vector>
#include <stdint.h>

#include "Buffer.h"
#include "ImageData.h"

namespace Utilities {

/**
 * This class is to both encode and decode a losslessy compressed image format.
 *
 * Note: This is a purely single threaded class. The only way to multithread is to encode seperate images.
 */
class QuiteOkImage {
public:
    struct Pixel {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t alpha;
    };
    struct PixelSigned {
        int8_t red;
        int8_t green;
        int8_t blue;
        int8_t alpha;
    };
    enum Type { RED_GREEN_BLUE = 3, RED_GREEN_BLUE_ALPHA = 4 };
    enum ColorSpace { S_RGB = 0, LINEAR = 1 };
    enum ChunkType {
        QOI_OP_NONE,
        QOI_OP_RGB,
        QOI_OP_RGBA,
        QOI_OP_INDEX,
        QOI_OP_DIFF,
        QOI_OP_LUMA,
        QOI_OP_RUN
    };
    
    const static Pixel INITIAL_PIXEL;
    const static Pixel    ZERO_PIXEL;
    constexpr static size_t PIXEL_HASH_TABLE_SIZE = 64;
    constexpr static size_t MAX_RUN_AMOUNT = 62;
    
private:
    class ImageBuffer {
    public:
        uint8_t advance;
        uint8_t *buffer_p;
        uint8_t *write_head_r;
        uint8_t *write_head_end_r;
        bool sanitize;
    public:
        ImageBuffer( uint32_t width, uint32_t height, Type type, bool sanitize );
        ~ImageBuffer();
        
        bool writePixel( const Pixel& pixel );
    };
    Type type;
    Pixel pixel_hash_table[ PIXEL_HASH_TABLE_SIZE ];
    Pixel previous_pixel;
    uint8_t run_amount; // 0 - 62
    
    /**
     * This method fills the hash table with zeros and sets the previous pixel to its initial state.
     */
    void reset();
    
    /**
     * This function gets the hash index of the given pixel.
     * @return (r * 3 + g * 5 + b * 7 + a * 11) % 64
     */
    static uint8_t getHashIndex( const Pixel& pixel );
    
    /**
     * This method places the pixel in the hash table.
     * @return If the same pixel is in the table then this returns true.
     */
    void placePixelInHash( const Pixel& pixel );
    
    static uint8_t wrap( int16_t operand );
    
    static bool matchColor( const Pixel& one, const Pixel& two );
    static PixelSigned subColor( const Pixel& current, const Pixel& previous );
    
    bool isOPRunPossiable( const Pixel& pixel ) const;
    bool isOPIndexPossiable( const Pixel& pixel ) const;
    bool isOPDiffPossiable( const Pixel& pixel ) const;
    bool isOPLumaPossiable( const Pixel& pixel ) const;
    bool isOpRGBPossiable( const Pixel& pixel ) const;
    
    void applyOPDiff( const Pixel& pixel, Buffer& buffer ) const;
    void applyOPLuma( const Pixel& pixel, Buffer& buffer ) const;
    void applyOPRGB(  const Pixel& pixel, Buffer& buffer ) const;
    void applyOPRGBA( const Pixel& pixel, Buffer& buffer ) const;
    
public:
    QuiteOkImage();
    ~QuiteOkImage();
    
    Buffer * write( const ImageData& image_data );
    bool read( const Buffer::Reader &reader, ImageData& image_data );
};

};

#endif // QUITE_OK_IMAGE_HEADER

