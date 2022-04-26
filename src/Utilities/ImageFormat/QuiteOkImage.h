#ifndef QUITE_OK_IMAGE_HEADER
#define QUITE_OK_IMAGE_HEADER

#include "ImageFormat.h"

namespace Utilities {

namespace ImageFormat {

/**
 * This class is to both encode and decode a losslessy compressed image format.
 *
 * This format is developed by Dominic Szablewski.
 * The specification for this format can be found in qoiformat.org
 * Note: This is a purely single threaded class. The only way to multithread is to encode seperate images.
 */
class QuiteOkImage : public ImageFormat {
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
    struct QOIStatus {
        // These are the used_* bits are true if the apply methods were ran.
        uint16_t used_RGBA  : 1;
        uint16_t used_RGB   : 1;
        uint16_t used_index : 1;
        uint16_t used_diff  : 1;
        uint16_t used_luma  : 1;
        uint16_t used_run   : 1;
        uint16_t channel_ep : 1; // The image was black and white and it needed to be expanded.
        uint16_t depth_redu : 1; // The depth of the image had to be reduced.
        uint16_t complete   : 1; // The entire source has been read.
        uint16_t success    : 1;
        uint16_t status     : 8;
    };
    enum Status {
        OKAY                 = 0,
        INVALID_IMAGE_FORMAT = 1, // The format is not convertable to the exporter
        INVALID_BEGIN_HEADER = 2, // The begin header is invalid.
        INVALID_END_HEADER   = 3, // The end header is invalid.
        INVALID_IMAGE_SIZE   = 4  // The image's width or/and height is not valid
    };
    
    const static Pixel INITIAL_PIXEL;
    const static Pixel    ZERO_PIXEL;
    constexpr static size_t PIXEL_HASH_TABLE_SIZE = 64;
    constexpr static size_t MAX_RUN_AMOUNT = 62;
    constexpr static uint8_t BIAS = 2;
    constexpr static uint8_t BIG_BIAS = 8;
    constexpr static uint8_t GREEN_BIAS = 32;
    
    constexpr static uint8_t QOI_OP_RGB   = 0b11111110;
    constexpr static uint8_t QOI_OP_RGBA  = 0b11111111;
    constexpr static uint8_t QOI_OP_INDEX = 0b00000000;
    constexpr static uint8_t QOI_OP_DIFF  = 0b01000000;
    constexpr static uint8_t QOI_OP_LUMA  = 0b10000000;
    constexpr static uint8_t QOI_OP_RUN   = 0b11000000;
    constexpr static uint8_t QOI_OP_D_BIT = 0b00111111;
    constexpr static uint8_t QOI_OP_S_BIT = QOI_OP_RUN;  // Small opcodes
    constexpr static uint8_t QOI_OP_B_BIT = QOI_OP_RGBA; // Big opcodes
    
    const static std::string FILE_EXTENSION;
    
private:
    Pixel pixel_hash_table[ PIXEL_HASH_TABLE_SIZE ];
    Pixel previous_pixel;
    PixelSigned difference;
    int8_t dr_dg;
    int8_t db_dg;
    uint8_t run_amount; // 1 - 62
    QOIStatus status;
    
    unsigned int back_header_search;
    
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
    
    static bool matchColor( const Pixel& one, const Pixel& two );
    static PixelSigned subColor( const Pixel& current, const Pixel& previous );
    
    bool isOPIndexPossiable( const Pixel& pixel ) const;
    bool isOPDiffPossiable( const Pixel& pixel ) const;
    bool isOPLumaPossiable( const Pixel& pixel ) const;
    bool isOpRGBPossiable( const Pixel& pixel ) const;
    
    void applyOPRun(  Buffer& buffer );
    void applyOPDiff( const Pixel& pixel, Buffer& buffer );
    void applyOPLuma( const Pixel& pixel, Buffer& buffer );
    void applyOPRGB(  const Pixel& pixel, Buffer& buffer );
    void applyOPRGBA( const Pixel& pixel, Buffer& buffer );
    
public:
    QuiteOkImage();
    ~QuiteOkImage();
    
    bool canRead() const;
    bool canWrite() const;
    
    virtual std::string getExtension() const;
    
    int write( const ImageData& image_data, Buffer& buffer );
    int read( const Buffer& buffer, ImageData& image_data );
    
    QOIStatus getStatus() const;
};

};

};

#endif // QUITE_OK_IMAGE_HEADER

