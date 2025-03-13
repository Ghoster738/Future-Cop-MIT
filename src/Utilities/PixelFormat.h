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
        virtual uint_fast8_t byteSize() const = 0;
    };

    class PixelFormatColor : public PixelFormat {
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
            channel_fp getDistanceSq( const GenericColor& other ) const {
                const channel_fp R = (other.red   - red);
                const channel_fp G = (other.green - green);
                const channel_fp B = (other.blue  - blue);
                const channel_fp A = (other.alpha - alpha);
                
                return R * R + G * G + B * B + A * A;
            }
            
            std::string getString() const;
        };
        enum ChannelInterpolation {
            LINEAR,
            sRGB
        };
        ChannelInterpolation interpolation;
        
        PixelFormatColor() : interpolation(LINEAR) {}
        PixelFormatColor( ChannelInterpolation interpolate ) : interpolation(interpolate) {}
        
        virtual void writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const GenericColor& color ) const = 0;
        virtual GenericColor readPixel( Buffer::Reader &buffer, Buffer::Endian endian = Buffer::Endian::NO_SWAP ) const = 0;
    };

    class PixelFormatColor_W8 : public PixelFormatColor {
    private:
        PixelFormatColor_W8( PixelFormatColor::ChannelInterpolation color ) : PixelFormatColor(color) {}

    public:
        struct Color {
            uint8_t white;
            
            Color() {}
            Color( Utilities::PixelFormatColor::GenericColor generic, PixelFormatColor::ChannelInterpolation interpolate );
            
            PixelFormatColor::GenericColor toGeneric( PixelFormatColor::ChannelInterpolation interpolate ) const;
        };
        
        virtual PixelFormat* duplicate() const {
            return new PixelFormatColor_W8( interpolation );
        }
        virtual uint_fast8_t byteSize() const { return 1; }
        virtual void writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& color ) const;
        virtual PixelFormatColor::GenericColor readPixel( Buffer::Reader &buffer, Buffer::Endian endian = Buffer::Endian::NO_SWAP ) const;
        
        virtual std::string getName() const { return "Black & White 8-bit"; }

        static const PixelFormatColor_W8 linear;
        static const PixelFormatColor_W8  s_rgb;
    };

    class PixelFormatColor_W8A8 : public PixelFormatColor {
    private:
        PixelFormatColor_W8A8( PixelFormatColor::ChannelInterpolation color ) : PixelFormatColor(color) {}

    public:
        struct Color {
            uint8_t white;
            uint8_t alpha;
            
            Color() {}
            Color( Utilities::PixelFormatColor::GenericColor generic, PixelFormatColor::ChannelInterpolation interpolate );
            
            PixelFormatColor::GenericColor toGeneric( PixelFormatColor::ChannelInterpolation interpolate ) const;
        };
        
        virtual PixelFormat* duplicate() const {
            return new PixelFormatColor_W8A8( interpolation );
        }
        virtual uint_fast8_t byteSize() const { return 2; }
        virtual void writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& color ) const;
        virtual PixelFormatColor::GenericColor readPixel( Buffer::Reader &buffer, Buffer::Endian endian = Buffer::Endian::NO_SWAP ) const;
        virtual std::string getName() const { return "Black & White 8-bit, Alpha 8-bit"; }

        static const PixelFormatColor_W8A8 linear;
        static const PixelFormatColor_W8A8  s_rgb;
    };

    class PixelFormatColor_R5G5B5A1 : public PixelFormatColor {
    private:
        PixelFormatColor_R5G5B5A1( PixelFormatColor::ChannelInterpolation color ) : PixelFormatColor(color) {}

    public:
        struct Color {
            uint16_t red   : 5;
            uint16_t green : 5;
            uint16_t blue  : 5;
            uint16_t alpha : 1;
            
            Color() {}
            Color( Utilities::PixelFormatColor::GenericColor generic, PixelFormatColor::ChannelInterpolation interpolate );
            
            PixelFormatColor::GenericColor toGeneric( PixelFormatColor::ChannelInterpolation interpolate ) const;
        };
        
        virtual PixelFormat* duplicate() const {
            return new PixelFormatColor_R5G5B5A1( interpolation );
        }
        virtual uint_fast8_t byteSize() const { return 2; }
        virtual void writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& color ) const;
        virtual PixelFormatColor::GenericColor readPixel( Buffer::Reader &buffer, Buffer::Endian endian = Buffer::Endian::NO_SWAP ) const;
        virtual std::string getName() const { return "Red 5-bit, Green 5-bit, Blue 5-bit, Alpha 1-bit"; }

        static const PixelFormatColor_R5G5B5A1 linear;
        static const PixelFormatColor_R5G5B5A1  s_rgb;
    };
    
    class PixelFormatColor_B5G5R5A1 : public PixelFormatColor {
    private:
        PixelFormatColor_B5G5R5A1( PixelFormatColor::ChannelInterpolation color ) : PixelFormatColor(color) {}

    public:
        struct Color {
            uint16_t blue  : 5;
            uint16_t green : 5;
            uint16_t red   : 5;
            uint16_t alpha : 1;
            
            Color() {}
            Color( Utilities::PixelFormatColor::GenericColor generic, PixelFormatColor::ChannelInterpolation interpolate );
            
            PixelFormatColor::GenericColor toGeneric( PixelFormatColor::ChannelInterpolation interpolate ) const;
        };
        
        virtual PixelFormat* duplicate() const {
            return new PixelFormatColor_B5G5R5A1( interpolation );
        }
        virtual uint_fast8_t byteSize() const { return 2; }
        virtual void writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& color ) const;
        virtual PixelFormatColor::GenericColor readPixel( Buffer::Reader &buffer, Buffer::Endian endian = Buffer::Endian::NO_SWAP ) const;
        virtual std::string getName() const { return "Blue 5-bit, Green 5-bit, Red 5-bit, Alpha 1-bit"; }

        static const PixelFormatColor_B5G5R5A1 linear;
        static const PixelFormatColor_B5G5R5A1  s_rgb;
    };

    class PixelFormatColor_R5G5B5T1 : public PixelFormatColor {
    private:
        PixelFormatColor_R5G5B5T1( PixelFormatColor::ChannelInterpolation color ) : PixelFormatColor(color) {}

    public:
        struct Color {
            uint16_t red   : 5;
            uint16_t green : 5;
            uint16_t blue  : 5;
            uint16_t semi_transparency : 1;
            
            Color() {}
            Color( Utilities::PixelFormatColor::GenericColor generic, PixelFormatColor::ChannelInterpolation interpolate );
            
            PixelFormatColor::GenericColor toGeneric( PixelFormatColor::ChannelInterpolation interpolate ) const;
        };
        
        virtual PixelFormat* duplicate() const {
            return new PixelFormatColor_R5G5B5T1( interpolation );
        }
        
        virtual uint_fast8_t byteSize() const { return 2; }
        
        virtual void writePixel( Utilities::Buffer::Writer &buffer, Utilities::Buffer::Endian endian, const PixelFormatColor::GenericColor& coloring ) const;
        virtual PixelFormatColor::GenericColor readPixel( Utilities::Buffer::Reader &buffer, Utilities::Buffer::Endian endian = Utilities::Buffer::Endian::NO_SWAP ) const;
        
        virtual std::string getName() const { return "Red 5-bit, Green 5-bit, Blue 5-bit, Semi-Transparent 1-bit"; }

        static const PixelFormatColor_R5G5B5T1 linear;
        static const PixelFormatColor_R5G5B5T1  s_rgb;
    };

    class PixelFormatColor_B5G5R5T1 : public Utilities::PixelFormatColor {
    private:
        PixelFormatColor_B5G5R5T1( PixelFormatColor::ChannelInterpolation color ) : PixelFormatColor(color) {}

    public:
        struct Color {
            uint16_t red   : 5;
            uint16_t green : 5;
            uint16_t blue  : 5;
            uint16_t semi_transparency : 1;
            
            Color() {}
            Color( Utilities::PixelFormatColor::GenericColor generic, PixelFormatColor::ChannelInterpolation interpolate );
            
            PixelFormatColor::GenericColor toGeneric( PixelFormatColor::ChannelInterpolation interpolate ) const;
        };
        
        virtual PixelFormat* duplicate() const {
            return new PixelFormatColor_B5G5R5T1( interpolation );
        }
        
        virtual uint_fast8_t byteSize() const { return 2; }
        
        virtual void writePixel( Utilities::Buffer::Writer &buffer, Utilities::Buffer::Endian endian, const PixelFormatColor::GenericColor& coloring ) const;
        virtual PixelFormatColor::GenericColor readPixel( Utilities::Buffer::Reader &buffer, Utilities::Buffer::Endian endian = Utilities::Buffer::Endian::NO_SWAP ) const;
        virtual std::string getName() const { return "Blue 5-bit, Green 5-bit, Red 5-bit, Semi-Transparent 1-bit"; }

        static const PixelFormatColor_B5G5R5T1 linear;
        static const PixelFormatColor_B5G5R5T1  s_rgb;
    };

    class PixelFormatColor_R8G8B8 : public PixelFormatColor {
    private:
        PixelFormatColor_R8G8B8( PixelFormatColor::ChannelInterpolation color ) : PixelFormatColor(color) {}

    public:
        struct Color {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            
            Color() {}
            Color( Utilities::PixelFormatColor::GenericColor generic, PixelFormatColor::ChannelInterpolation interpolate );
            
            PixelFormatColor::GenericColor toGeneric( PixelFormatColor::ChannelInterpolation interpolate ) const;
        };
        
        virtual PixelFormat* duplicate() const {
            return new PixelFormatColor_R8G8B8( interpolation );
        }
        virtual uint_fast8_t byteSize() const { return 3; }
        virtual void writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& color ) const;
        virtual PixelFormatColor::GenericColor readPixel( Buffer::Reader &buffer, Buffer::Endian endian = Buffer::Endian::NO_SWAP ) const;
        virtual std::string getName() const { return "Red 8-bit, Green 8-bit, Blue 8-bit"; }

        static const PixelFormatColor_R8G8B8 linear;
        static const PixelFormatColor_R8G8B8  s_rgb;
    };

    class PixelFormatColor_R8G8B8A8 : public PixelFormatColor {
    private:
        PixelFormatColor_R8G8B8A8( PixelFormatColor::ChannelInterpolation color ) : PixelFormatColor(color) {}

    public:
        struct Color {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            uint8_t alpha;
            
            Color() {}
            Color( PixelFormatColor::GenericColor generic, PixelFormatColor::ChannelInterpolation interpolate );
            
            PixelFormatColor::GenericColor toGeneric( PixelFormatColor::ChannelInterpolation interpolate ) const;
        };
        
        virtual PixelFormat* duplicate() const {
            return new PixelFormatColor_R8G8B8A8( interpolation );
        }
        virtual uint_fast8_t byteSize() const { return 4; }
        virtual void writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& color ) const;
        virtual PixelFormatColor::GenericColor readPixel( Buffer::Reader &buffer, Buffer::Endian endian = Buffer::Endian::NO_SWAP ) const;
        virtual std::string getName() const { return "Red 8-bit, Green 8-bit, Blue 8-bit, Alpha 8-bit"; }

        static const PixelFormatColor_R8G8B8A8 linear;
        static const PixelFormatColor_R8G8B8A8  s_rgb;
    };

    class ColorPalette {
    private:
        const PixelFormatColor *color_r;
        Buffer buffer;
        Buffer::Endian endianess;
    public:
        ColorPalette( const ColorPalette& palette );
        ColorPalette( const PixelFormatColor& palette_color, Buffer::Endian endianess = Buffer::Endian::NO_SWAP );
        virtual ~ColorPalette() {}
        
        bool empty() const;
        
        const PixelFormatColor* getColorFormat() const { return this->color_r; }
        PixelFormatColor::GenericColor getIndex( palette_index index ) const;
        uint_fast8_t getLastIndex() const;
        Buffer::Endian getEndian() const { return endianess; }
        Buffer::Reader getReader() const { return buffer.getReader(); }
        Buffer::Reader getReader( palette_index index ) const { return buffer.getReader( static_cast<size_t>( index ) * this->color_r->byteSize(), this->color_r->byteSize() ); }
        
        bool setIndex( palette_index index, const PixelFormatColor::GenericColor &color );
        bool setAmount( uint16_t amount );
    };
}

#endif // UTILITIES_PIXEL_FORMAT_HEADER

