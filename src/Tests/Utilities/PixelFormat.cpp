#include "../../Utilities/PixelFormat.h"
#include <iostream>

int testColorProfiles( Utilities::PixelFormatColor::ChannelInterpolation interpolate ) {
    int problem = 0;
    
    Utilities::PixelFormatColor::GenericColor generic;
    
    Utilities::PixelFormatColor_W8       white( interpolate );
    Utilities::PixelFormatColor_W8A8     white_alpha( interpolate );
    Utilities::PixelFormatColor_R5G5B5A1 r5g5b5a1( interpolate );
    Utilities::PixelFormatColor_R8G8B8   r8g8b8( interpolate );
    Utilities::PixelFormatColor_R8G8B8A8 r8g8b8a8( interpolate );
    
    {
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
                i = 255;
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

int main() {
    int problem = 0;
    
    problem |= testColorProfiles( Utilities::PixelFormatColor::ChannelInterpolation::LINEAR );
    problem |= testColorProfiles( Utilities::PixelFormatColor::ChannelInterpolation::sRGB );
    
    return problem;
}
