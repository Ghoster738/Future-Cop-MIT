#ifndef UTILITIES_PIXEL_FORMAT_HEADER
#define UTILITIES_PIXEL_FORMAT_HEADER

#include "Buffer.h"

#include <stdint.h>
#include <vector>

namespace Utilities {

    typedef float channel_fp;
    typedef uint16_t palette_index;

    class PixelFormat {
    public:
        virtual ~PixelFormat() {}
        virtual PixelFormat* duplicate() const = 0;
        virtual std::string getName() const = 0;
    };

    class PixelFormatByte : public PixelFormat {
    public:
        virtual uint_fast8_t byteSize() const = 0;
    };

    class PixelFormatBit : public PixelFormat {
    public:
        virtual uint_fast8_t bitSize() const = 0;
    };

    class PixelFormatColor : public PixelFormatByte {
    public:
        /**
         * GenericColor is to always use linear color space.
         */
        struct GenericColor {
            channel_fp red;
            channel_fp green;
            channel_fp blue;
            channel_fp alpha;
            
            GenericColor() {}
            GenericColor( channel_fp r, channel_fp g, channel_fp b, channel_fp a) : red( r ), green( g ), blue( b ), alpha( a ) {}
            
            /**
             * This sets the grey scale value.
             */
            void setValue( channel_fp value ) {
                red = value;
                green = value;
                blue = value;
            }
        };
        enum ChannelInterpolation {
            LINEAR,
            sRGB
        };
        ChannelInterpolation interpolation;
        
        PixelFormatColor() : interpolation(LINEAR) {}
        PixelFormatColor( ChannelInterpolation interpolate ) : interpolation(interpolate) {}
        
