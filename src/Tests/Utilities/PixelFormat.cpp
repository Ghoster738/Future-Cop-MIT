#include "../../Utilities/PixelFormat.h"
#include <iostream>
#include <string>

#include "../../Utilities/Image2D.h"
#include <glm/vec2.hpp>
#include "TestImage2D.h"
#include "TestPalette.h"

int testColorProfiles( Utilities::PixelFormatColor::ChannelInterpolation interpolate ) {
    int problem = 0;
    
    Utilities::PixelFormatColor::GenericColor generic;
    
    // Brute force will be used to test the various color formats.
    
    {
        Utilities::PixelFormatColor_W8 white( interpolate );
        Utilities::PixelFormatColor_W8::Color color[2];
        
        for( uint16_t i = 0; i <= 255; i++ )
        {
            color[0].white = i;
            
            generic = color[0].toGeneric( interpolate );
            color[1] = Utilities::PixelFormatColor_W8::Color( generic, interpolate );
            
            if( color[0].white != color[1].white )
            {
                std::cout << "The color conversion for PixelFormatColor_W8 test has failed!" << std::endl;
                std::cout << "  At " << i << " the colors do not match." << std::endl;
                std::cout << "  Color[0] " << static_cast<uint32_t>(color[0].white) << std::endl;
                std::cout << "  Color[1] " << static_cast<uint32_t>(color[1].white) << std::endl;
                std::cout << "Generic: "<< generic.getString() << std::endl;
                //i = 256;
                problem = 1;
            }
        }
    }
    
    {
        Utilities::PixelFormatColor_W8A8 white_alpha( interpolate );
        Utilities::PixelFormatColor_W8A8::Color color[2];
        
        for( uint16_t w = 0; w <= 255; w++ )
        {
            color[0].white = w;
            uint16_t a = 56;
            
            generic = color[0].toGeneric( interpolate );
            color[1] = Utilities::PixelFormatColor_W8A8::Color( generic, interpolate );
            
            if( color[0].white != color[1].white || color[0].alpha != color[1].alpha )
            {
                std::cout << "The color conversion for PixelFormatColor_W8A8 test has failed!" << std::endl;
                std::cout << "  At (" << w << ", " << a << ") the colors do not match." << std::endl;
                std::cout << "  Color[0] " << static_cast<uint32_t>(color[0].white) << ", " << static_cast<uint32_t>(color[0].alpha) << std::endl;
                std::cout << "  Color[1] " << static_cast<uint32_t>(color[1].white) << ", " << static_cast<uint32_t>(color[1].alpha) << std::endl;
                std::cout << "Generic: "<< generic.getString() << std::endl;
                w = 256;
                a = 256;
                problem = 1;
            }
        }
        
        for( uint16_t a = 0; a <= 255; a++ )
        {
            uint16_t w = 80;
            color[0].alpha = a;
            
            generic = color[0].toGeneric( interpolate );
            color[1] = Utilities::PixelFormatColor_W8A8::Color( generic, interpolate );
            
            if( color[0].white != color[1].white || color[0].alpha != color[1].alpha )
            {
                std::cout << "The color conversion for PixelFormatColor_W8A8 test has failed!" << std::endl;
                std::cout << "  At (" << w << ", " << a << ") the colors do not match." << std::endl;
                std::cout << "  Color[0] " << static_cast<uint32_t>(color[0].white) << ", " << static_cast<uint32_t>(color[0].alpha) << std::endl;
                std::cout << "  Color[1] " << static_cast<uint32_t>(color[1].white) << ", " << static_cast<uint32_t>(color[1].alpha) << std::endl;
                std::cout << "Generic: "<< generic.getString() << std::endl;
                w = 256;
                a = 256;
                problem = 1;
            }
        }
    }
    
    {
        Utilities::PixelFormatColor_R5G5B5A1 r5g5b5a1( interpolate );
        Utilities::PixelFormatColor_R5G5B5A1::Color color[2];
        
        for( uint16_t v = 0; v <= 31; v++ )
        {
            color[0].red = v;
            color[0].green = 15;
            color[0].blue  = 3;
            color[0].alpha = 0;
            
            generic = color[0].toGeneric( interpolate );
            color[1] = Utilities::PixelFormatColor_R5G5B5A1::Color( generic, interpolate );
            
            if( color[0].red != color[1].red || color[0].green != color[1].green || color[0].blue != color[1].blue || color[0].alpha != color[1].alpha )
            {
                std::cout << "The color conversion for PixelFormatColor_R5G5B5A1 test has failed!" << std::endl;
                std::cout << "  Color[0] changes on red." << std::endl;
                std::cout << "  Color[0] " << static_cast<uint32_t>(color[0].red) << ", " << static_cast<uint32_t>(color[0].green) << ", " << static_cast<uint32_t>(color[0].blue) << ", " << static_cast<uint32_t>(color[0].alpha) << std::endl;
                std::cout << "  Color[1] " << static_cast<uint32_t>(color[1].red) << ", " << static_cast<uint32_t>(color[1].green) << ", " << static_cast<uint32_t>(color[1].blue) << ", " << static_cast<uint32_t>(color[1].alpha) << std::endl;
                std::cout << "Generic: "<< generic.getString() << std::endl;
                v = 32;
                problem = 1;
            }
        }
        
        for( uint16_t v = 0; v <= 31; v++ )
        {
            color[0].red = 4;
            color[0].green = v;
            color[0].blue  = 3;
            color[0].alpha = 1;
            
            generic = color[0].toGeneric( interpolate );
            color[1] = Utilities::PixelFormatColor_R5G5B5A1::Color( generic, interpolate );
            
            if( color[0].red != color[1].red || color[0].green != color[1].green || color[0].blue != color[1].blue || color[0].alpha != color[1].alpha )
            {
                std::cout << "The color conversion for PixelFormatColor_R5G5B5A1 test has failed!" << std::endl;
                std::cout << "  Color[0] changes on green." << std::endl;
                std::cout << "  Color[0] " << static_cast<uint32_t>(color[0].red) << ", " << static_cast<uint32_t>(color[0].green) << ", " << static_cast<uint32_t>(color[0].blue) << ", " << static_cast<uint32_t>(color[0].alpha) << std::endl;
                std::cout << "  Color[1] " << static_cast<uint32_t>(color[1].red) << ", " << static_cast<uint32_t>(color[1].green) << ", " << static_cast<uint32_t>(color[1].blue) << ", " << static_cast<uint32_t>(color[1].alpha) << std::endl;
                std::cout << "Generic: "<< generic.getString() << std::endl;
                v = 32;
                problem = 1;
            }
        }
        
        for( uint16_t v = 0; v <= 31; v++ )
        {
            color[0].red = 4;
            color[0].green = 6;
            color[0].blue  = v;
            color[0].alpha = 1;
            
            generic = color[0].toGeneric( interpolate );
            color[1] = Utilities::PixelFormatColor_R5G5B5A1::Color( generic, interpolate );
            
            if( color[0].red != color[1].red || color[0].green != color[1].green || color[0].blue != color[1].blue || color[0].alpha != color[1].alpha )
            {
                std::cout << "The color conversion for PixelFormatColor_R5G5B5A1 test has failed!" << std::endl;
                std::cout << "  Color[0] changes on blue." << std::endl;
                std::cout << "  Color[0] " << static_cast<uint32_t>(color[0].red) << ", " << static_cast<uint32_t>(color[0].green) << ", " << static_cast<uint32_t>(color[0].blue) << ", " << static_cast<uint32_t>(color[0].alpha) << std::endl;
                std::cout << "  Color[1] " << static_cast<uint32_t>(color[1].red) << ", " << static_cast<uint32_t>(color[1].green) << ", " << static_cast<uint32_t>(color[1].blue) << ", " << static_cast<uint32_t>(color[1].alpha) << std::endl;
                std::cout << "Generic: "<< generic.getString() << std::endl;
                v = 32;
                problem = 1;
            }
        }
    }
    
    {
        Utilities::PixelFormatColor_R8G8B8 r8g8b8( interpolate );
        Utilities::PixelFormatColor_R8G8B8::Color color[2];
        
        for( uint16_t v = 0; v <= 255; v++ )
        {
            color[0].red = v;
            color[0].green = 156;
            color[0].blue  = 60;
            
            generic = color[0].toGeneric( interpolate );
            color[1] = Utilities::PixelFormatColor_R8G8B8::Color( generic, interpolate );
            
            if( color[0].red != color[1].red || color[0].green != color[1].green || color[0].blue != color[1].blue )
            {
                std::cout << "The color conversion for PixelFormatColor_R8G8B8 test has failed!" << std::endl;
                std::cout << "  Color[0] changes on red." << std::endl;
                std::cout << "  Color[0] " << static_cast<uint32_t>(color[0].red) << ", " << static_cast<uint32_t>(color[0].green) << ", " << static_cast<uint32_t>(color[0].blue) << std::endl;
                std::cout << "  Color[1] " << static_cast<uint32_t>(color[1].red) << ", " << static_cast<uint32_t>(color[1].green) << ", " << static_cast<uint32_t>(color[1].blue) << std::endl;
                std::cout << "Generic: "<< generic.getString() << std::endl;
                v = 256;
                problem = 1;
            }
        }
        
        for( uint16_t v = 0; v <= 255; v++ )
        {
            color[0].red = 100;
            color[0].green = v;
            color[0].blue  = 60;
            
            generic = color[0].toGeneric( interpolate );
            color[1] = Utilities::PixelFormatColor_R8G8B8::Color( generic, interpolate );
            
            if( color[0].red != color[1].red || color[0].green != color[1].green || color[0].blue != color[1].blue )
            {
                std::cout << "The color conversion for PixelFormatColor_R8G8B8 test has failed!" << std::endl;
                std::cout << "  Color[0] changes on green." << std::endl;
                std::cout << "  Color[0] " << static_cast<uint32_t>(color[0].red) << ", " << static_cast<uint32_t>(color[0].green) << ", " << static_cast<uint32_t>(color[0].blue) << std::endl;
                std::cout << "  Color[1] " << static_cast<uint32_t>(color[1].red) << ", " << static_cast<uint32_t>(color[1].green) << ", " << static_cast<uint32_t>(color[1].blue) << std::endl;
                std::cout << "Generic: "<< generic.getString() << std::endl;
                v = 256;
                problem = 1;
            }
        }
        
        for( uint16_t v = 0; v <= 255; v++ )
        {
            color[0].red = 100;
            color[0].green = 80;
            color[0].blue  = v;
            
            generic = color[0].toGeneric( interpolate );
            color[1] = Utilities::PixelFormatColor_R8G8B8::Color( generic, interpolate );
            
            if( color[0].red != color[1].red || color[0].green != color[1].green || color[0].blue != color[1].blue )
            {
                std::cout << "The color conversion for PixelFormatColor_R8G8B8 test has failed!" << std::endl;
                std::cout << "  Color[0] changes on blue." << std::endl;
                std::cout << "  Color[0] " << static_cast<uint32_t>(color[0].red) << ", " << static_cast<uint32_t>(color[0].green) << ", " << static_cast<uint32_t>(color[0].blue) << std::endl;
                std::cout << "  Color[1] " << static_cast<uint32_t>(color[1].red) << ", " << static_cast<uint32_t>(color[1].green) << ", " << static_cast<uint32_t>(color[1].blue) << std::endl;
                std::cout << "Generic: "<< generic.getString() << std::endl;
                v = 256;
                problem = 1;
            }
        }
    }
    
    {
        Utilities::PixelFormatColor_R8G8B8A8 r8g8b8( interpolate );
        Utilities::PixelFormatColor_R8G8B8A8::Color color[2];
        
        for( uint16_t v = 0; v <= 255; v++ )
        {
            color[0].red = v;
            color[0].green = 156;
            color[0].blue  = 60;
            color[0].alpha = 92;
            
            generic = color[0].toGeneric( interpolate );
            color[1] = Utilities::PixelFormatColor_R8G8B8A8::Color( generic, interpolate );
            
            if( color[0].red != color[1].red || color[0].green != color[1].green || color[0].blue != color[1].blue )
            {
                std::cout << "The color conversion for PixelFormatColor_R8G8B8 test has failed!" << std::endl;
                std::cout << "  Color[0] changes on red." << std::endl;
                std::cout << "  Color[0] " << static_cast<uint32_t>(color[0].red) << ", " << static_cast<uint32_t>(color[0].green) << ", " << static_cast<uint32_t>(color[0].blue)  << ", " << static_cast<uint32_t>(color[0].alpha) << std::endl;
                std::cout << "  Color[1] " << static_cast<uint32_t>(color[1].red) << ", " << static_cast<uint32_t>(color[1].green) << ", " << static_cast<uint32_t>(color[1].blue)  << ", " << static_cast<uint32_t>(color[1].alpha) << std::endl;
                std::cout << "Generic: "<< generic.getString() << std::endl;
                v = 256;
                problem = 1;
            }
        }
        
        for( uint16_t v = 0; v <= 255; v++ )
        {
            color[0].red = 100;
            color[0].green = v;
            color[0].blue  = 60;
            color[0].alpha = 92;
            
            generic = color[0].toGeneric( interpolate );
            color[1] = Utilities::PixelFormatColor_R8G8B8A8::Color( generic, interpolate );
            
            if( color[0].red != color[1].red || color[0].green != color[1].green || color[0].blue != color[1].blue )
            {
                std::cout << "The color conversion for PixelFormatColor_R8G8B8 test has failed!" << std::endl;
                std::cout << "  Color[0] changes on green." << std::endl;
                std::cout << "  Color[0] " << static_cast<uint32_t>(color[0].red) << ", " << static_cast<uint32_t>(color[0].green) << ", " << static_cast<uint32_t>(color[0].blue)  << ", " << static_cast<uint32_t>(color[0].alpha) << std::endl;
                std::cout << "  Color[1] " << static_cast<uint32_t>(color[1].red) << ", " << static_cast<uint32_t>(color[1].green) << ", " << static_cast<uint32_t>(color[1].blue)  << ", " << static_cast<uint32_t>(color[1].alpha) << std::endl;
                std::cout << "Generic: "<< generic.getString() << std::endl;
                v = 256;
                problem = 1;
            }
        }
        
        for( uint16_t v = 0; v <= 255; v++ )
        {
            color[0].red = 100;
            color[0].green = 80;
            color[0].blue  = v;
            color[0].alpha = 92;
            
            generic = color[0].toGeneric( interpolate );
            color[1] = Utilities::PixelFormatColor_R8G8B8A8::Color( generic, interpolate );
            
            if( color[0].red != color[1].red || color[0].green != color[1].green || color[0].blue != color[1].blue )
            {
                std::cout << "The color conversion for PixelFormatColor_R8G8B8 test has failed!" << std::endl;
                std::cout << "  Color[0] changes on blue." << std::endl;
                std::cout << "  Color[0] " << static_cast<uint32_t>(color[0].red) << ", " << static_cast<uint32_t>(color[0].green) << ", " << static_cast<uint32_t>(color[0].blue)  << ", " << static_cast<uint32_t>(color[0].alpha) << std::endl;
                std::cout << "  Color[1] " << static_cast<uint32_t>(color[1].red) << ", " << static_cast<uint32_t>(color[1].green) << ", " << static_cast<uint32_t>(color[1].blue)  << ", " << static_cast<uint32_t>(color[1].alpha) << std::endl;
                std::cout << "Generic: "<< generic.getString() << std::endl;
                v = 256;
                problem = 1;
            }
        }
        
        for( uint16_t v = 0; v <= 255; v++ )
        {
            color[0].red = 100;
            color[0].green = 80;
            color[0].blue  = 190;
            color[0].alpha = v;
            
            generic = color[0].toGeneric( interpolate );
            color[1] = Utilities::PixelFormatColor_R8G8B8A8::Color( generic, interpolate );
            
            if( color[0].red != color[1].red || color[0].green != color[1].green || color[0].blue != color[1].blue )
            {
                std::cout << "The color conversion for PixelFormatColor_R8G8B8 test has failed!" << std::endl;
                std::cout << "  Color[0] changes on alpha." << std::endl;
                std::cout << "  Color[0] " << static_cast<uint32_t>(color[0].red) << ", " << static_cast<uint32_t>(color[0].green) << ", " << static_cast<uint32_t>(color[0].blue)  << ", " << static_cast<uint32_t>(color[0].alpha) << std::endl;
                std::cout << "  Color[1] " << static_cast<uint32_t>(color[1].red) << ", " << static_cast<uint32_t>(color[1].green) << ", " << static_cast<uint32_t>(color[1].blue)  << ", " << static_cast<uint32_t>(color[1].alpha) << std::endl;
                std::cout << "Generic: "<< generic.getString() << std::endl;
                v = 256;
                problem = 1;
            }
        }
    }
    
    if( problem != 0 )
    {
        std::cout << "Before this point it used the ";
        
        switch( interpolate )
        {
            case Utilities::PixelFormatColor::ChannelInterpolation::LINEAR:
                std::cout << "linear";
                break;
            case Utilities::PixelFormatColor::ChannelInterpolation::sRGB:
                std::cout << "sRGB";
                break;
            default:
                std::cout << "ERROR Unregonized";
        }
        
        std::cout << " interpolation." << std::endl;
    }
    
    return problem;
}

