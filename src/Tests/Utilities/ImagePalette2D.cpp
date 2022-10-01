#include "../../Utilities/ImagePalette2D.h"
#include <glm/vec2.hpp>
#include <iostream>

#include "TestImage2D.h"
#include "TestPalette.h"

int main() {
    int problem = 0;
    
    Utilities::PixelFormatColor_R8G8B8 color;
    Utilities::ColorPalette color_palette( color );
    
    Utilities::ImagePalette2D image( 16, 16, color_palette );
    
    return problem;
}
