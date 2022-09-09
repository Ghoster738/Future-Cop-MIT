#include "PixelFormat.h"

#include <cmath>
#include <cassert>

namespace {
const Utilities::channel_fp SRGB_VALUE = 2.2;


const Utilities::channel_fp MAX_U5BIT_VALUE = 31.0;
// A rounded up number of this MAX_U5BIT_VALUE powered by 2.2.
const Utilities::channel_fp MAX_U5BIT_sRGB_VALUE = 1909.834465;

const Utilities::channel_fp MAX_UBYTE_VALUE = 255.0;
// A rounded up number of this MAX_UBYTE_VALUE powered by 2.2.
const Utilities::channel_fp MAX_UBYTE_sRGB_VALUE = 196964.6992;
}

Utilities::PixelFormatColor_W8::Color::Color( Utilities::PixelFormatColor::GenericColor generic, ChannelInterpolation interpolate )
{
    PixelFormatColor_W8A8::Color other_color( generic, interpolate );
    
    white = other_color.white;
}

Utilities::PixelFormatColor::GenericColor Utilities::PixelFormatColor_W8::Color::toGeneric( ChannelInterpolation interpolate ) const {
    Utilities::PixelFormatColor_W8A8::Color expand;
    expand.white = white;
    expand.alpha = 255;
    
    return expand.toGeneric( interpolate );
}

void Utilities::PixelFormatColor_W8::writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& generic_color )
{
    PixelFormatColor_W8::Color color( generic_color, interpolation );
    
    buffer.writeU8( color.white );
}

Utilities::PixelFormatColor::GenericColor Utilities::PixelFormatColor_W8::readPixel( Buffer::Reader &buffer, Buffer::Endian endian )
{
    PixelFormatColor_W8::Color color;
    
    color.white = buffer.readU8();
    
    return color.toGeneric( interpolation );
}

Utilities::PixelFormatColor_W8A8::Color::Color( Utilities::PixelFormatColor::GenericColor generic, ChannelInterpolation interpolate )
{
    if( interpolate == sRGB )
        white = pow( generic.red, 1.0 / SRGB_VALUE ) * MAX_UBYTE_VALUE + 0.5;
    else
        white = generic.red * MAX_UBYTE_VALUE;
    
    alpha = generic.alpha * MAX_UBYTE_VALUE;
}

Utilities::PixelFormatColor::GenericColor Utilities::PixelFormatColor_W8A8::Color::toGeneric( ChannelInterpolation interpolate ) const {
    GenericColor color;
    
    channel_fp value = static_cast<channel_fp>(white);
    
    if( interpolate == sRGB )
        color.setValue( pow( value, SRGB_VALUE ) / MAX_UBYTE_sRGB_VALUE );
    else
        color.setValue( value / MAX_UBYTE_VALUE );
    
    color.alpha = static_cast<channel_fp>(alpha) / MAX_UBYTE_VALUE;
    
    return color;
}

void Utilities::PixelFormatColor_W8A8::writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& generic_color )
{
    PixelFormatColor_W8A8::Color color( generic_color, interpolation );
    
    buffer.writeU8( color.white );
    buffer.writeU8( color.alpha );
}

Utilities::PixelFormatColor::GenericColor Utilities::PixelFormatColor_W8A8::readPixel( Buffer::Reader &buffer, Buffer::Endian endian )
{
    PixelFormatColor_W8A8::Color color;
    
    color.white = buffer.readU8();
    color.alpha = buffer.readU8();
    
    return color.toGeneric( interpolation );
}

Utilities::PixelFormatColor_R5G5B5A1::Color::Color( PixelFormatColor::GenericColor generic, ChannelInterpolation interpolate ) {
    if( interpolate == sRGB ) {
        red   = pow( generic.red,   1.0 / SRGB_VALUE ) * MAX_U5BIT_VALUE + 0.5;
        green = pow( generic.green, 1.0 / SRGB_VALUE ) * MAX_U5BIT_VALUE + 0.5;
        blue  = pow( generic.blue,  1.0 / SRGB_VALUE ) * MAX_U5BIT_VALUE + 0.5;
    }
    else {
        red   = generic.red   * MAX_U5BIT_VALUE;
        green = generic.green * MAX_U5BIT_VALUE;
        blue  = generic.blue  * MAX_U5BIT_VALUE;
    }
    
    if( generic.alpha > 0.5 )
        alpha = 1;
    else
        alpha = 0;
}