int checkReadWriteOperation( Utilities::Buffer &pixel_buffer, const Utilities::PixelFormatColor::GenericColor generic, Utilities::PixelFormatColor &format, std::string display, uint_fast8_t pixel_size )
{
    int problem = 0;
    auto pixel_writer = pixel_buffer.getWriter();
    auto pixel_reader = pixel_buffer.getReader();
    
    if( pixel_size != format.byteSize() )
    {
        std::cout << "Error: the byte size for the pixels are invalid." << std::endl;
        std::cout << "  Expected Size: " << static_cast<unsigned>( pixel_size ) << std::endl;
        std::cout << "  Real Size: " << static_cast<unsigned>( format.byteSize() ) << std::endl;
        problem = 1;
    }
    
    format.writePixel( pixel_writer, Utilities::Buffer::Endian::NO_SWAP, generic );
    
    if( pixel_writer.getPosition() != format.byteSize() )
    {
        std::cout << "pixel_writer in bad position" << std::endl;
        std::cout << "    byte size: " << static_cast<int>( format.byteSize() ) << std::endl;
        std::cout << "    written: " << pixel_writer.getPosition() << std::endl;
        problem = 1;
    }
    
    pixel_writer.setPosition( 0, Utilities::Buffer::Direction::BEGIN );
    
    auto modified_generic = format.readPixel( pixel_reader );
    pixel_reader.setPosition( 0, Utilities::Buffer::Direction::BEGIN );
    
    format.writePixel( pixel_writer, Utilities::Buffer::Endian::NO_SWAP, modified_generic );
    
    if( pixel_writer.getPosition() != format.byteSize() )
    {
        std::cout << "pixel_writer in bad position" << std::endl;
        std::cout << "    byte size: " << static_cast<int>( format.byteSize() ) << std::endl;
        std::cout << "    written: " << pixel_writer.getPosition() << std::endl;
        problem = 1;
    }
    
    auto recovered_generic = format.readPixel( pixel_reader );
    
    testColor( problem, modified_generic, recovered_generic, display + " pixel operations broken!", "" );
    
    return problem;
}


