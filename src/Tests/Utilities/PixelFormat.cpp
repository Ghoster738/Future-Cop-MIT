#include "../../Utilities/PixelFormat.h"
#include <iostream>
#include <string>

void printGeneric( const Utilities::PixelFormatColor::GenericColor generic ) {
    std::cout << "  Generic ( " << generic.red << ", " << generic.green << ", " << generic.blue << ", " << generic.alpha << " )"<< std::endl;
}

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
                printGeneric( generic );
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
                printGeneric( generic );
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
                printGeneric( generic );
                w = 256;
                a = 256;
                problem = 1;
            }
        }
    }
    
    {
        Utilities::PixelFormatColor_R5G5B5A1 r5g5b5a1( interpolate );
        Utilities::PixelFormatColor_R5G5B5A1::Color color[2];
        
        for( uint16_t v = 0; v <= 255; v++ )
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
                printGeneric( generic );
                v = 256;
                problem = 1;
            }
        }
    }
    
    Utilities::PixelFormatColor_R8G8B8   r8g8b8( interpolate );
    Utilities::PixelFormatColor_R8G8B8A8 r8g8b8a8( interpolate );
    
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

int main() {
    int problem = 0;
    
    problem |= testColorProfiles( Utilities::PixelFormatColor::ChannelInterpolation::LINEAR );
    problem |= testColorProfiles( Utilities::PixelFormatColor::ChannelInterpolation::sRGB );
    
    return problem;
}
