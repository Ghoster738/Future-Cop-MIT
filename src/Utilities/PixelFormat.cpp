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

std::string Utilities::PixelFormatColor::GenericColor::getString() const {
    return "(" + std::to_string( red ) + ", " + std::to_string( green ) + ", " + std::to_string( blue ) + ", " + std::to_string( alpha ) + ")";
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

void Utilities::PixelFormatColor_W8::writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& generic_color ) const
{
    PixelFormatColor_W8::Color color( generic_color, interpolation );
    
    buffer.writeU8( color.white );
}

Utilities::PixelFormatColor::GenericColor Utilities::PixelFormatColor_W8::readPixel( Buffer::Reader &buffer, Buffer::Endian endian ) const
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
        white = generic.red * MAX_UBYTE_VALUE + 0.5;
    
    alpha = generic.alpha * MAX_UBYTE_VALUE + 0.5;
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

void Utilities::PixelFormatColor_W8A8::writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& generic_color ) const
{
    PixelFormatColor_W8A8::Color color( generic_color, interpolation );
    
    buffer.writeU8( color.white );
    buffer.writeU8( color.alpha );
}

Utilities::PixelFormatColor::GenericColor Utilities::PixelFormatColor_W8A8::readPixel( Buffer::Reader &buffer, Buffer::Endian endian ) const
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
        red   = generic.red   * MAX_U5BIT_VALUE + 0.5;
        green = generic.green * MAX_U5BIT_VALUE + 0.5;
        blue  = generic.blue  * MAX_U5BIT_VALUE + 0.5;
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

void Utilities::PixelFormatColor_R5G5B5A1::writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& generic_color ) const
{
    PixelFormatColor_R5G5B5A1::Color color( generic_color, interpolation );
    
    uint16_t data;
    data  = (color.alpha << 15);
    data |= (color.red   << 10);
    data |= (color.green <<  5);
    data |= (color.blue  <<  0);
    
    buffer.writeU16( data, endian );
}

Utilities::PixelFormatColor::GenericColor Utilities::PixelFormatColor_R5G5B5A1::readPixel( Buffer::Reader &buffer, Buffer::Endian endian  ) const
{
    PixelFormatColor_R5G5B5A1::Color color;
    auto word = buffer.readU16( endian );
    
    // Thanks ktownsend of the adafruit forms.
    // Based on the code from https://forums.adafruit.com/viewtopic.php?t=21536
    color.blue  = (word & 0x001F);
    color.green = (word & 0x03E0) >>  5; // Right shift by  4 and left shift by 2
    color.red   = (word & 0x7C00) >> 10; // Right shift by 10 and left shift by 3
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

void Utilities::PixelFormatColor_R8G8B8::writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& generic_color ) const
{
    PixelFormatColor_R8G8B8::Color color( generic_color, interpolation );
    
    buffer.writeU8( color.red );
    buffer.writeU8( color.green );
    buffer.writeU8( color.blue );
}

Utilities::PixelFormatColor::GenericColor Utilities::PixelFormatColor_R8G8B8::readPixel( Buffer::Reader &buffer, Buffer::Endian endian ) const
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
        red   = generic.red   * MAX_UBYTE_VALUE + 0.5;
        green = generic.green * MAX_UBYTE_VALUE + 0.5;
        blue  = generic.blue  * MAX_UBYTE_VALUE + 0.5;
    }
    
    alpha = generic.alpha * MAX_UBYTE_VALUE + 0.5;
}

Utilities::PixelFormatColor::GenericColor Utilities::PixelFormatColor_R8G8B8A8::Color::toGeneric( ChannelInterpolation interpolate ) const {
    GenericColor color;
    
    auto   red_value = static_cast<channel_fp>(red);
    auto green_value = static_cast<channel_fp>(green);
    auto  blue_value = static_cast<channel_fp>(blue);
    
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

void Utilities::PixelFormatColor_R8G8B8A8::writePixel( Buffer::Writer &buffer, Buffer::Endian endian, const PixelFormatColor::GenericColor& generic_color ) const
{
    PixelFormatColor_R8G8B8A8::Color color( generic_color, interpolation );
    
    buffer.writeU8( color.red );
    buffer.writeU8( color.green );
    buffer.writeU8( color.blue );
    buffer.writeU8( color.alpha );
}

Utilities::PixelFormatColor::GenericColor Utilities::PixelFormatColor_R8G8B8A8::readPixel( Buffer::Reader &buffer, Buffer::Endian endian ) const
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

bool Utilities::ColorPalette::empty() const {
    if( buffer.getReader().totalSize() < color_p->byteSize() )
        return true;
    else
        return false;
}

uint_fast8_t Utilities::ColorPalette::getLastIndex() const
{
    if( empty() )
        return 0;
    else
        return buffer.getReader().totalSize() / color_p->byteSize() - 1;
}

Utilities::PixelFormatColor::GenericColor Utilities::ColorPalette::getIndex( palette_index index ) const
{
    auto reader = buffer.getReader( index * color_p->byteSize(), color_p->byteSize() );
    
    return color_p->readPixel( reader, endianess );
}

bool Utilities::ColorPalette::setIndex( palette_index index, const PixelFormatColor::GenericColor color )
{
    if( index <= getLastIndex() ) {
        auto writer = buffer.getWriter( index * color_p->byteSize(), color_p->byteSize() );
        
        color_p->writePixel( writer, endianess, getIndex( index ) );
        
        return true;
    }
    return false;
}
