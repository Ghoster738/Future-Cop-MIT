#include "../../Utilities/ImagePalette2D.h"
#include <glm/vec2.hpp>
#include <bitset>
#include <iostream>

#include "TestImage2D.h"
#include "TestPalette.h"

// Sourced from wikipedia. https://en.wikipedia.org/wiki/Arecibo_message#Message_as_binary_string
// TODO This is for the bitset test for the ImagePalette2D class. 
const std::vector<bool> ARECIBO_MESSAGE( {0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,1,0,1,1,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,1,0,0,0,1,1,0,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,1,1,0,1,0,0,0,1,1,0,0,0,1,1,0,0,0,0,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,0,1,0,0,0,0,1,1,0,0,0,1,1,1,0,0,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,1,1,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,1,1,1,0,1,0,0,1,0,1,1,0,1,1,0,0,0,0,0,0,1,0,0,1,1,1,0,0,1,0,0,1,1,1,1,1,1,1,0,1,1,1,0,0,0,0,1,1,1,0,0,0,0,0,1,1,0,1,1,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,1,1,0,1,1,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,1,1,1,1,1,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,1,0,0,0,0,0,0,1,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1,1,0,1,0,0,1,1,1,1,0,0,0} );

template<class T>
int testImage( T &image, const std::string &julia_image, const Utilities::ColorPalette& color_palette, Utilities::GridBase2D<float> test_julia_set ) {
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
    
    problem |= testScale<T>( image, test_julia_set.getWidth(), test_julia_set.getHeight(), julia_image );
    
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

int compareBuffer( Utilities::Buffer::Reader source, Utilities::Buffer::Reader copy, std::string name )
{
    int problem = 0;
    
    source.setPosition( 0 );
    copy.setPosition( 0 );
    
    if( source.totalSize() != copy.totalSize() ) {
        std::cout << "Wrong size for " << name << std::endl;
        return 1;
    }
    
    while( !source.ended() && !copy.ended() ) {
        auto source_byte = source.readU8();
        auto copy_byte   =   copy.readU8();
        
        if( problem == 0 && source_byte != copy_byte ) {
            std::cout << name << " addToBuffer()!" << std::endl;
            std::cout << "  Position: " << source.getPosition() - 1 << std::endl;
            std::cout << "  Source:   " << static_cast<unsigned>( source_byte ) << std::endl;
            std::cout << "  Copy:     " << static_cast<unsigned>( copy_byte )   << std::endl;
            problem = 1;
        }
    }
    
    return problem;
}

template<class T, unsigned width, unsigned height>
int testPalette2D( const Utilities::ColorPalette &color_palette, std::string name ) {
    int problem = 0;
    
    // Generate the julia set.
    Utilities::GridBase2D<float> test_julia_set( width, height );
    
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
    
    const std::string julia_image = "Julia Image " + name + "Constructor( grid_2d_unit width, grid_2d_unit height, const ColorPalette& palette )";
    
    // Finally generate the image.
    T image( test_julia_set.getWidth(), test_julia_set.getHeight(), color_palette );
    
    // Generate and test
    problem |= testImage( image, julia_image, color_palette, test_julia_set );
    
    {
        T image_copy( image );
        
        const std::string julia_image = "Julia Image " + name + "Constructor( const ImagePalette2D &image )";
        
        problem |= testImage( image_copy, julia_image, color_palette, test_julia_set );
    }
    { // Test Image2D generation.
        auto image_copy = image.toColorImage();
        const std::string unpaletted_image = "Unpaletted Image " + name + " ";
        
        problem |= testScale<Utilities::Image2D>( image_copy, image.getWidth(), image.getHeight(), unpaletted_image );
        
        problem |= compareImage2D<T, Utilities::Image2D>( image, image_copy, unpaletted_image );
    }
    {
        T image_copy( test_julia_set.getWidth() * 2, test_julia_set.getHeight() * 2, color_palette );
        
        const std::string inscribe_sub_image = "Inscribe Sub " + name + "Image";
        
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
            std::cout << inscribe_sub_image << " inscribeSubImage() too much height method had succeeded." << std::endl;
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
        T sub_image( 0, 0, color_palette );
        
        const std::string sub_image_name = "Sub " + name + "Image";
        
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
            problem |= compareImage2D<T>( sub_image, image, sub_image_name );
        }
    }
    
    return problem;
}

