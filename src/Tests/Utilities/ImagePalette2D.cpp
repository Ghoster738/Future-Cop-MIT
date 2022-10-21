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
            problem |= 1;
            std::cout << "Color Palette generation failed!" << std::endl;
            std::cout << "  Could not resize to " << colors.size() << std::endl;
        }
        else
        {
            for( size_t i = 0; i < colors.size(); i++ ) {
                if( !color_palette.setIndex( i, colors[ i ] ) && problem == 0 )
                {
                    problem |= 1;
                    std::cout << "setIndex at " << i << " has failed" << std::endl;
                    std::cout << " at this color: " << colors[ i ].getString() << std::endl;
                }
            }
        }
    }
    
    // Generate the julia set.
    Utilities::GridBase2D<float> test_julia_set( 16, 16 );
    
    {
        const glm::vec2 RES_VEC( test_julia_set.getWidth(), test_julia_set.getHeight() );
        
        auto max = std::numeric_limits<float>::min();
        auto min = std::numeric_limits<float>::max();
        
        for( unsigned w = 0; w < test_julia_set.getWidth(); w++ ) {
            for( unsigned h = 0; h < test_julia_set.getHeight(); h++ ) {
                test_julia_set.setValue( w, h, juliaFractal( glm::vec2( w, h ) / RES_VEC * glm::vec2( 0.2 ) ) );
                
                max = std::max( test_julia_set.getValue( w, h ), max );
                min = std::min( test_julia_set.getValue( w, h ), min );
            }
        }
        
        // The normalization is needed to test all the colors.
        const auto expand = 1 / (max - min);
        
        for( unsigned w = 0; w < test_julia_set.getWidth(); w++ ) {
            for( unsigned h = 0; h < test_julia_set.getHeight(); h++ ) {
                auto value = (test_julia_set.getValue( w, h ) - min) * expand;
                
                test_julia_set.setValue( w, h, value );
            }
        }
    }
    
    const std::string julia_image = "Julia Image";
    
    // Finally generate the image.
    Utilities::ImagePalette2D image( test_julia_set.getWidth(), test_julia_set.getHeight(), color_palette );
    
    if( image.getColorPalette() == &color_palette ) {
        // I am pretty sure this test will not fail.
        problem |= 1;
        std::cout << "Color Palette is supposed to be duplicated for " << julia_image << std::endl;
    }
    
    // Also make sure that the sizes match for the color palettes.
    if( image.getColorPalette()->getLastIndex() != color_palette.getLastIndex() ) {
        problem |= 1;
        std::cout << "Color Palette of the image does not have the same number of colors for " << julia_image << std::endl;
        std::cout << "  image palette " << static_cast<unsigned>( image.getColorPalette()->getLastIndex() ) << std::endl;
        std::cout << "  original palette " << static_cast<unsigned>( color_palette.getLastIndex() ) << std::endl;
    }
    
    problem |= testScale<Utilities::ImagePalette2D>( image, test_julia_set.getWidth(), test_julia_set.getHeight(), julia_image );
    
    // Apply the julia set to this image.
    {
        auto const MAX_INDEX = static_cast<float>( color_palette.getLastIndex() );
        
        for( unsigned w = 0; w < image.getWidth(); w++ ) {
            for( unsigned h = 0; h < image.getHeight(); h++ ) {
                auto const VALUE = test_julia_set.getValue( w, h ) * MAX_INDEX;
                
                if( !image.writePixel( w, h, VALUE ) && problem == 0 ) {
                    problem |= 1;
                    std::cout << "Failure to write pixel at (" << w << ", " << h << ")" << std::endl;
                }
            }
        }
    }
    
    // Test the pixels
    {
        auto const MAX_INDEX = static_cast<float>( color_palette.getLastIndex() );
        
        for( unsigned w = 0; w < image.getWidth(); w++ ) {
            for( unsigned h = 0; h < image.getHeight(); h++ ) {
                auto const VALUE = test_julia_set.getValue( w, h ) * MAX_INDEX;
                
                auto const SOURCE    = image.readPixel( w, h );
                auto const REFERENCE = color_palette.getIndex( VALUE );
                
                problem |= testColor( problem, SOURCE, REFERENCE, julia_image, " (" + std::to_string(w) + ", " + std::to_string(h) + ")" );
            }
        }
    }
    
    
    
    return problem;
}