        virtual void writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const GenericColor& color ) = 0;
        virtual GenericColor readPixel( Buffer::Reader &buffer, Buffer::Endian endian = Buffer::Endian::NO_SWAP ) = 0;
    };

    class PixelFormatColor_W8 : public PixelFormatColor {
    public:
        struct Color {
            uint8_t white;
            
            Color() {}
            Color( Utilities::PixelFormatColor::GenericColor generic, ChannelInterpolation interpolate );
            
            PixelFormatColor::GenericColor toGeneric( ChannelInterpolation interpolate ) const;
        };
        PixelFormatColor_W8() : PixelFormatColor(LINEAR) {}
        PixelFormatColor_W8( ChannelInterpolation color ) : PixelFormatColor(color) {}
        
        virtual PixelFormat* duplicate() const {
            return new PixelFormatColor_W8( interpolation );
        }
        virtual uint_fast8_t byteSize() const { return 1; }
        virtual void writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& color );
        virtual PixelFormatColor::GenericColor readPixel( Buffer::Reader &buffer, Buffer::Endian endian = Buffer::Endian::NO_SWAP );
        
        virtual std::string getName() const { return "Black & White 8-bit"; }
    };

    class PixelFormatColor_W8A8 : public PixelFormatColor {
    public:
        struct Color {
            uint8_t white;
            uint8_t alpha;
            
            Color() {}
            Color( Utilities::PixelFormatColor::GenericColor generic, ChannelInterpolation interpolate );
            
            PixelFormatColor::GenericColor toGeneric( ChannelInterpolation interpolate ) const;
        };
        PixelFormatColor_W8A8() : PixelFormatColor(LINEAR) {}
        PixelFormatColor_W8A8( ChannelInterpolation color ) : PixelFormatColor(color) {}
        
        virtual PixelFormat* duplicate() const {
            return new PixelFormatColor_W8A8( interpolation );
        }
        virtual uint_fast8_t byteSize() const { return 2; }
        virtual void writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& color );
        virtual PixelFormatColor::GenericColor readPixel( Buffer::Reader &buffer, Buffer::Endian endian = Buffer::Endian::NO_SWAP );
        virtual std::string getName() const { return "Black & White 8-bit, Alpha 8-bit"; }
    };

    class PixelFormatColor_R5G5B5A1 : public PixelFormatColor {
    public:
        struct Color {
            uint16_t red   : 5;
            uint16_t green : 5;
            uint16_t blue  : 5;
            uint16_t alpha : 1;
            
            Color() {}
            Color( Utilities::PixelFormatColor::GenericColor generic, ChannelInterpolation interpolate );
            
            PixelFormatColor::GenericColor toGeneric( ChannelInterpolation interpolate ) const;
        };
        PixelFormatColor_R5G5B5A1() : PixelFormatColor(LINEAR) {}
        PixelFormatColor_R5G5B5A1( ChannelInterpolation color ) : PixelFormatColor(color) {}
        
        virtual PixelFormat* duplicate() const {
            return new PixelFormatColor_R5G5B5A1( interpolation );
        }
        virtual uint_fast8_t byteSize() const { return 2; }
        virtual void writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& color );
        virtual PixelFormatColor::GenericColor readPixel( Buffer::Reader &buffer, Buffer::Endian endian = Buffer::Endian::NO_SWAP );
        virtual std::string getName() const { return "Red 5-bit, Green 5-bit, Blue 5-bit, Alpha 1-bit"; }
    };

    class PixelFormatColor_R8G8B8 : public PixelFormatColor {
    public:
        struct Color {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            
            Color() {}
            Color( Utilities::PixelFormatColor::GenericColor generic, ChannelInterpolation interpolate );
            
            PixelFormatColor::GenericColor toGeneric( ChannelInterpolation interpolate ) const;
        };
        PixelFormatColor_R8G8B8() : PixelFormatColor(LINEAR) {}
        PixelFormatColor_R8G8B8( ChannelInterpolation color ) : PixelFormatColor(color) {}
        
        virtual PixelFormat* duplicate() const {
            return new PixelFormatColor_R8G8B8( interpolation );
        }
        virtual uint_fast8_t byteSize() const { return 3; }
        virtual void writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& color );
        virtual PixelFormatColor::GenericColor readPixel( Buffer::Reader &buffer, Buffer::Endian endian = Buffer::Endian::NO_SWAP );
        virtual std::string getName() const { return "Red 8-bit, Green 8-bit, Blue 8-bit"; }
    };

    class PixelFormatColor_R8G8B8A8 : public PixelFormatColor {
    public:
        struct Color {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            uint8_t alpha;
            
            Color() {}
            Color( Utilities::PixelFormatColor::GenericColor generic, ChannelInterpolation interpolate );
            
            PixelFormatColor::GenericColor toGeneric( ChannelInterpolation interpolate ) const;
        };
        PixelFormatColor_R8G8B8A8() : PixelFormatColor(LINEAR) {}
        PixelFormatColor_R8G8B8A8( ChannelInterpolation color ) : PixelFormatColor(color) {}
        
        virtual PixelFormat* duplicate() const {
            return new PixelFormatColor_R8G8B8A8( interpolation );
        }
        virtual uint_fast8_t byteSize() const { return 4; }
        virtual void writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& color );
        virtual PixelFormatColor::GenericColor readPixel( Buffer::Reader &buffer, Buffer::Endian endian = Buffer::Endian::NO_SWAP );
        virtual std::string getName() const { return "Red 8-bit, Green 8-bit, Blue 8-bit, Alpha 8-bit"; }
    };

    class ColorPalette {
    private:
        PixelFormatColor *color_p;
        Buffer buffer;
        Buffer::Endian endianess;
    public:
        ColorPalette( const PixelFormatColor& color_palette, Buffer::Endian endianess = Buffer::Endian::NO_SWAP );
        virtual ~ColorPalette() {
            delete color_p;
        }
        
        const PixelFormatColor& getColorFormat() const { return *color_p; }
        
        uint_fast16_t getTotalIndex() const;
        
        PixelFormatColor::GenericColor getIndex( palette_index index ) const;
        bool setIndex( palette_index index, const PixelFormatColor::GenericColor color );
    };

    class PixelFormatIndex : public PixelFormatByte {
    protected:
        uint_fast8_t index_size;
    public:
        PixelFormatIndex( uint_fast8_t index_size_param );
        
        void writePixel( palette_index indexValue, Buffer::Writer &buffer, Buffer::Endian endian = Buffer::Endian::NO_SWAP );
        palette_index readPixel( Buffer::Reader &buffer, Buffer::Endian endian = Buffer::Endian::NO_SWAP );
        virtual uint_fast8_t byteSize() const { return index_size; }
        virtual std::string getName() const { return "Palette byte format"; }
    };

    class PixelFormatBitIndex : public PixelFormatBit {
    protected:
        uint_fast8_t bits_per_index;
    public:
        PixelFormatBitIndex( uint_fast8_t bits_per_index_param );
        
        void writePixel( uint8_t *buffer_r, uint_fast8_t shift, palette_index indexValue );
        palette_index readPixel( Buffer::Reader &buffer, uint_fast8_t offset, Buffer::Endian endian );
        virtual uint_fast8_t bitSize() const { return bits_per_index; }
        virtual std::string getName() const { return "Palette bit format"; }
    };
}

#endif // UTILITIES_PIXEL_FORMAT_HEADER

