#include "../../Utilities/ImagePalette2D.h"
#include <glm/vec2.hpp>
#include <iostream>

#include "TestImage2D.h"

bool checkColorPalette() {
    return false;
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
        return 1;
}
