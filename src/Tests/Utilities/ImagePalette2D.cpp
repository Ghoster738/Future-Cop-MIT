#include "../../Utilities/ImagePalette2D.h"
#include <glm/vec2.hpp>
#include <iostream>

#include "TestImage2D.h"
#include "TestPalette.h"

int main() {
    int problem = 0;
    
    Utilities::PixelFormatColor_R5G5B5A1 color;
    Utilities::ColorPalette color_palette( color );
    
    // Setup the color palette
    {
        auto colors = generateColorPalette();
        
        if( !color_palette.setAmount( colors.size() ) )
        {
            problem = 1;
            std::cout << "Color Palette generation failed!" << std::endl;
            std::cout << "  Could not resize to " << colors.size() << std::endl;
        }
        else
        {
            for( size_t i = 0; i < colors.size(); i++ ) {
                if( !color_palette.setIndex( i, colors[ i ] ) && problem == 0 )
                {
                    problem = 1;
                    std::cout << "setIndex at " << i << " has failed" << std::endl;
                    std::cout << " at this color: " << colors[ i ].getString() << std::endl;
                }
            }
        }
    }
    
    const unsigned JULIA_WIDTH = 16, JULIA_HEIGHT = 16;
    
    // Generate the julia set first.
    Utilities::GridBase2D<float> test_julia_set( JULIA_WIDTH, JULIA_HEIGHT );
    
    {
        const glm::vec2 RES_VEC( JULIA_WIDTH, JULIA_HEIGHT );
        
        for( unsigned w = 0; w < JULIA_WIDTH; w++ ) {
            for( unsigned h = 0; h < JULIA_HEIGHT; h++ ) {
                test_julia_set.setValue( w, h, juliaFractal( glm::vec2( w, h ) / RES_VEC * glm::vec2( 0.2 ) ) );
            }
        }
    }
    
    Utilities::ImagePalette2D image( JULIA_WIDTH, JULIA_HEIGHT, color_palette );
    
    return problem;
}