bool checkColorPalette( Utilities::Buffer::Endian endianess = Utilities::Buffer::Endian::LITTLE) {
    Utilities::PixelFormatColor_R5G5B5A1 color;
    Utilities::ColorPalette color_palette( color, endianess );
    const auto FIRST_COLOR = Utilities::PixelFormatColor::GenericColor(1, 0, 0.5, 1);
    
    if( dynamic_cast<const Utilities::PixelFormatColor_R5G5B5A1*>( color_palette.getColorFormat() ) == nullptr )
    {
        std::cout << "The color format is all wrong! " << color_palette.getColorFormat()->getName() << std::endl;
        return false;
    }
    if( !color_palette.empty() )
    {
        std::cout << "The color palette is not empty when it should be." << std::endl;
        return false;
    }
    if( color_palette.setIndex( 0, FIRST_COLOR ) )
    {
        std::cout << "The color palette did set first color when it should." << std::endl;
        return false;
    }
    if( color_palette.getEndian() != endianess )
    {
        std::cout << "The color palette is not the correct endian " << color_palette.getEndian()
            << ", but " << endianess << std::endl;
        return false;
    }
    
    // Set the amount of pixels to be one.
    color_palette.setAmount( 1 );
    
    if( color_palette.empty() )
    {
        std::cout << "The color palette is empty when it should not be." << std::endl;
        return false;
    }
    if( color_palette.getLastIndex() != 0 )
    {
        std::cout << "The color palette's last index is not zero, but "
            << static_cast<unsigned>( color_palette.getLastIndex() ) << "." << std::endl;
        return false;
    }
    if( color_palette.setIndex( 1, FIRST_COLOR ) )
    {
        std::cout << "The color palette did not set second color when it should not have." << std::endl;
        return false;
    }
    if( !color_palette.setIndex( 0, FIRST_COLOR ) )
    {
        std::cout << "The color palette did not set first color when it should." << std::endl;
        return false;
    }
    if( testColor( 0, color_palette.getIndex( 0 ), FIRST_COLOR, "Palette test", "" ) )
        return false;
    if( color_palette.getReader().totalSize() != 2 )
    {
        std::cout << "The color palette's reader should be 2 not "
            << color_palette.getReader().totalSize() << "." << std::endl;
        return false;
    }
    if( color_palette.getReader( 0 ).totalSize() != 2 )
    {
        std::cout << "The color palette's pixel size should be 2 not "
            << color_palette.getReader( 0 ).totalSize() << "." << std::endl;
        return false;
    }
    
    auto palette = generateColorPalette();
    
    if( palette.size() != 256 ) {
        std::cout << "The expected size of the palette is not " << palette.size() << " rather than " << 256 << std::endl;
        return false;
    }
    
    color_palette.setAmount( 16 );
    
    for( unsigned i = 0; i < 16; i++ ) {
        if( !color_palette.setIndex( i, palette[ i ] ) )
        {
            std::cout << "The color palette did not set the color when it should not have at index " << i << std::endl;
            return false;
        }
    }
    for( unsigned i = 0; i < 16; i++ ) {
        std::string test_string = "Partial Palette at index " + std::to_string( i );
        if( testColor( 0, color_palette.getIndex( i ), palette[ i ], test_string, "" ) )
            return false;
    }
    if( color_palette.getLastIndex() != 15 )
    {
        std::cout << "The color palette's last index is not 15, but "
            << static_cast<unsigned>( color_palette.getLastIndex() ) << "." << std::endl;
        return false;
    }
    if( color_palette.getReader().totalSize() != 32 )
    {
        std::cout << "The color palette's reader should be 32 not "
            << color_palette.getReader().totalSize() << "." << std::endl;
        return false;
    }
    if( color_palette.getReader( 8 ).totalSize() != 2 )
    {
        std::cout << "The color palette's pixel size should be 2 not "
        << color_palette.getReader( 8 ).totalSize() << "." << std::endl;
        return false;
    }
    
    color_palette.setAmount( 8 );
    // The color palette should only lose the index values that are high.
    for( unsigned i = 0; i < 8; i++ ) {
        std::string test_string = "Half-Partial Palette at index " + std::to_string( i );
        if( testColor( 0, color_palette.getIndex( i ), palette[ i ], test_string, "" ) )
            return false;
    }
    // The size should change.
    if( color_palette.setIndex( 8, FIRST_COLOR ) )
    {
        std::cout << "Index 8 at Half-Partial Palete should not be written." << std::endl;
        return false;
    }
    if( color_palette.getLastIndex() != 7 )
    {
        std::cout << "The color palette's last index is not 8, but "
            << static_cast<unsigned>( color_palette.getLastIndex() ) << "." << std::endl;
        return false;
    }
    if( color_palette.getReader().totalSize() != 16 )
    {
        std::cout << "The color palette's reader should be 16 not "
            << color_palette.getReader().totalSize() << "." << std::endl;
        return false;
    }
    if( color_palette.getReader( 4 ).totalSize() != 2 )
    {
        std::cout << "The color palette's pixel size should be 3 not "
        << color_palette.getReader( 4 ).totalSize() << "." << std::endl;
        return false;
    }
    
    // Set to full amount.
    color_palette.setAmount( 256 );
    
    for( unsigned i = 8; i < 256; i++ ) {
        if( !color_palette.setIndex( i, palette[ i ] ) )
        {
            std::cout << "The color palette did not set the color when it should not have at index " << i << std::endl;
            return false;
        }
    }
    for( unsigned i = 0; i < 256; i++ ) {
        std::string test_string = "Full Palette at index " + std::to_string( i );
        if( testColor( 0, color_palette.getIndex( i ), palette[ i ], test_string, "" ) )
            return false;
    }
    if( color_palette.getLastIndex() != 255 )
    {
        std::cout << "The color palette's last index is not 256, but "
            << static_cast<unsigned>( color_palette.getLastIndex() ) << "." << std::endl;
        return false;
    }
    if( color_palette.getReader().totalSize() != 512 )
    {
        std::cout << "The color palette's reader should be 512 not "
            << color_palette.getReader().totalSize() << "." << std::endl;
        return false;
    }
    if( color_palette.getReader( 255 ).totalSize() != 2 )
    {
        std::cout << "The color palette's pixel size should be 2 not "
        << color_palette.getReader( 255 ).totalSize() << "." << std::endl;
        return false;
    }
    
    {
        const unsigned INDEX = 40;
        Utilities::Buffer buffer;
        
        buffer.addU16( 0xB260, endianess );
        
        auto expected_reader = buffer.getReader();
        auto tested_reader = color_palette.getReader( INDEX );
        
        auto expected_value = expected_reader.readU16();
        auto tested_value   =   tested_reader.readU16();
        
        if( expected_value != tested_value )
        {
            std::cout << "Endainess test had failed at " << INDEX << std::hex << std::endl;
            std::cout << "Expected Value 0x" << expected_value << std::endl;
            std::cout << "Tested Value 0x" << tested_value << std::endl;
            std::cout << std::dec;
            return false;
        }
    }
    
    Utilities::ColorPalette color_palette_2( color_palette );
    
    for( unsigned i = 0; i < 256; i++ ) {
        std::string test_string = "Palette Copy constructor failed at index " + std::to_string( i );
        if( testColor( 0, color_palette.getIndex( i ), palette[ i ], test_string, "" ) )
            return false;
    }
    
    return true;
}

