#include "../../Utilities/ImagePalette2D.h"
#include <glm/vec2.hpp>
#include <bitset>
#include <iostream>

#include "TestImage2D.h"
#include "TestPalette.h"

// Sourced from wikipedia. https://en.wikipedia.org/wiki/Arecibo_message#Message_as_binary_string
// TODO This is for the bitset test for the ImagePalette2D class. 
const std::bitset<1> ARECIBO_MESSAGE( "00000010101010000000000001010000010100000001001000100010001001011001010101010101010100100100000000000000000000000000000000000001100000000000000000001101000000000000000000011010000000000000000001010100000000000000000011111000000000000000000000000000000001100001110001100001100010000000000000110010000110100011000110000110101111101111101111101111100000000000000000000000000100000000000000000100000000000000000000000000001000000000000000001111110000000000000111110000000000000000000000011000011000011100011000100000001000000000100001101000011000111001101011111011111011111011111000000000000000000000000001000000110000000001000000000001100000000000000010000011000000000011111100000110000001111100000000001100000000000001000000001000000001000001000000110000000100000001100001100000010000000000110001000011000000000000000110011000000000000011000100001100000000011000011000000100000001000000100000000100000100000001100000000100010000000011000000001000100000000010000000100000100000001000000010000000100000000000011000000000110000000011000000000100011101011000000000001000000010000000000000010000011111000000000000100001011101001011011000000100111001001111111011100001110000011011100000000010100000111011001000000101000001111110010000001010000011000000100000110110000000000000000000000000000000000011100000100000000000000111010100010101010101001110000000001010101000000000000000010100000000000000111110000000000000000111111111000000000000111000000011100000000011000000000001100000001101000000000101100000110011000000011001100001000101000001010001000010001001000100100010000000010001010001000000000000100001000010000000000001000000000100000000000000100101000000000001111001111101001111000" );

