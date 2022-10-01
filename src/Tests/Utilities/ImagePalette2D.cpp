#include "../../Utilities/ImagePalette2D.h"
#include <glm/vec2.hpp>
#include <iostream>

#include "TestImage2D.h"

void transitionColor( std::vector<Utilities::PixelFormatColor::GenericColor> &generated, size_t offset ) {
    Utilities::PixelFormatColor::GenericColor color[2];
    Utilities::PixelFormatColor::GenericColor new_color;
    
    color[ 0 ] = generated.at( offset );
    color[ 1 ] = generated.at( (offset + 64) % 0x100 );
    
    for( unsigned i = 1; i < 64; i++ )
    {
        float factor = ((float)i) / 64.0;
        
        new_color.red   = color[ 0 ].red   * (1.0f - factor) + color[1].red   * factor;
        new_color.green = color[ 0 ].green * (1.0f - factor) + color[1].green * factor;
        new_color.blue  = color[ 0 ].blue  * (1.0f - factor) + color[1].blue  * factor;
        new_color.alpha = color[ 0 ].alpha * (1.0f - factor) + color[1].alpha * factor;
        
        generated[ i + offset ] = new_color;
    }
}

std::vector<Utilities::PixelFormatColor::GenericColor> generateColorPalette()
{
    std::vector<Utilities::PixelFormatColor::GenericColor> generated( 0x100 );
    
    generated.at(   0 ) = Utilities::PixelFormatColor::GenericColor( 1, 0, 0, 1 );
    generated.at(  64 ) = Utilities::PixelFormatColor::GenericColor( 0, 1, 0, 1 );
    generated.at( 128 ) = Utilities::PixelFormatColor::GenericColor( 0, 1, 1, 1 );
    generated.at( 192 ) = Utilities::PixelFormatColor::GenericColor( 0, 0, 1, 1 );
    
    transitionColor( generated, 0 );
    transitionColor( generated, 64 );
    transitionColor( generated, 128 );
    transitionColor( generated, 192 );
    
    for( unsigned i = 0; i < 256; i++ ) {
        std::cout << "P[" << i << "] = " << generated[ i ].getString() << std::endl;
    }
    
    return generated;
}

bool checkColorPalette() {
    Utilities::PixelFormatColor_R8G8B8 color;
    Utilities::ColorPalette color_palette( color );
    const auto FIRST_COLOR = Utilities::PixelFormatColor::GenericColor(1, 0, 0.5, 1);
    
    if( dynamic_cast<const Utilities::PixelFormatColor_R8G8B8*>( &color_palette.getColorFormat() ) == nullptr )
    {
        std::cout << "The color format is all wrong! " << color_palette.getColorFormat().getName() << std::endl;
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
    generateColorPalette();
    
    return false;
    return true;
}

int main() {
    int problem = 0;
    
    if( checkColorPalette() )
    {
        Utilities::PixelFormatColor_R8G8B8 color;
        Utilities::ColorPalette color_palette( color );
        
        Utilities::ImagePalette2D image( 16, 16, color_palette );
        
        return problem;
    }
    else
    {
        std::cout << "checkColorPalette() has failed." << std::endl;
        return 1;
    }
}