int main() {
    int problem = 0;
    
    // Test the generic "from" and "to" operations to make sure that they can be
    // converted without data loss. If data is lost even when the pixel format is the same, then
    // there is something wrong with the conversion process.
    // These tests also tests to see if the generic color struct works.
    problem |= testColorProfiles( Utilities::PixelFormatColor::ChannelInterpolation::LINEAR );
    problem |= testColorProfiles( Utilities::PixelFormatColor::ChannelInterpolation::sRGB );
    
    // Test the write and read pixel operations
    {
        // Only one color is useful enough.
        Utilities::PixelFormatColor::GenericColor generic(0.125, 0.5, 0.8125, 0.75);
        
        const auto INTERPOLATE = Utilities::PixelFormatColor::ChannelInterpolation::LINEAR;
        
        Utilities::Buffer pixel_buffer;
        
        // Allocate enough memory for the pixel. For now 64 bit would due.
        pixel_buffer.addI32( 0 );
        pixel_buffer.addI32( 0 );
        
        // Test W8.
        {
            Utilities::PixelFormatColor_W8 format;
            problem |= checkReadWriteOperation( pixel_buffer, generic, format, "PixelFormatColor_W8", 1 );
        }
        
        // Test W8A8.
        {
            Utilities::PixelFormatColor_W8A8 format;
            problem |= checkReadWriteOperation( pixel_buffer, generic, format, "PixelFormatColor_W8A8", 2 );
        }
        
        // Test R5G5B5A1.
        {
            Utilities::PixelFormatColor_W8A8 format;
            problem |= checkReadWriteOperation( pixel_buffer, generic, format, "PixelFormatColor_R5G5B5A1", 2 );
            Utilities::PixelFormatColor::GenericColor generic(0.125, 0.5, 0.8125, 0.0);
            problem |= checkReadWriteOperation( pixel_buffer, generic, format, "PixelFormatColor_R5G5B5A1", 2 );
        }
        
        // Test R8G8B8.
        {
            Utilities::PixelFormatColor_R8G8B8 format;
            problem |= checkReadWriteOperation( pixel_buffer, generic, format, "PixelFormatColor_R8G8B8", 3 );
        }
        
        // Test R8G8B8A8.
        {
            Utilities::PixelFormatColor_R8G8B8A8 format;
            problem |= checkReadWriteOperation( pixel_buffer, generic, format, "PixelFormatColor_R8G8B8A8", 4 );
        }
    }
    
    if( !checkColorPalette( Utilities::Buffer::Endian::BIG ) ) {
        std::cout << "checkColorPalette() BIG has failed." << std::endl;
        return 1;
    }
    else
    if( !checkColorPalette( Utilities::Buffer::Endian::LITTLE ) ) {
        std::cout << "checkColorPalette() LITTLE has failed." << std::endl;
        return 1;
    }
    else
        return problem;
}