int testImage( Utilities::ImagePalette2D &image, const std::string &julia_image, const Utilities::ColorPalette& color_palette, Utilities::GridBase2D<float> test_julia_set ) {
    int problem = 0;
    
    if( image.getColorPalette() == &color_palette ) {
        // I am pretty sure this test will not fail.
        problem |= 1;
        std::cout << "Color Palette is supposed to be duplicated for " << julia_image << std::endl;
        return problem;
    }
    
    // Also make sure that the sizes match for the color palettes.
    if( image.getColorPalette()->getLastIndex() != color_palette.getLastIndex() ) {
        problem |= 1;
        std::cout << "Color Palette of the image does not have the same number of colors for " << julia_image << std::endl;
        std::cout << "  image palette " << static_cast<unsigned>( image.getColorPalette()->getLastIndex() ) << std::endl;
        std::cout << "  original palette " << static_cast<unsigned>( color_palette.getLastIndex() ) << std::endl;
        return problem;
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
    Utilities::GridBase2D<float> test_julia_set( 64, 64 );
    
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
    
    const std::string julia_image = "Julia Image Constructor( grid_2d_unit width, grid_2d_unit height, const ColorPalette& palette )";
    
    // Finally generate the image.
    Utilities::ImagePalette2D image( test_julia_set.getWidth(), test_julia_set.getHeight(), color_palette );
    
    // Generate and test
    problem |= testImage( image, julia_image, color_palette, test_julia_set );
    
    {
        Utilities::ImagePalette2D image_copy( image );
        
        const std::string julia_image = "Julia Image Constructor( const ImagePalette2D &image )";
        
        problem |= testImage( image_copy, julia_image, color_palette, test_julia_set );
    }
    { // Test Image2D generation.
        auto image_copy = image.toColorImage();
        const std::string unpaletted_image = "Unpaletted Image";
        
        problem |= testScale<Utilities::Image2D>( image_copy, image.getWidth(), image.getHeight(), unpaletted_image );
        
        problem |= compareImage2D<Utilities::ImagePalette2D, Utilities::Image2D>( image, image_copy, unpaletted_image );
    }
    { // Test MorbinImage2D generation.
        auto image_copy = image.toColorMorbinImage();
        const std::string unpaletted_image = "Unpaletted Morbin Image";
        
        problem |= testScale<Utilities::ImageMorbin2D>( image_copy, image.getWidth(), image.getHeight(), unpaletted_image );
        problem |= compareImage2D<Utilities::ImagePalette2D, Utilities::ImageMorbin2D>( image, image_copy, unpaletted_image );
    }
    /*
    {
        Utilities::ImagePaletteMorbin2D image_copy( image );
        
        const std::string julia_image = "Julia Image Constructor( const ImagePaletteMorbin2D &image )";
        
        problem |= testImage( image_copy, julia_image, color_palette, test_julia_set );
    } */
    {
        Utilities::ImagePalette2D image_copy( test_julia_set.getWidth() * 2, test_julia_set.getHeight() * 2, color_palette );
        
        const std::string inscribe_sub_image = "Inscribe Sub Image";
        
        // Draw the background.
        for( unsigned w = 0; w < image_copy.getWidth(); w++ ) {
            for( unsigned h = 0; h < image_copy.getHeight(); h++ ) {
                if( !image_copy.writePixel( w, h, 0 ) && problem == 0 ) {
                    problem |= 1;
                    std::cout << "Failure to write pixel at (" << w << ", " << h << ") for " << inscribe_sub_image << std::endl;
                }
            }
        }
        
        // Test the inscribe sub image methods.
        if( image_copy.inscribeSubImage( test_julia_set.getWidth() + 1, 0, image ) )
        {
            problem |= 1;
            std::cout << inscribe_sub_image << " inscribeSubImage() too much width method had succeeded." << std::endl;
        }
        if( image_copy.inscribeSubImage( 0, test_julia_set.getHeight() + 1, image ) )
        {
            problem |= 1;
            std::cout << inscribe_sub_image << " inscribeSubImage() too much hieght method had succeeded." << std::endl;
        }
        if( !image_copy.inscribeSubImage( test_julia_set.getWidth() / 2, test_julia_set.getHeight() / 2, image ) )
        {
            problem |= 1;
            std::cout << inscribe_sub_image << " inscribeSubImage() method had failed." << std::endl;
        }
        
        // Test for the Julia Set
        for( unsigned w = 0; w < test_julia_set.getWidth(); w++ ) {
            for( unsigned h = 0; h < test_julia_set.getHeight(); h++ ) {
                auto const SOURCE = image_copy.readPixel( test_julia_set.getWidth() / 2 + w, test_julia_set.getHeight() / 2 + h );
                auto const REFERENCE = image.readPixel( w, h );
                
                problem |= testColor( problem, SOURCE, REFERENCE, inscribe_sub_image, " (" + std::to_string(w) + ", " + std::to_string(h) + ")" );
            }
        }
        
        // Test the rest of the image_copy to see if they comprise of the first index.
        // Bascially, it scans everywhere except where the Julia Set is placed.
        
        // This spans from the width of the image_copy.
        for( unsigned w = 0; w < image_copy.getWidth(); w++ ) {
            // Test top of the image_copy until it lowers to the Julia Set.
            for( unsigned h = 0; h < test_julia_set.getHeight() / 2; h++ ) {
                if( problem == 0 && image_copy.getPixelIndex( w, h ) != 0 ) {
                    problem = 1;
                    std::cout << inscribe_sub_image << "( " << w << ", " << h << " ) = ";
                    std::cout << static_cast<unsigned>(image_copy.getPixelIndex( w, h )) << std::endl;
                }
            }
            // Test the area bellow the Julia Set.
            for( unsigned h = image_copy.getHeight() - test_julia_set.getHeight() / 2; h < image_copy.getHeight(); h++ ) {
                if( problem == 0 && image_copy.getPixelIndex( w, h ) != 0 ) {
                    problem = 1;
                    std::cout << inscribe_sub_image << "( " << w << ", " << h << " ) = ";
                    std::cout << static_cast<unsigned>(image_copy.getPixelIndex( w, h )) << std::endl;
                }
            }
        }
        
        // Test the sides of the sub_image.
        for( unsigned w = 0; w < test_julia_set.getWidth() / 2; w++ ) {
            // Test the left side of the area next to the Julia Set
            for( unsigned h = test_julia_set.getHeight() / 2; h < test_julia_set.getHeight() / 2 + test_julia_set.getHeight(); h++ ) {
                if( problem == 0 && image_copy.getPixelIndex( w, h ) != 0 ) {
                    problem = 1;
                    std::cout << inscribe_sub_image << "( " << w << ", " << h << " ) = ";
                    std::cout << static_cast<unsigned>(image_copy.getPixelIndex( w, h )) << std::endl;
                }
            }
            
            // Test the right side of the area next to the Julia Set.
            auto x = w + test_julia_set.getWidth() / 2 + test_julia_set.getWidth();
            
            for( unsigned h = test_julia_set.getHeight() / 2; h < test_julia_set.getHeight() / 2 + test_julia_set.getHeight(); h++ ) {
                
                if( problem == 0 && image_copy.getPixelIndex( x, h ) != 0 ) {
                    problem = 1;
                    std::cout << inscribe_sub_image << "( " << x << ", " << h << " ) = ";
                    std::cout << static_cast<unsigned>(image_copy.getPixelIndex( x, h )) << std::endl;
                }
            }
        }
        
        // Test the sub image operation.
        Utilities::ImagePalette2D sub_image( 0, 0, color_palette );
        
        const std::string sub_image_name = "Sub Image";
        
        if( image_copy.subImage( image_copy.getWidth() + 1, 0, test_julia_set.getWidth(), test_julia_set.getHeight(), sub_image ) ) {
            problem = 1;
            std::cout << sub_image_name << " subImage() method had succeeded when the sub image went out of bounds";
            std::cout << " on the x-axis." << std::endl;
        }
        if( image_copy.subImage( 0, image_copy.getHeight() + 1, test_julia_set.getWidth(), test_julia_set.getHeight(), sub_image ) ) {
            problem = 1;
            std::cout << sub_image_name << " subImage() method had succeeded when the sub image went out of bounds";
            std::cout << " on the y-axis." << std::endl;
        }
        
        if( !image_copy.subImage( test_julia_set.getWidth() / 2, test_julia_set.getHeight() / 2, test_julia_set.getWidth(), test_julia_set.getHeight(), sub_image ) ) {
            problem = 1;
            std::cout << sub_image_name << " subImage() method had failed when it was prefectly fine." << std::endl;
        }
        else
        {
            problem |= compareImage2D<Utilities::ImagePalette2D>( sub_image, image, sub_image_name );
        }
    }
    
    
    return problem;
}