template<class T>
int compareColor( int problem, int index, const Utilities::ColorPalette &color_palette, const T &image, unsigned x, unsigned y, std::string name ) {
    if( color_palette.getIndex( index ).getDistanceSq( image.readPixel(x, y) ) > 0.03125 )
    {
        std::cout << name << " (" << x << ", " << y << ") has wrong pixels" << std::endl;
        problem |= 1;
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
    
    problem |= testPalette2D<Utilities::ImagePalette2D, 50, 100>( color_palette, "" );
    
    {
        std::string arecibo_name = "Arecibo Image";
        Utilities::PixelFormatColor_R5G5B5A1 color;
        Utilities::ColorPalette color_palette( color );
        
        color_palette.setAmount( 2 );
        
        color_palette.setIndex( 0, Utilities::PixelFormatColor::GenericColor( 0.0f, 0.0f, 0.0f, 1.0f ) );
        color_palette.setIndex( 1, Utilities::PixelFormatColor::GenericColor( 1.0f, 1.0f, 1.0f, 1.0f ) );
        
        Utilities::ImagePalette2D arecibo_image( 23, 73, color_palette );
        
        if( !arecibo_image.fromBitfield( ARECIBO_MESSAGE ) ) {
            std::cout << arecibo_name << " bitfield failed to form." << std::endl;
            problem |= 1;
        }
        
        size_t pixel_position = 0;
        
        // Now check for a single flaw in the Arecibo Message.
        for( size_t y = 0; y < arecibo_image.getHeight(); y++ ) {
            for( size_t x = 0; x < arecibo_image.getWidth(); x++) {
                auto color = color_palette.getIndex( ARECIBO_MESSAGE[ pixel_position ] );
                
                if( problem == 0 && color.getDistanceSq( arecibo_image.readPixel(x, y) ) > 0.03125 ) {
                    std::cout << arecibo_name << " has wrong pixels" << std::endl;
                    problem |= 1;
                }
                
                pixel_position++;
            }
        }
    }
    {
        Utilities::ColorPalette color_palette( color );
        
        color_palette.setAmount( 8 );
        
        color_palette.setIndex( 0, Utilities::PixelFormatColor::GenericColor( 0.0f, 0.0f, 0.0f, 1.0f ) );
        color_palette.setIndex( 1, Utilities::PixelFormatColor::GenericColor( 1.0f, 0.0f, 0.0f, 1.0f ) );
        color_palette.setIndex( 2, Utilities::PixelFormatColor::GenericColor( 1.0f, 1.0f, 0.0f, 1.0f ) );
        color_palette.setIndex( 3, Utilities::PixelFormatColor::GenericColor( 0.0f, 1.0f, 0.0f, 1.0f ) );
        color_palette.setIndex( 4, Utilities::PixelFormatColor::GenericColor( 0.0f, 1.0f, 1.0f, 1.0f ) );
        color_palette.setIndex( 5, Utilities::PixelFormatColor::GenericColor( 0.0f, 0.0f, 1.0f, 1.0f ) );
        color_palette.setIndex( 6, Utilities::PixelFormatColor::GenericColor( 1.0f, 0.0f, 1.0f, 1.0f ) );
        color_palette.setIndex( 7, Utilities::PixelFormatColor::GenericColor( 1.0f, 1.0f, 1.0f, 1.0f ) );
        
        std::string simple_rect_name = "Simple Rect Image";
        const std::vector<bool> SIMPLE_RECT(
        {
            0, 0, 0,  0, 0, 1,  0, 1, 0,  0, 1, 1,
            1, 0, 0,  1, 0, 1,  1, 1, 0,  1, 1, 1
        } );
        
        {
            Utilities::ImagePalette2D simple_rect_image( 4, 4, color_palette );
            
            if( simple_rect_image.fromBitfield( SIMPLE_RECT, 3 ) ) {
                std::cout << simple_rect_name << " bitfield should have not succeeded." << std::endl;
                problem |= 1;
            }
        }
        
        Utilities::ImagePalette2D simple_rect_image( 4, 2, color_palette );
        
        if( simple_rect_image.fromBitfield( SIMPLE_RECT, 0 ) ) {
            std::cout << simple_rect_name << " bitfield succeed at 0." << std::endl;
            problem |= 1;
        }
        if( simple_rect_image.fromBitfield( SIMPLE_RECT, 9 ) ) {
            std::cout << simple_rect_name << " bitfield succeed at 9." << std::endl;
            problem |= 1;
        }
        if( !simple_rect_image.fromBitfield( SIMPLE_RECT, 3 ) ) {
            std::cout << simple_rect_name << " bitfield failed to form." << std::endl;
            problem |= 1;
        }
        
        size_t pixel_position = 0;
        
        // Now check for a single flaw in the simple rect
        for( size_t y = 0; y < simple_rect_image.getHeight(); y++ ) {
            for( size_t x = 0; x < simple_rect_image.getWidth(); x++) {
                auto color = color_palette.getIndex( pixel_position );
                
                if( problem == 0 ) {
                    if( color.getDistanceSq( simple_rect_image.readPixel(x, y) ) > 0.03125 )
                    {
                        std::cout << simple_rect_name << " has wrong pixels" << std::endl;
                        problem |= 1;
                    }
                    if( pixel_position != simple_rect_image.getPixelIndex(x, y) ) {
                        std::cout << "(" << x << ", " << y << ") = " << static_cast<unsigned>( simple_rect_image.getPixelIndex(x, y) ) << " but not " << pixel_position << std::endl;
                        problem |= 1;
                    }
                }
                
                pixel_position++;
            }
        }
        
        std::string simple_rect_buffer_name = "Simple Rect Buffer Image";
        Utilities::Buffer simple_rect_buffer;
        
        Utilities::ImagePalette2D simple_rect_buff_image(
            simple_rect_image.getWidth(), simple_rect_image.getHeight(), color_palette );
        
        {// This is a fail condition!
            auto reader = simple_rect_buffer.getReader();
            
            if( simple_rect_buff_image.fromReader( reader ) ) {
                std::cout << simple_rect_buffer_name << " fromReader has succeeded when it should have failed!" << std::endl;
                problem |= 1;
            }
        }
        
        for( size_t i = 0; i < simple_rect_image.getHeight() * simple_rect_image.getWidth(); i++ )
            simple_rect_buffer.addU8( i );
        
        auto reader = simple_rect_buffer.getReader();
        
        if( !simple_rect_buff_image.fromReader( reader ) ) {
            std::cout << simple_rect_buffer_name << " fromReader has failed" << std::endl;
            problem |= 1;
        }
        
        problem |= compareImage2D( simple_rect_image, simple_rect_buff_image, simple_rect_buffer_name );
        
        Utilities::Buffer simple_rect_buffer_copy;
        
        {
            auto writer = simple_rect_buffer_copy.getWriter();
            
            if( simple_rect_buff_image.toWriter( writer ) ) {
                std::cout << simple_rect_buffer_name << " toWriter should have failed!" << std::endl;
                problem = 1;
            }
        }
        
        simple_rect_buff_image.addToBuffer( simple_rect_buffer_copy );
        
        // Compare the buffers
        problem |= compareBuffer( reader, simple_rect_buffer_copy.getReader(), simple_rect_buffer_name );
        
        {
            auto writer = simple_rect_buffer_copy.getWriter(0);
            
            // Clear the buffer
            while( !writer.ended() ) {
                writer.writeU8( 0 );
            }
            
            // The toWriter should not attempt to run if writer is used up.
            if( simple_rect_buff_image.toWriter( writer ) ) {
                std::cout << simple_rect_buffer_name << " toWriter should have failed!" << std::endl;
                problem = 1;
            }
            
            // Reset writer.
            writer.setPosition( 0 );
            
            // This time toWriter should succeed.
            if( !simple_rect_buff_image.toWriter( writer ) ) {
                std::cout << simple_rect_buffer_name << " toWriter should have succeeded!" << std::endl;
                problem = 1;
            }
            
            // Compare the buffers.
            problem |= compareBuffer( reader, simple_rect_buffer_copy.getReader(), simple_rect_buffer_name );
        }
    }
    // Implement this!
    problem |= testPalette2D<Utilities::ImagePaletteMorbin2D, 64, 64>( color_palette, "Morbin " );
    {
        Utilities::ColorPalette color_palette( color );
        
        color_palette.setAmount( 8 );
        
        color_palette.setIndex( 0, Utilities::PixelFormatColor::GenericColor( 0.0f, 0.0f, 0.0f, 1.0f ) );
        color_palette.setIndex( 1, Utilities::PixelFormatColor::GenericColor( 1.0f, 0.0f, 0.0f, 1.0f ) );
        color_palette.setIndex( 2, Utilities::PixelFormatColor::GenericColor( 1.0f, 1.0f, 0.0f, 1.0f ) );
        color_palette.setIndex( 3, Utilities::PixelFormatColor::GenericColor( 0.0f, 1.0f, 0.0f, 1.0f ) );
        color_palette.setIndex( 4, Utilities::PixelFormatColor::GenericColor( 0.0f, 1.0f, 1.0f, 1.0f ) );
        color_palette.setIndex( 5, Utilities::PixelFormatColor::GenericColor( 0.0f, 0.0f, 1.0f, 1.0f ) );
        color_palette.setIndex( 6, Utilities::PixelFormatColor::GenericColor( 1.0f, 0.0f, 1.0f, 1.0f ) );
        color_palette.setIndex( 7, Utilities::PixelFormatColor::GenericColor( 1.0f, 1.0f, 1.0f, 1.0f ) );
        
        std::string simple_rect_name = "Simple Rect Morbin Image One bit";
        const std::vector<bool> SIMPLE_RECT( {
            0, 0, 0, 0,
            1, 0, 0, 1,
            0, 1, 1, 1,
            0, 1, 1, 0
        } );
        
        {
            Utilities::ImagePaletteMorbin2D simple_rect_image( 8, 8, color_palette );
            
            if( simple_rect_image.fromBitfield( SIMPLE_RECT ) ) {
                std::cout << simple_rect_name << " bitfield succeeded to form." << std::endl;
                problem |= 1;
            }
        }
        
        Utilities::ImagePaletteMorbin2D simple_rect_image( 4, 4, color_palette );
        
        if( !simple_rect_image.fromBitfield( SIMPLE_RECT ) ) {
            std::cout << simple_rect_name << " bitfield failed to form." << std::endl;
            problem |= 1;
        }
        
        // Compare the image in morbin order
        problem |= compareColor( problem, 0, color_palette, simple_rect_image, 0, 0, simple_rect_name );
        problem |= compareColor( problem, 0, color_palette, simple_rect_image, 1, 0, simple_rect_name );
        problem |= compareColor( problem, 1, color_palette, simple_rect_image, 2, 0, simple_rect_name );
        problem |= compareColor( problem, 0, color_palette, simple_rect_image, 3, 0, simple_rect_name );
        problem |= compareColor( problem, 0, color_palette, simple_rect_image, 0, 1, simple_rect_name );
        problem |= compareColor( problem, 0, color_palette, simple_rect_image, 1, 1, simple_rect_name );
        problem |= compareColor( problem, 0, color_palette, simple_rect_image, 2, 1, simple_rect_name );
        problem |= compareColor( problem, 1, color_palette, simple_rect_image, 3, 1, simple_rect_name );
        problem |= compareColor( problem, 0, color_palette, simple_rect_image, 0, 2, simple_rect_name );
        problem |= compareColor( problem, 1, color_palette, simple_rect_image, 1, 2, simple_rect_name );
        problem |= compareColor( problem, 0, color_palette, simple_rect_image, 2, 2, simple_rect_name );
        problem |= compareColor( problem, 1, color_palette, simple_rect_image, 3, 2, simple_rect_name );
        problem |= compareColor( problem, 1, color_palette, simple_rect_image, 0, 3, simple_rect_name );
        problem |= compareColor( problem, 1, color_palette, simple_rect_image, 1, 3, simple_rect_name );
        problem |= compareColor( problem, 1, color_palette, simple_rect_image, 2, 3, simple_rect_name );
        problem |= compareColor( problem, 0, color_palette, simple_rect_image, 3, 3, simple_rect_name );
        
        std::string simple_3_bit_rect_name = "Simple Rect Morbin Image Three bit";
        const std::vector<bool> SIMPLE_3_BIT_RECT( {
            0, 0, 0,  0, 0, 1,
            0, 1, 0,  1, 0, 0
        } );
        
        {
            Utilities::ImagePaletteMorbin2D simple_rect_image( 4, 4, color_palette );
            
            if( simple_rect_image.fromBitfield( SIMPLE_3_BIT_RECT, 3 ) ) {
                std::cout << simple_3_bit_rect_name << " bitfield succeeded to form." << std::endl;
                problem |= 1;
            }
        }
        
        Utilities::ImagePaletteMorbin2D simple_rect_3_bit_image( 2, 2, color_palette );
        
        if( !simple_rect_3_bit_image.fromBitfield( SIMPLE_3_BIT_RECT, 3 ) ) {
            std::cout << simple_3_bit_rect_name << " bitfield failed to form." << std::endl;
            problem |= 1;
        }
        problem |= compareColor( problem, 0, color_palette, simple_rect_3_bit_image, 0, 0, simple_3_bit_rect_name );
        problem |= compareColor( problem, 1, color_palette, simple_rect_3_bit_image, 1, 0, simple_3_bit_rect_name );
        problem |= compareColor( problem, 2, color_palette, simple_rect_3_bit_image, 0, 1, simple_3_bit_rect_name );
        problem |= compareColor( problem, 4, color_palette, simple_rect_3_bit_image, 1, 1, simple_3_bit_rect_name );
    
        // Test ImagePalette2D.MorbinImage2D generation.
        /*auto image_copy = image.toColorMorbinImage();
        const std::string unpaletted_image = "Unpaletted Morbin Image";
        
        problem |= testScale<Utilities::ImageMorbin2D>( image_copy, image.getWidth(), image.getHeight(), unpaletted_image );
        problem |= compareImage2D<Utilities::ImagePalette2D, Utilities::ImageMorbin2D>( image, image_copy, unpaletted_image );*/
    }
    
    return problem;
}