Utilities::PixelFormatColor::GenericColor Utilities::PixelFormatColor_R5G5B5A1::Color::toGeneric( ChannelInterpolation interpolate ) const {
    GenericColor color;
    
    float   red_value = static_cast<channel_fp>(red);
    float green_value = static_cast<channel_fp>(green);
    float  blue_value = static_cast<channel_fp>(blue);
    
    if( interpolate == sRGB ) {
        color.red   = pow(   red_value, SRGB_VALUE ) / MAX_U5BIT_sRGB_VALUE;
        color.green = pow( green_value, SRGB_VALUE ) / MAX_U5BIT_sRGB_VALUE;
        color.blue  = pow(  blue_value, SRGB_VALUE ) / MAX_U5BIT_sRGB_VALUE;
    }
    else {
        color.red   =   red_value / MAX_U5BIT_VALUE;
        color.green = green_value / MAX_U5BIT_VALUE;
        color.blue  =  blue_value / MAX_U5BIT_VALUE;
    }
    
    if( alpha )
        color.alpha = 1.0;
    else
        color.alpha = 0.0;
    
    return color;
}

void Utilities::PixelFormatColor_R5G5B5A1::writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& generic_color )
{
    PixelFormatColor_R5G5B5A1::Color color( generic_color, interpolation );
    
    uint16_t data;
    data  = ((color.alpha << 15) & 0x8000);
    data |= ((color.red   << 10) & 0x7C00);
    data |= ((color.green <<  5) & 0x03E0);
    data |= ((color.blue  <<  0) & 0x001F);
    
    buffer.writeU16( data, endian );
}

Utilities::PixelFormatColor::GenericColor Utilities::PixelFormatColor_R5G5B5A1::readPixel( Buffer::Reader &buffer, Buffer::Endian endian  )
{
    PixelFormatColor_R5G5B5A1::Color color;
    auto word = buffer.readU16( endian );
    
    // Thanks ktownsend of the adafruit forms.
    // Based on the code from https://forums.adafruit.com/viewtopic.php?t=21536
    color.blue  = (word & 0x001F) << 3; // Left shift by 3
    color.green = (word & 0x03E0) >> 2; // Right shift by  4 and left shift by 2
    color.red   = (word & 0x7C00) >> 7; // Right shift by 10 and left shift by 3
    color.alpha = (word & 0x8000) != 0;
    
    return color.toGeneric( interpolation );
}

Utilities::PixelFormatColor_R8G8B8::Color::Color( Utilities::PixelFormatColor::GenericColor generic, ChannelInterpolation interpolate )
{
    PixelFormatColor_R8G8B8A8::Color other_color( generic, interpolate );
    
    red   = other_color.red;
    green = other_color.green;
    blue  = other_color.blue;
}

Utilities::PixelFormatColor::GenericColor Utilities::PixelFormatColor_R8G8B8::Color::toGeneric( ChannelInterpolation interpolate ) const {
    Utilities::PixelFormatColor_R8G8B8A8::Color expand;
    expand.red   = red;
    expand.green = green;
    expand.blue  = blue;
    expand.alpha = 255;
    
    return expand.toGeneric( interpolate );
}

void Utilities::PixelFormatColor_R8G8B8::writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& generic_color )
{
    PixelFormatColor_R8G8B8::Color color( generic_color, interpolation );
    
    buffer.writeU8( color.red );
    buffer.writeU8( color.green );
    buffer.writeU8( color.blue );
}

Utilities::PixelFormatColor::GenericColor Utilities::PixelFormatColor_R8G8B8::readPixel( Buffer::Reader &buffer, Buffer::Endian endian )
{
    PixelFormatColor_R8G8B8::Color color;
    
    color.red   = buffer.readU8();
    color.green = buffer.readU8();
    color.blue  = buffer.readU8();
    
    return color.toGeneric( interpolation );
}

Utilities::PixelFormatColor_R8G8B8A8::Color::Color( Utilities::PixelFormatColor::GenericColor generic, ChannelInterpolation interpolate )
{
    if( interpolate == sRGB ) {
        red   = pow( generic.red,   1.0 / SRGB_VALUE ) * MAX_UBYTE_VALUE + 0.5;
        green = pow( generic.green, 1.0 / SRGB_VALUE ) * MAX_UBYTE_VALUE + 0.5;
        blue  = pow( generic.blue,  1.0 / SRGB_VALUE ) * MAX_UBYTE_VALUE + 0.5;
    }
    else {
        red   = generic.red   * MAX_UBYTE_VALUE;
        green = generic.green * MAX_UBYTE_VALUE;
        blue  = generic.blue  * MAX_UBYTE_VALUE;
    }
    
    alpha = generic.alpha * MAX_UBYTE_VALUE;
}

