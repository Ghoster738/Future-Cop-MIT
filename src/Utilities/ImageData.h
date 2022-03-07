#ifndef DATA_IMAGE_HEADER
#define DATA_IMAGE_HEADER

#include <vector>
#include <stdint.h>

namespace Utilities {

class ImageData {
public:
    enum Type { BLACK_WHITE, BLACK_WHITE_ALHPA, RED_GREEN_BLUE, RED_GREEN_BLUE_ALHPA };
protected:
    unsigned int width;
    unsigned int height;
    unsigned int bytes_per_channel;
    std::vector<char> image_data;
    Type type;
public:
    ImageData();
    ImageData( const ImageData &obj );
    ImageData( unsigned int width, unsigned int height, Type type, unsigned int bytes_per_channel );

    /**
     * @return the image's width.
     */
    unsigned int getWidth() const;

    /**
     * @return the image's height.
     */
    unsigned int getHeight() const;

    bool inscribeSubImage( unsigned int x, unsigned int y, const ImageData& ref );

    ImageData subImage( unsigned int x, unsigned int y, unsigned int width, unsigned int height ) const;

    ImageData applyPalette( const ImageData& palette ) const;

    /**
     * This sets the width of the image.
     * However, be warned that this image is not really scaled! To make the image wider use the setFormat() method.
     * @param width The width of the image.
     */
    void setWidth( unsigned int width );

    /**
     * This sets the height of the image.
     * However, be warned that this image is not really scaled! To make the image wider use the setFormat() method.
     * @param height The height of the image.
     */
    void setHeight( unsigned int height );

    /**
     * This method sets the format only if it returns true.
     * @param type The format of the exports
     * @param bytes_per_channel
     * @return If the format is supported for write then it will return true. Otherwise it will return false.
     */
    bool setFormat( Type type, unsigned int bytes_per_channel );

    /**
     * You can get the size of each pixel with this method.
     * @return the size of the pixel size.
     */
    unsigned int getPixelSize() const;
    /**
     * This method only writes the pixel when it is in bounds.
     * @param x the x location bounded by width.
     * @param y the y location bounded by width.
     * @return True if the pixel is written, false if no pixel is written.
     */
    bool setPixel( unsigned int x, unsigned int y, const char *const pixel );

    const char *const getPixel( unsigned int x, unsigned int y ) const;
    
    /**
     * Flip the image vertically.It is an O(p) operation, and p is the
     * number pixels.
     * @return If the image is vaild this would flip the image.
     */
    bool flipVertically();
    
    /**
     * Flip the image horizontally.It is an O(p) operation, and p is the
     * number pixels.
     * @return If the image is vaild this would flip the image.
     */
    bool flipHorizontally();

    /**
     * This runs a test to see if this texture is exportable.
     */
    bool isValid();

    /**
     * This gets the raw image data. However, note that the pointer could change at reallocations if this is possible.
     * @return the pointer of the member variable image_data.
     */
    char * getRawImageData();

    /**
     * This gets the raw image data. However, note that the pointer could change at reallocations if this is possible.
     * @return the pointer of the member variable image_data.
     */
    const char *const getRawImageData() const;

    /**
     * This method exports a png file.
     * @param file_path The file path to the PNG file.
     * @return 1 for a successful write, 0 for a warning, less than 0 for any errors.
     */
    int write( const char *const file_path ) const;

    inline static void translate_16_to_24( uint16_t word, uint8_t &blue, uint8_t &green, uint8_t &red ) {
        // Thanks ktownsend of the adafruit forms.
        // Based on the code from https://forums.adafruit.com/viewtopic.php?t=21536
        blue  = (word & 0x001F) << 3; // Left shift by 3
        green = (word & 0x03E0) >> 2; // Right shift by  4 and left shift by 2
        red   = (word & 0x7C00) >> 7; // Right shift by 10 and left shift by 3
    }
    inline static uint16_t translate_24_to_16( uint16_t blue, uint16_t green, uint16_t red ) {
        return ((red << 7) & 0x7C00) | ((green << 2) & 0x03E0) | ((blue >> 3) & 0x001F);
    }
};

};

#endif // DATA_IMAGE_HEADER

