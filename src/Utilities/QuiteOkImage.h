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
    
    const static Pixel INITIAL_PIXEL;
    const static Pixel    ZERO_PIXEL;
    constexpr static size_t PIXEL_HASH_TABLE_SIZE = 64;
    constexpr static size_t MAX_RUN_AMOUNT = 62;
    
private:
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
    int read( const Buffer& buffer, ImageData& image_data, unsigned int back_header_search = 4);
};

};

#endif // QUITE_OK_IMAGE_HEADER