Utilities::PixelFormatColor::GenericColor Utilities::PixelFormatColor_R8G8B8A8::Color::toGeneric( ChannelInterpolation interpolate ) const {
    GenericColor color;
    
    float   red_value = static_cast<channel_fp>(red);
    float green_value = static_cast<channel_fp>(green);
    float  blue_value = static_cast<channel_fp>(blue);
    
    if( interpolate == sRGB ) {
        color.red   = pow(   red_value, SRGB_VALUE ) / MAX_UBYTE_sRGB_VALUE;
        color.green = pow( green_value, SRGB_VALUE ) / MAX_UBYTE_sRGB_VALUE;
        color.blue  = pow(  blue_value, SRGB_VALUE ) / MAX_UBYTE_sRGB_VALUE;
    }
    else {
        color.red   =   red_value / MAX_UBYTE_VALUE;
        color.green = green_value / MAX_UBYTE_VALUE;
        color.blue  =  blue_value / MAX_UBYTE_VALUE;
    }
    
    color.alpha = static_cast<channel_fp>(alpha) / MAX_UBYTE_VALUE;
    
    return color;
}

void Utilities::PixelFormatColor_R8G8B8A8::writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& generic_color )
{
    PixelFormatColor_R8G8B8A8::Color color( generic_color, interpolation );
    
    buffer.writeU8( color.red );
    buffer.writeU8( color.green );
    buffer.writeU8( color.blue );
    buffer.writeU8( color.alpha );
}

Utilities::PixelFormatColor::GenericColor Utilities::PixelFormatColor_R8G8B8A8::readPixel( Buffer::Reader &buffer, Buffer::Endian endian )
{
    PixelFormatColor_R8G8B8A8::Color color;
    
    color.red   = buffer.readU8();
    color.green = buffer.readU8();
    color.blue  = buffer.readU8();
    color.alpha = buffer.readU8();
    
    return color.toGeneric( interpolation );
}

Utilities::ColorPalette::ColorPalette( const Utilities::PixelFormatColor& color_palette, Buffer::Endian endianess_param ) : color_p(nullptr), buffer(), endianess( endianess_param )
{
    color_p = dynamic_cast<PixelFormatColor*>( color_palette.duplicate() );
    
    assert( color_p != nullptr );
}

uint_fast16_t Utilities::ColorPalette::getTotalIndex() const
{
    return buffer.getReader().totalSize() / color_p->byteSize();
}

Utilities::PixelFormatColor::GenericColor Utilities::ColorPalette::getIndex( palette_index index ) const
{
    auto reader = buffer.getReader( index * color_p->byteSize(), color_p->byteSize() );
    
    return color_p->readPixel( reader, endianess );
}

bool Utilities::ColorPalette::setIndex( palette_index index, const PixelFormatColor::GenericColor color )
{
    if( index < getTotalIndex() ) {
        auto writer = buffer.getWriter( index * color_p->byteSize(), color_p->byteSize() );
        
        color_p->writePixel( writer, endianess, getIndex( index ) );
        
        return true;
    }
    return false;
}

Utilities::PixelFormatIndex::PixelFormatIndex( uint_fast8_t index_size_param ) : index_size( index_size_param ) {
    if( index_size == 0 )
        index_size = 1;
    else
    if( index_size <= 2 )
        index_size = 2;
};

void Utilities::PixelFormatIndex::writePixel( palette_index indexValue, Buffer::Writer &buffer, Buffer::Endian endian ) {
    if( index_size == 1 )
        buffer.writeU8( indexValue );
    else
    if( index_size == 2 )
        buffer.writeU16( indexValue, endian );
}

Utilities::palette_index Utilities::PixelFormatIndex::readPixel( Buffer::Reader &buffer, Buffer::Endian endian ) {
    if( index_size == 1 )
        return buffer.readU8();
    else
    if( index_size == 2 )
        return buffer.readU16( endian );
    return 0;
}

Utilities::PixelFormatBitIndex::PixelFormatBitIndex( uint_fast8_t bits_per_index_param ) : bits_per_index( bits_per_index_param )
{
    if( bits_per_index > 8 )
        bits_per_index = 8;
}

void Utilities::PixelFormatBitIndex::writePixel( uint8_t *buffer_r, uint_fast8_t shift, palette_index indexValue )
{
    uint_fast8_t byte_offset = 0;
    
    buffer_r[ byte_offset ] = 0;
    
    for( uint_fast8_t s = shift; s < shift + bits_per_index; s++ ) {
        if( (s % 8) == 0 ) {
            byte_offset++;
            buffer_r[ byte_offset ] = 0;
        }
        
        buffer_r[ byte_offset ] |= (1 << ((s - shift) % 8)) & indexValue;
    }
}

/*Utilities::palette_index Utilities::PixelFormatBitIndex::readPixel( Buffer::Reader &buffer, uint_fast8_t shift )
{
    if( index_size == 1 )
        return buffer.readU8();
    else
    if( index_size == 2 )
        return buffer.readU16();
    return 0;
}
*/
